#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace ArsNovaUI
{

class ToggleSwitch : public juce::Component
{
public:

    void paint(juce::Graphics& graphics) override;

    void mouseDown(const juce::MouseEvent& event) override;

    static constexpr float trackInset = 2.0f;

    std::function<void(bool)> onToggle;

    bool isOn = true;
};

}
