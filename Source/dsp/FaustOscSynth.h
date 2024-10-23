/* ------------------------------------------------------------
name: "FaustOscSynth"
Code generated with Faust 2.23.4 (https://faust.grame.fr)
Compilation options: -lang cpp -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#include <cmath>
#include <cstring>

/************************** BEGIN MapUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>

/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN PathBuilder.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_PATHBUILDER_H
#define FAUST_PATHBUILDER_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class PathBuilder
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            std::replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        void pushLabel(const std::string& label) { fControlsLevel.push_back(label); }
        void popLabel() { fControlsLevel.pop_back(); }
    
};

#endif  // FAUST_PATHBUILDER_H
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * MapUI : Faust User Interface
 * This class creates a map of complete hierarchical path and zones for each UI items.
 ******************************************************************************/

class MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Complete path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
        
        // set/get
        void setParamValue(const std::string& path, FAUSTFLOAT value)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                *fPathZoneMap[path] = value;
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                *fLabelZoneMap[path] = value;
            }
        }
        
        FAUSTFLOAT getParamValue(const std::string& path)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                return *fPathZoneMap[path];
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                return *fLabelZoneMap[path];
            } else {
                return FAUSTFLOAT(0);
            }
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getMap() { return fPathZoneMap; }
        
        int getParamsCount() { return int(fPathZoneMap.size()); }
        
        std::string getParamAddress(int index)
        { 
            std::map<std::string, FAUSTFLOAT*>::iterator it = fPathZoneMap.begin();
            while (index-- > 0 && it++ != fPathZoneMap.end()) {}
            return (*it).first;
        }
    
        std::string getParamAddress(FAUSTFLOAT* zone)
        {
            std::map<std::string, FAUSTFLOAT*>::iterator it = fPathZoneMap.begin();
            do {
                if ((*it).second == zone) return (*it).first;
            }
            while (it++ != fPathZoneMap.end());
            return "";
        }
    
        static bool endsWith(std::string const& str, std::string const& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
};


#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
    
};

#endif
/**************************  END  meta.h **************************/
/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceInit(int sample_rate) = 0;

        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hertz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/

// BEGIN-FAUSTDSP

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

//#ifndef FAUSTCLASS
//#define FAUSTCLASS mydsp
//#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

class mydspSIG0 {
    
  private:
    
    int iVec1[2];
    int iRec2[2];
    
  public:
    
    int getNumInputsmydspSIG0() {
        return 0;
    }
    int getNumOutputsmydspSIG0() {
        return 1;
    }
    
    void instanceInitmydspSIG0(int sample_rate) {
        for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
            iVec1[l3] = 0;
        }
        for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
            iRec2[l4] = 0;
        }
    }
    
    void fillmydspSIG0(int count, float* table) {
        for (int i1 = 0; i1 < count; i1 = i1 + 1) {
            iVec1[0] = 1;
            iRec2[0] = (iVec1[1] + iRec2[1]) % 65536;
            table[i1] = std::sin(9.58738e-05f * float(iRec2[0]));
            iVec1[1] = iVec1[0];
            iRec2[1] = iRec2[0];
        }
    }

};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

static float ftbl0mydspSIG0[65536];

class oscDsp : public dsp {
    
 private:
    
    FAUSTFLOAT fHslider0;
    int fSampleRate;
    float fConst0;
    float fConst1;
    int iVec0[2];
    float fRec0[2];
    FAUSTFLOAT fEntry0;
    float fRec1[2];
    float fConst2;
    
 public:
    oscDsp() {
    }
    
