#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>
#include "IconButton.h"

namespace ArsNovaUI
{

class ButtonPane : public juce::Component
{
public:

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    IconButton& addButton(const Icon& icon, const juce::String& tooltip, std::function<void()> onClick = nullptr);
    IconButton& addButton(const juce::String& tooltip, std::function<void()> onClick = nullptr);

    void setSelectedButton(const IconButton* selected);

    int idealWidth(int height) const;

    struct Grid
    {
        int cellWidth;
        int cellHeight;
        int gap;
        int edgeInset;
    };

    enum class Selection { Momentary, Exclusive, ExclusiveOrNone };
    enum class Look      { Pane, Flat };

    std::function<void(const IconButton*)> onSelectionChanged;

    Selection           selection = Selection::Momentary;
    Look                look      = Look::Pane;
    std::optional<Grid> gridLayout;

    float insetRatio = Theme::contentInsetRatio;

private:

    juce::OwnedArray<IconButton> buttons;

    const IconButton* selectedButton = nullptr;
};

}
