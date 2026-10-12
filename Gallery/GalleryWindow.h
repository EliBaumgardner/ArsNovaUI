#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ArsNovaUI/Theme/CustomLookAndFeel.h>
#include "GalleryPage.h"

class GalleryWindow : public juce::DocumentWindow
{
public:

    explicit GalleryWindow(const juce::String& title);
    ~GalleryWindow() override;

    void closeButtonPressed() override;

private:

    CustomLookAndFeel lookAndFeel;
    GalleryPage       page;
};
