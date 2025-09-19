#include "SynthGUI.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_message.H>
#include <iostream>

SynthGUI::SynthGUI(Parameters* sharedParams)
        : window(nullptr), messageBox(nullptr), params(sharedParams) {}

SynthGUI::~SynthGUI() {
    if (window) {
        window->hide();
        delete window;
    }
}

void SynthGUI::create() {
    window = new Fl_Window(400, 400, "faust synth test");

    // Gain Slider
    gainSlider = new Fl_Slider(50, 50, 300, 30, "Gain");
    gainSlider->type(FL_HOR_NICE_SLIDER);
    gainSlider->bounds(0.0, 1.0);
    gainSlider->value(params->gain.load());
    gainSlider->callback(cb_gain, this);

    // Cutoff Frequency Slider
    cutoffSlider = new Fl_Slider(50, 100, 300, 30, "Cutoff Frequency");
    cutoffSlider->type(FL_HOR_NICE_SLIDER);
    cutoffSlider->bounds(50.0, 10000.0);
    cutoffSlider->value(params->cutoffFrequency.load());
    cutoffSlider->callback(cb_cutoff, this);

    // Q Factor Slider
    qSlider = new Fl_Slider(50, 150, 300, 30, "Q Factor");
    qSlider->type(FL_HOR_NICE_SLIDER);
    qSlider->bounds(1.0, 30.0);
    qSlider->value(params->qFactor.load());
    qSlider->callback(cb_q, this);

    // Gate Button
    gateButton = new Fl_Button(150, 200, 100, 30, "Gate");
    gateButton->callback(cb_gate, this);

    // Message Box
    messageBox = new Fl_Box(50, 250, 300, 30, "Status: Ready");
    messageBox->box(FL_DOWN_FRAME);
    messageBox->labelfont(FL_BOLD + FL_ITALIC);
    messageBox->labelsize(12);
    messageBox->align(FL_ALIGN_CENTER);

    window->end();
}

void SynthGUI::show() {
    window->show();
}

void SynthGUI::displayMessage(const std::string &msg) {
    if (messageBox) {
        messageBox->label(msg.c_str());
        window->redraw();
    }
}

// Callback implementations
void SynthGUI::cb_gain(Fl_Widget* widget, void* data) {
    SynthGUI* gui = static_cast<SynthGUI*>(data);
    float value = gui->gainSlider->value();
    gui->params->gain.store(value);
    gui->displayMessage("Gain set");
}

void SynthGUI::cb_cutoff(Fl_Widget* widget, void* data) {
    SynthGUI* gui = static_cast<SynthGUI*>(data);
    float value = gui->cutoffSlider->value();
    gui->params->cutoffFrequency.store(value);
    gui->displayMessage("Cutoff Frequency set");
}

void SynthGUI::cb_q(Fl_Widget* widget, void* data) {
    SynthGUI* gui = static_cast<SynthGUI*>(data);
    float value = gui->qSlider->value();
    gui->params->qFactor.store(value);
    gui->displayMessage("Q Factor set to " + std::to_string(value));
}

void SynthGUI::cb_gate(Fl_Widget* widget, void* data) {
    SynthGUI* gui = static_cast<SynthGUI*>(data);
    bool currentGate = gui->params->gate.load();
    gui->params->gate.store(!currentGate); // Toggle gate
    gui->displayMessage("Gate " + std::string(currentGate ? "Off" : "On"));
}
