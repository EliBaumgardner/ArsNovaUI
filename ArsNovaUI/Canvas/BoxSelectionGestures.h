#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ArsNovaUI
{

class EditCanvas;

class BoxSelectionGestures
{
public:

    explicit BoxSelectionGestures(EditCanvas& canvas);

    void begin(juce::Point<int> clickPoint);
    void update(const juce::MouseEvent& e);
    void finish ();

private:

    EditCanvas& canvas;

    juce::Point<int> anchor;
};

}
