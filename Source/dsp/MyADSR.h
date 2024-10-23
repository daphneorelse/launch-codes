/*
  ==============================================================================

    MyADSR.h
    Created: 9 Jun 2024 1:02:25pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MyADSR : public juce::ValueTree::Listener
{
public:
    MyADSR(juce::ValueTree t_synthTree, const juce::Identifier& t_propertyName, juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& tables)
    : synthTree(t_synthTree)
    , propertyName(t_propertyName)
    , envTables(tables)
    {
        synthTree.addListener(this);
        
        envMode.store(synthTree[propertyName]);
        
        recalculateRates();
    }
    
    ~MyADSR()
    {
        synthTree.removeListener(this);
    }
    
    struct Parameters
    {
        Parameters() = default;

        // TODO maybe delete and only use default constructor
        Parameters (float attackTimeSeconds,
                    float decayTimeSeconds,
                    float releaseTimeSeconds,
                    float initialLevel,
                    float peakLevel,
                    float sustainLevel,
                    float stealReleaseTimeSeconds = 0.0f,
                    float endLevel = 0.0f)
            : attack (attackTimeSeconds),
              decay (decayTimeSeconds),
              release (releaseTimeSeconds),
              stealRelease (stealReleaseTimeSeconds),
              initial (initialLevel),
              peak (peakLevel),
              sustain (sustainLevel),
              end (endLevel)
        {
        }

        float attack = 0.1f, decay = 0.1f, release = 0.1f, stealRelease = 0.0f, initial = 0.0f, peak = 1.0f, sustain = 0.75f, end = 0.0f;
    };

    void setParameters (const Parameters& newParameters)
    {
        jassert (sampleRate > 0.0);

        parameters = newParameters;
        recalculateRates();
        
        // TODO think about simply setting parameters directly without Parameters class
    }

    const Parameters& getParameters() const noexcept  { return parameters; }

    bool isActive() const noexcept                    { return state != State::idle; }

    void setSampleRate (double newSampleRate) noexcept
    {
        jassert (newSampleRate > 0.0);
        sampleRate = newSampleRate;
    }

    void reset() noexcept
    {
        state = State::idle;
        envelopeInput = 0.0f;
    }

    void noteOn() noexcept
    {
        if (state != State::idle)
        {
            releaseStartAmp = envelopeOutput;
            envelopeInput = 0.0f;
            state = State::stealRelease;
        }
        else
            state = State::attack;
    }

    void noteOff() noexcept
    {
        if (state != State::idle)
        {
            releaseStartAmp = envelopeOutput;
            
            envelopeInput = 0.0f;
            state = State::release;
        }
    }

    float getNextSample() noexcept
    {
        switch (state)
        {
            case State::idle:
            {
                return parameters.end;
            }
                
            case State::stealRelease:
            {
                envelopeOutput = releaseStartAmp + envelopeInput * (parameters.initial - releaseStartAmp);
                
                envelopeInput += stealReleaseRate;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::attack:
            {
                envelopeOutput = envTables[0]->processSample(envelopeInput) * (parameters.peak - parameters.initial) + parameters.initial;
                
                envelopeInput += attackRate;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::decay:
            {
                envelopeOutput = envTables[1]->processSample(envelopeInput) * (parameters.sustain - parameters.peak) + parameters.peak;
                
                envelopeInput += decayRate;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::sustain:
            {
                return parameters.sustain;
            }

            case State::release:
            {
                envelopeOutput = envTables[2]->processSample(envelopeInput) * (parameters.end - releaseStartAmp) + releaseStartAmp;
                
                envelopeInput += releaseRate;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }
        }

        return envelopeOutput;
    }
    
    // returns current env value and skips numSamples ahead
    float getNextSamples(const int numSamples) noexcept
    {
        switch (state)
        {
            case State::idle:
            {
                return parameters.end;
            }
                
            case State::stealRelease:
            {
                envelopeOutput = releaseStartAmp + envelopeInput * (parameters.initial - releaseStartAmp);
                
                envelopeInput += stealReleaseRate * numSamples;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::attack:
            {
                envelopeOutput = envTables[0]->processSample(envelopeInput) * (parameters.peak - parameters.initial) + parameters.initial;
                
                envelopeInput += attackRate * numSamples;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::decay:
            {
                envelopeOutput = envTables[1]->processSample(envelopeInput) * (parameters.sustain - parameters.peak) + parameters.peak;
                
                envelopeInput += decayRate * numSamples;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }

            case State::sustain:
            {
                return parameters.sustain;
            }

            case State::release:
            {
                envelopeOutput = envTables[2]->processSample(envelopeInput) * (parameters.end - releaseStartAmp) + releaseStartAmp;
                
                envelopeInput += releaseRate * numSamples;
                
                if (envelopeInput >= 1.0f)
                    goToNextState();
                
                break;
            }
        }

        return envelopeOutput;
    }

    template <typename FloatType>
    void applyEnvelopeToBuffer (juce::AudioBuffer<FloatType>& buffer, int startSample, int numSamples)
    {
        jassert (startSample + numSamples <= buffer.getNumSamples());

        if (state == State::idle)
        {
            buffer.clear (startSample, numSamples);
            return;
        }

        if (state == State::sustain)
        {
            buffer.applyGain (startSample, numSamples, parameters.sustain);
            return;
        }

        auto numChannels = buffer.getNumChannels();

        while (--numSamples >= 0)
        {
            auto env = getNextSample();

            for (int i = 0; i < numChannels; ++i)
                buffer.getWritePointer (i)[startSample] *= env;

            ++startSample;
        }
    }
    
    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        if (property == propertyName)
            envMode.store(synthTree[propertyName]);
    }

private:
    void recalculateRates() noexcept
    {
        attackRate       = 1.0f / (parameters.attack * sampleRate);
        decayRate        = 1.0f / (parameters.decay * sampleRate);
        releaseRate      = 1.0f / (parameters.release * sampleRate);
        stealReleaseRate = parameters.stealRelease == 0.0f ? 1.0f : 1.0f / (parameters.stealRelease * sampleRate);
        // TODO there are problems with steal release... sometimes attacks sound more dull, and changing sustain mid-note leads to unexpected behavior
    }

    void goToNextState() noexcept
    {
        envelopeInput = 0.0f;
        
        if (state == State::stealRelease)
        {
            state = State::attack;
            return;
        }
        if (state == State::attack)
        {
            state = State::decay;
            return;
        }

        if (state == State::decay)
        {
            if (envMode.load())
            {
                releaseStartAmp = envelopeOutput;
                state = State::stealRelease;
            }
            else
                state = State::sustain;
            return;
        }

        if (state == State::release)
            reset();
    }

    enum class State { attack, decay, release, sustain, idle, stealRelease };

    State state = State::idle;
    std::atomic<int> envMode;
    
    juce::ValueTree synthTree;
    const juce::Identifier& propertyName;
    juce::OwnedArray<juce::dsp::LookupTableTransform<float>>& envTables;
    Parameters parameters;

    double sampleRate = 44100.0;
    float envelopeInput = 0.0f, envelopeOutput = 0.0f, attackRate = 0.0f, decayRate = 0.0f, releaseRate = 0.0f, releaseStartAmp = 0.0f, stealReleaseRate = 0.0f;
};
