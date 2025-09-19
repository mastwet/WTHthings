#ifndef SYNTHGUI_H
#define SYNTHGUI_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Slider.H>
#include <string>
#include "Parameters.h"

class SynthGUI {
public:
    SynthGUI(Parameters* sharedParams);
    ~SynthGUI();
    void create();
    void show();
    void displayMessage(const std::string &msg);

private:
    Fl_Window *window;
    Fl_Box *messageBox;
    Fl_Slider *gainSlider;
    Fl_Slider *cutoffSlider;
    Fl_Slider *qSlider;
    Fl_Button *gateButton;

    Parameters* params;

    // Static callback functions for FLTK
    static void cb_gain(Fl_Widget* widget, void* data);
    static void cb_cutoff(Fl_Widget* widget, void* data);
    static void cb_q(Fl_Widget* widget, void* data);
    static void cb_gate(Fl_Widget* widget, void* data);
    static void cb_button(Fl_Widget* widget, void* data);
};

#endif // SYNTHGUI_H
