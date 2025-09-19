/* ------------------------------------------------------------
name: "test"
Code generated with Faust 2.75.7 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

static float mydsp_faustpower2_f(float value) {
    return value * value;
}

class mydsp{

private:

public:
    FAUSTFLOAT fButton0;
    FAUSTFLOAT fHslider0;
    int fSampleRate;
    float fConst0;
    FAUSTFLOAT fHslider1;
    FAUSTFLOAT fHslider2;
    int iRec1[2];
    float fRec0[3];

    mydsp() {
    }

//    void metadata(Meta* m) {
//        m->declare("compile_options", "-lang cpp -ct 1 -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
//        m->declare("filename", "test.dsp");
//        m->declare("filters.lib/fir:author", "Julius O. Smith III");
//        m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
//        m->declare("filters.lib/iir:author", "Julius O. Smith III");
//        m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
//        m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/name", "Faust Filters Library");
//        m->declare("filters.lib/resonlp:author", "Julius O. Smith III");
//        m->declare("filters.lib/resonlp:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/resonlp:license", "MIT-style STK-4.3 license");
//        m->declare("filters.lib/tf2:author", "Julius O. Smith III");
//        m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
//        m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
//        m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
//        m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
//        m->declare("filters.lib/version", "1.3.0");
//        m->declare("maths.lib/author", "GRAME");
//        m->declare("maths.lib/copyright", "GRAME");
//        m->declare("maths.lib/license", "LGPL with exception");
//        m->declare("maths.lib/name", "Faust Math Library");
//        m->declare("maths.lib/version", "2.8.0");
//        m->declare("name", "test");
//        m->declare("noises.lib/name", "Faust Noise Generator Library");
//        m->declare("noises.lib/version", "1.4.1");
//        m->declare("platform.lib/name", "Generic Platform Library");
//        m->declare("platform.lib/version", "1.3.0");
//    }

    virtual int getNumInputs() {
        return 0;
    }
    virtual int getNumOutputs() {
        return 1;
    }

    static void classInit(int sample_rate) {
    }

    virtual void instanceConstants(int sample_rate) {
        fSampleRate = sample_rate;
        fConst0 = 3.1415927f / std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
    }

    virtual void instanceResetUserInterface() {
        fButton0 = FAUSTFLOAT(0.0f);
        fHslider0 = FAUSTFLOAT(1.0f);
        fHslider1 = FAUSTFLOAT(5e+02f);
        fHslider2 = FAUSTFLOAT(5.0f);
    }

    virtual void instanceClear() {
        for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
            iRec1[l0] = 0;
        }
        for (int l1 = 0; l1 < 3; l1 = l1 + 1) {
            fRec0[l1] = 0.0f;
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

    virtual mydsp* clone() {
        return new mydsp();
    }

    virtual int getSampleRate() {
        return fSampleRate;
    }

//    virtual void buildUserInterface(UI* ui_interface) {
//        ui_interface->openVerticalBox("test");
//        ui_interface->declare(&fHslider1, "0", "");
//        ui_interface->addHorizontalSlider("cutoffFrequency", &fHslider1, FAUSTFLOAT(5e+02f), FAUSTFLOAT(5e+01f), FAUSTFLOAT(1e+04f), FAUSTFLOAT(0.01f));
//        ui_interface->declare(&fHslider2, "1", "");
//        ui_interface->addHorizontalSlider("q", &fHslider2, FAUSTFLOAT(5.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(3e+01f), FAUSTFLOAT(0.1f));
//        ui_interface->declare(&fHslider0, "2", "");
//        ui_interface->addHorizontalSlider("gain", &fHslider0, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
//        ui_interface->declare(&fButton0, "3", "");
//        ui_interface->addButton("gate", &fButton0);
//        ui_interface->closeBox();
//    }

    virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
        FAUSTFLOAT* output0 = outputs[0];
        float fSlow0 = std::tan(fConst0 * float(fHslider1));
        float fSlow1 = 1.0f / fSlow0;
        float fSlow2 = 1.0f / float(fHslider2);
        float fSlow3 = (fSlow1 + fSlow2) / fSlow0 + 1.0f;
        float fSlow4 = float(fButton0) * float(fHslider0) / fSlow3;
        float fSlow5 = 1.0f / fSlow3;
        float fSlow6 = (fSlow1 - fSlow2) / fSlow0 + 1.0f;
        float fSlow7 = 2.0f * (1.0f - 1.0f / mydsp_faustpower2_f(fSlow0));
        for (int i0 = 0; i0 < count; i0 = i0 + 1) {
            iRec1[0] = 1103515245 * iRec1[1] + 12345;
            fRec0[0] = 4.656613e-10f * float(iRec1[0]) - fSlow5 * (fSlow6 * fRec0[2] + fSlow7 * fRec0[1]);
            output0[i0] = FAUSTFLOAT(fSlow4 * (fRec0[2] + 2.0f * fRec0[1] + fRec0[0]));
            iRec1[1] = iRec1[0];
            fRec0[2] = fRec0[1];
            fRec0[1] = fRec0[0];
        }
    }

};

class dspmain : public mydsp {
public:
    dspmain() : mydsp() {
        // Initialize any additional variables or states if necessary
    }

    // Method to set the gain parameter
    void setGain(FAUSTFLOAT gain) {
        fHslider0 = gain;
    }

    // Method to set the cutoff frequency
    void setCutoffFrequency(FAUSTFLOAT cutoff) {
        fHslider1 = cutoff;
    }

    // Method to set the Q factor
    void setQFactor(FAUSTFLOAT q) {
        fHslider2 = q;
    }

    // Method to toggle the gate button
    void toggleGate() {
        fButton0 = (fButton0 == 0.0f) ? 1.0f : 0.0f;
    }

    // Override the compute method if additional processing is needed
    virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) override {
        // Optionally, add pre-processing or post-processing here

        // Call the base class compute method to perform DSP
        mydsp::compute(count, inputs, outputs);

        // Optionally, add additional processing after DSP computation
    }

    // Add any additional control methods as needed
};



#endif
