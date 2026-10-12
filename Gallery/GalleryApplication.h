#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "GalleryWindow.h"

class GalleryApplication : public juce::JUCEApplication
{
public:

    const juce::String getApplicationName   () override;
    const juce::String getApplicationVersion() override;

    void initialise(const juce::String& commandLine) override;
    void shutdown  () override;

    static constexpr int   snapshotDelayMs = 800;
    static constexpr float snapshotScale   = 2.0f;

private:

    std::unique_ptr<GalleryWindow> window;
};