    void metadata(Meta* m) {
        m->declare("basics.lib/name", "Faust Basic Element Library");
        m->declare("basics.lib/tabulateNd", "Copyright (C) 2023 Bart Brouns <bart@magnetophon.nl>");
        m->declare("basics.lib/version", "1.19.1");
        m->declare("compile_options", "-lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
        m->declare("filename", "oscillators.dsp");
        m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/name", "Faust Filters Library");
        m->declare("filters.lib/pole:author", "Julius O. Smith III");
        m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/version", "1.3.0");
        m->declare("maths.lib/author", "GRAME");
        m->declare("maths.lib/copyright", "GRAME");
        m->declare("maths.lib/license", "LGPL with exception");
        m->declare("maths.lib/name", "Faust Math Library");
        m->declare("maths.lib/version", "2.8.0");
        m->declare("name", "oscillators");
        m->declare("oscillators.lib/name", "Faust Oscillator Library");
        m->declare("oscillators.lib/polyblep:author", "Jacek Wieczorek");
        m->declare("oscillators.lib/polyblep_saw:author", "Jacek Wieczorek");
        m->declare("oscillators.lib/polyblep_square:author", "Jacek Wieczorek");
        m->declare("oscillators.lib/polyblep_triangle:author", "Jacek Wieczorek");
        m->declare("oscillators.lib/version", "1.5.1");
        m->declare("platform.lib/name", "Generic Platform Library");
        m->declare("platform.lib/version", "1.3.0");
    }

    virtual int getNumInputs() {
        return 0;
    }
    virtual int getNumOutputs() {
        return 2;
    }
    
    static void classInit(int sample_rate) {
        mydspSIG0* sig0 = newmydspSIG0();
        sig0->instanceInitmydspSIG0(sample_rate);
        sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
        deletemydspSIG0(sig0);
    }
    
    virtual void instanceConstants(int sample_rate) {
        fSampleRate = sample_rate;
        fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
        fConst1 = 1.0f / fConst0;
        fConst2 = 4.0f / fConst0;
    }
    
    virtual void instanceResetUserInterface() {
        fHslider0 = FAUSTFLOAT(4.4e+02f);
        fEntry0 = FAUSTFLOAT(0.0f);
    }
    
    virtual void instanceClear() {
        for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
            iVec0[l0] = 0;
        }
        for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
            fRec0[l1] = 0.0f;
        }
        for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
            fRec1[l2] = 0.0f;
        }
    }
    
    virtual void init(int sample_rate) {
        classInit(sample_rate);
        instanceInit(sample_rate);
    }
    
    virtual void instanceInit(int sample_rate) {
        instanceConstants(sample_rate);
        instanceResetUserInterface();
        instanceClear();
    }
    
    virtual oscDsp* clone() {
        return new oscDsp();
    }
    
    virtual int getSampleRate() {
        return fSampleRate;
    }
    
    virtual void buildUserInterface(UI* ui_interface) {
        ui_interface->openVerticalBox("oscillators");
        ui_interface->addHorizontalSlider("freq", &fHslider0, FAUSTFLOAT(4.4e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(2e+04f), FAUSTFLOAT(0.1f));
        ui_interface->addNumEntry("wave", &fEntry0, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(3.0f), FAUSTFLOAT(1.0f));
        ui_interface->closeBox();
    }
    
    virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
        FAUSTFLOAT* output0 = outputs[0];
        FAUSTFLOAT* output1 = outputs[1];
        float fSlow0 = float(fHslider0);
        float fSlow1 = fConst1 * fSlow0;
        float fSlow2 = 1.0f - fSlow1;
        float fSlow3 = fConst0 / fSlow0;
        float fSlow4 = float(fEntry0);
        float fSlow5 = float(fSlow4 == 3.0f);
        float fSlow6 = float(fSlow4 == 2.0f);
        float fSlow7 = fConst2 * fSlow0 * float(fSlow4 == 1.0f);
        float fSlow8 = float(fSlow4 == 0.0f);
        for (int i0 = 0; i0 < count; i0 = i0 + 1) {
            iVec0[0] = 1;
            float fTemp0 = ((1 - iVec0[1]) ? 0.0f : fSlow1 + fRec0[1]);
            fRec0[0] = fTemp0 - std::floor(fTemp0);
            int iTemp1 = (fRec0[0] < fSlow1) + 2 * (fRec0[0] > fSlow2);
            float fTemp2 = fRec0[0] + -1.0f;
            float fTemp3 = ((iTemp1 == 0) ? 0.0f : ((iTemp1 == 1) ? fSlow3 * fRec0[0] * (2.0f - fSlow3 * fRec0[0]) + -1.0f : fSlow3 * fTemp2 * (fSlow3 * fTemp2 + 2.0f) + 1.0f));
            float fTemp4 = std::fmod(std::fmod(fRec0[0] + 0.5f, 1.0f) + 1.0f, 1.0f);
            int iTemp5 = (fTemp4 < fSlow1) + 2 * (fTemp4 > fSlow2);
            float fTemp6 = fTemp4 + -1.0f;
            float fTemp7 = ((iTemp5 == 0) ? 0.0f : ((iTemp5 == 1) ? fSlow3 * fTemp4 * (2.0f - fSlow3 * fTemp4) + -1.0f : fSlow3 * fTemp6 * (fSlow3 * fTemp6 + 2.0f) + 1.0f));
            float fTemp8 = 2.0f * fRec0[0];
            float fTemp9 = float(2 * int(fTemp8) + -1);
            fRec1[0] = fTemp7 + 0.999f * fRec1[1] + fTemp9 - fTemp3;
            float fTemp10 = 0.25f * (fSlow8 * ftbl0mydspSIG0[std::max<int>(0, std::min<int>(int(65536.0f * fRec0[0]), 65535))] + fSlow7 * fRec1[0] + fSlow6 * (fTemp8 + (-1.0f - fTemp3)) + fSlow5 * (fTemp9 + fTemp7 - fTemp3));
            output0[i0] = FAUSTFLOAT(fTemp10);
            output1[i0] = FAUSTFLOAT(fTemp10);
            iVec0[1] = iVec0[0];
            fRec0[1] = fRec0[0];
            fRec1[1] = fRec1[0];
        }
    }

};

