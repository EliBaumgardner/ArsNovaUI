#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Theme/Theme.h"

namespace ArsNovaUI
{

class CaptionLabel : public juce::Label
{
public:

    CaptionLabel();

    void parentHierarchyChanged() override;
    void lookAndFeelChanged() override;

    enum class Tone { Strong, Soft, Muted };

    Theme::FontStyle fontStyle = Theme::FontStyle::Regular;

    Tone tone = Tone::Muted;

    float fontHeight = Theme::textHeight;
};

}
