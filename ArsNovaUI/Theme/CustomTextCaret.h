#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace ArsNovaUI
{

class CustomTextCaret : public juce::CaretComponent
{
public:

    explicit CustomTextCaret(juce::Component* keyFocusOwner);

    void paint(juce::Graphics& graphics) override;

    float caretWidth { 2.0f };
};

}