static float mydsp_faustpower2_f(float value) {
    return value * value;
}

class noiseDsp : public dsp {
    
 private:
    
    int iRec0[2];
    FAUSTFLOAT fEntry0;
    FAUSTFLOAT fHslider0;
    FAUSTFLOAT fHslider1;
    int fSampleRate;
    float fConst0;
    FAUSTFLOAT fHslider2;
    float fRec1[3];
    
 public:
    noiseDsp() {
    }
    
    void metadata(Meta* m) {
        m->declare("compile_options", "-lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
        m->declare("filename", "noiseGen.dsp");
        m->declare("filters.lib/fir:author", "Julius O. Smith III");
        m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/iir:author", "Julius O. Smith III");
        m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/name", "Faust Filters Library");
        m->declare("filters.lib/resonbp:author", "Julius O. Smith III");
        m->declare("filters.lib/resonbp:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/resonbp:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/resonhp:author", "Julius O. Smith III");
        m->declare("filters.lib/resonhp:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/resonhp:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/resonlp:author", "Julius O. Smith III");
        m->declare("filters.lib/resonlp:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/resonlp:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/tf2:author", "Julius O. Smith III");
        m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
        m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
        m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
        m->declare("filters.lib/version", "1.3.0");
        m->declare("maths.lib/author", "GRAME");
        m->declare("maths.lib/copyright", "GRAME");
        m->declare("maths.lib/license", "LGPL with exception");
        m->declare("maths.lib/name", "Faust Math Library");
        m->declare("maths.lib/version", "2.8.0");
        m->declare("name", "noiseGen");
        m->declare("noises.lib/name", "Faust Noise Generator Library");
        m->declare("noises.lib/version", "1.4.1");
        m->declare("platform.lib/name", "Generic Platform Library");
        m->declare("platform.lib/version", "1.3.0");
    }

    virtual int getNumInputs() {
        return 0;
    }
    virtual int getNumOutputs() {
        return 2;
    }
    
    static void classInit(int sample_rate) {
    }
    
    virtual void instanceConstants(int sample_rate) {
        fSampleRate = sample_rate;
        fConst0 = 3.1415927f / std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
    }
    
    virtual void instanceResetUserInterface() {
        fEntry0 = FAUSTFLOAT(0.0f);
        fHslider0 = FAUSTFLOAT(0.1f);
        fHslider1 = FAUSTFLOAT(2e+04f);
        fHslider2 = FAUSTFLOAT(1.0f);
    }
    
    virtual void instanceClear() {
        for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
            iRec0[l0] = 0;
        }
        for (int l1 = 0; l1 < 3; l1 = l1 + 1) {
            fRec1[l1] = 0.0f;
        }
    }
    
