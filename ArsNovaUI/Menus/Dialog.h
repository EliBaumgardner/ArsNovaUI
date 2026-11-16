#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Buttons/IconButton.h"

#include <functional>
#include <memory>
#include <vector>

namespace ArsNovaUI
{

class Dialog
{
public:

    void addButton(juce::String label, std::function<void()> action);
    void show     (juce::Component& target) const;

    juce::String title;
    juce::String message;

private:

    struct Button
    {
        juce::String          label;
        std::function<void()> action;
    };

    class Panel : public juce::Component
    {
    public:

        explicit Panel(const Dialog& dialog);

        void paint  (juce::Graphics& graphics) override;
        void resized() override;

        void parentSizeChanged() override;

        bool keyPressed(const juce::KeyPress& key) override;

    private:

        juce::String title;
        juce::String message;

        juce::TextLayout titleLayout;
        juce::TextLayout messageLayout;

        juce::Rectangle<int> box;
        juce::Rectangle<int> titleArea;
        juce::Rectangle<int> messageArea;

        std::vector<std::unique_ptr<IconButton>> buttons;

        static constexpr int firstButtonResult = 1;
    };

    std::vector<Button> buttons;
};

}
