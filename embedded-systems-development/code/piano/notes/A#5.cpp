/* ------------------------------------------------------------
name: "A#5"
Code generated with Faust 2.54.9 (https://faust.grame.fr)
Compilation options: -a alsa-console.cpp -lang cpp -es 1 -mcd 16 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************ 
 IMPORTANT NOTE : this file contains two clearly delimited sections :
 the ARCHITECTURE section (in two parts) and the USER section. Each section
 is governed by its own copyright and license. Please check individually
 each section for license and copyright information.
 *************************************************************************/

/******************* BEGIN alsa-console.cpp ****************/

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2019 GRAME, Centre National de Creation Musicale
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
 
 ************************************************************************
 ************************************************************************/

#include <libgen.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <cmath>

#include "faust/dsp/timed-dsp.h"
#include "faust/gui/FUI.h"
#include "faust/misc.h"
#include "faust/gui/GUI.h"
#include "faust/gui/JSONUI.h"
#include "faust/gui/console.h"
#include "faust/audio/alsa-dsp.h"

#ifdef OSCCTRL
#include "faust/gui/OSCUI.h"
#endif

#ifdef HTTPCTRL
#include "faust/gui/httpdUI.h"
#endif

// Always include this file, otherwise -nvoices only mode does not compile....
#include "faust/gui/MidiUI.h"

#ifdef MIDICTRL
#include "faust/midi/rt-midi.h"
#include "faust/midi/RtMidi.cpp"
#endif

/******************************************************************************
 *******************************************************************************
 
 VECTOR INTRINSICS
 
 *******************************************************************************
 *******************************************************************************/


/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

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

class mydspSIG0 {
	
  private:
	
	int iVec0[2];
	int iRec0[2];
	
  public:
	
	int getNumInputsmydspSIG0() {
		return 0;
	}
	int getNumOutputsmydspSIG0() {
		return 1;
	}
	
	void instanceInitmydspSIG0(int sample_rate) {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			iRec0[l1] = 0;
		}
	}
	
	void fillmydspSIG0(int count, float* table) {
		for (int i1 = 0; i1 < count; i1 = i1 + 1) {
			iVec0[0] = 1;
			iRec0[0] = (iVec0[1] + iRec0[1]) % 65536;
			table[i1] = std::sin(9.58738e-05f * float(iRec0[0]));
			iVec0[1] = iVec0[0];
			iRec0[1] = iRec0[0];
		}
	}

};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

static float ftbl0mydspSIG0[65536];

class mydsp : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	float fRec1[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.9");
		m->declare("compile_options", "-a alsa-console.cpp -lang cpp -es 1 -mcd 16 -single -ftz 0");
		m->declare("filename", "A#5.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.5");
		m->declare("name", "A#5");
		m->declare("options", "[osc:on]");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.3");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.3");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	
	static void classInit(int sample_rate) {
		mydspSIG0* sig0 = newmydspSIG0();
		sig0->instanceInitmydspSIG0(sample_rate);
		sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
		deletemydspSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = 932.33f / std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
	}
	
	virtual void instanceResetUserInterface() {
	}
	
	virtual void instanceClear() {
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
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("A#5");
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec1[0] = fConst0 + (fRec1[1] - std::floor(fConst0 + fRec1[1]));
			output0[i0] = FAUSTFLOAT(ftbl0mydspSIG0[std::max<int>(0, std::min<int>(int(65536.0f * fRec1[0]), 65535))]);
			fRec1[1] = fRec1[0];
		}
	}

};

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

#include "faust/dsp/poly-dsp.h"

#ifdef POLY2
#include "faust/dsp/dsp-combiner.h"
#include "effect.h"
#endif

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

using namespace std;

dsp* DSP;

list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

//-------------------------------------------------------------------------
// 									MAIN
//-------------------------------------------------------------------------

static bool hasMIDISync()
{
    JSONUI jsonui;
    mydsp* tmp_dsp = new mydsp();
    tmp_dsp->buildUserInterface(&jsonui);
    string json = jsonui.JSON();
    delete tmp_dsp;

    return ((json.find("midi") != string::npos) &&
            ((json.find("start") != string::npos) ||
            (json.find("stop") != string::npos) ||
            (json.find("clock") != string::npos)));
}

int main(int argc, char *argv[] )
{
    char* name = basename (argv [0]);
    char rcfilename[256];
    char* home = getenv("HOME");
    int nvoices = 0;
    snprintf(rcfilename, 256, "%s/.%src", home, name);

#ifdef POLY2
    nvoices = lopt(argv, "--nvoices", nvoices);
    int group = lopt(argv, "--group", 1);
    cout << "Started with " << nvoices << " voices\n";
    DSP = new mydsp_poly(new mydsp(), nvoices, true, group);

#if MIDICTRL
    if (hasMIDISync()) {
        DSP = new timed_dsp(new dsp_sequencer(DSP, new effect()));
    } else {
        DSP = new dsp_sequencer(DSP, new effect());
    }
#else
    DSP = new dsp_sequencer(DSP, new effect());
#endif
    
#else
    nvoices = lopt(argv, "--nvoices", nvoices);
    int group = lopt(argv, "--group", 1);
    
    if (nvoices > 0) {
        cout << "Started with " << nvoices << " voices\n";
        DSP = new mydsp_poly(new mydsp(), nvoices, true, group);
        
#if MIDICTRL
        if (hasMIDISync()) {
            DSP = new timed_dsp(DSP);
        }
#endif
    } else {
#if MIDICTRL
        if (hasMIDISync()) {
            DSP = new timed_dsp(new mydsp());
        } else {
            DSP = new mydsp();
        }
#else
        DSP = new mydsp();
#endif
    }
#endif
    
    if (!DSP) {
        cerr << "Unable to allocate Faust DSP object" << endl;
        exit(1);
    }

    CMDUI* interface = new CMDUI(argc, argv, true);
    FUI* finterface	= new FUI();
    DSP->buildUserInterface(interface);
    DSP->buildUserInterface(finterface);

#ifdef MIDICTRL
    rt_midi midi_handler(name);
    MidiUI midiinterface(&midi_handler);
    DSP->buildUserInterface(&midiinterface);
    cout << "MIDI is on" << endl;
#endif

#ifdef HTTPCTRL
    httpdUI* httpdinterface = new httpdUI(name, DSP->getNumInputs(), DSP->getNumOutputs(), argc, argv);
    DSP->buildUserInterface(httpdinterface);
    cout << "HTTPD is on" << endl;
#endif

#ifdef OSCCTRL
    GUI* oscinterface = new OSCUI(name, argc, argv);
    DSP->buildUserInterface(oscinterface);
#endif

    alsaaudio audio(argc, argv, DSP);
    if (!audio.init(name, DSP)) {
        cerr << "Unable to init audio" << endl;
        exit(1);
    }
    
    finterface->recallState(rcfilename);
    
    if (!audio.start()) {
        cerr << "Unable to start audio" << endl;
        exit(1);
    }

#ifdef HTTPCTRL
    httpdinterface->run();
#endif

#ifdef OSCCTRL
    oscinterface->run();
#endif
#ifdef MIDICTRL
    if (!midiinterface.run()) {
        cerr << "MidiUI run error\n";
    }
#endif
    interface->run();

#ifdef MIDICTRL
    midiinterface.stop();
#endif

    audio.stop();
    finterface->saveState(rcfilename);

    // desallocation
    delete interface;
    delete finterface;
#ifdef HTTPCTRL
    delete httpdinterface;
#endif
#ifdef OSCCTRL
    delete oscinterface;
#endif

    return 0;
}

/******************** END alsa-console.cpp ****************/

#endif