    virtual void init(int sample_rate) {
        classInit(sample_rate);
        instanceInit(sample_rate);
    }
    
    virtual void instanceInit(int sample_rate) {
        instanceConstants(sample_rate);
        instanceResetUserInterface();
        instanceClear();
    }
    
    virtual noiseDsp* clone() {
        return new noiseDsp();
    }
    
    virtual int getSampleRate() {
        return fSampleRate;
    }
    
    virtual void buildUserInterface(UI* ui_interface) {
        ui_interface->openVerticalBox("noiseGen");
        ui_interface->addHorizontalSlider("cutoff", &fHslider1, FAUSTFLOAT(2e+04f), FAUSTFLOAT(3e+01f), FAUSTFLOAT(2e+04f), FAUSTFLOAT(0.1f));
        ui_interface->addNumEntry("fil_type", &fEntry0, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(3.0f), FAUSTFLOAT(1.0f));
        ui_interface->addHorizontalSlider("gain", &fHslider0, FAUSTFLOAT(0.1f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
        ui_interface->addHorizontalSlider("q", &fHslider2, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.1f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(0.01f));
        ui_interface->closeBox();
    }
    
    virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
        FAUSTFLOAT* output0 = outputs[0];
        FAUSTFLOAT* output1 = outputs[1];
        float fSlow0 = float(fEntry0);
        float fSlow1 = float(fHslider0);
        float fSlow2 = 4.656613e-10f * fSlow1 * float(fSlow0 == 3.0f);
        float fSlow3 = std::tan(fConst0 * float(fHslider1));
        float fSlow4 = 2.0f * (1.0f - 1.0f / mydsp_faustpower2_f(fSlow3));
        float fSlow5 = 1.0f / float(fHslider2);
        float fSlow6 = 1.0f / fSlow3;
        float fSlow7 = (fSlow6 - fSlow5) / fSlow3 + 1.0f;
        float fSlow8 = 1.0f / ((fSlow5 + fSlow6) / fSlow3 + 1.0f);
        float fSlow9 = 4.656613e-10f * fSlow1;
        float fSlow10 = float(fSlow0 == 2.0f) / fSlow3;
        float fSlow11 = float(fSlow0 == 0.0f);
        float fSlow12 = float(fSlow0 == 1.0f);
        for (int i0 = 0; i0 < count; i0 = i0 + 1) {
            iRec0[0] = 1103515245 * iRec0[1] + 12345;
            float fTemp0 = float(iRec0[0]);
            float fTemp1 = fSlow9 * fTemp0;
            fRec1[0] = fTemp1 - fSlow8 * (fSlow7 * fRec1[2] + fSlow4 * fRec1[1]);
            float fTemp2 = fRec1[2] + fRec1[0] + 2.0f * fRec1[1];
            float fTemp3 = fSlow12 * (fTemp1 - fSlow8 * fTemp2) + fSlow8 * (fSlow11 * fTemp2 + fSlow10 * (fRec1[0] - fRec1[2])) + fSlow2 * fTemp0;
            output0[i0] = FAUSTFLOAT(fTemp3);
            output1[i0] = FAUSTFLOAT(fTemp3);
            iRec0[1] = iRec0[0];
            fRec1[2] = fRec1[1];
            fRec1[1] = fRec1[0];
        }
    }

};


// END-FAUSTDSP

#endif
