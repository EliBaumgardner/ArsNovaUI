#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <vector>

namespace ArsNovaUI
{

class TabBar : public juce::Component,
               public juce::TooltipClient
{
public:

    void paint(juce::Graphics& graphics) override;
    void resized () override;

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

    juce::String getTooltip() override;

    struct Tab
    {
        int id = -1;

        juce::String name;

        bool marked = false;
    };

    static constexpr int tabPadding     = 10;
    static constexpr int closeSize      = 16;
    static constexpr int markWidth      = 10;
    static constexpr int accentHeight   = 2;
    static constexpr int minTabWidth    = 64;
    static constexpr int maxTabWidth    = 170;
    static constexpr int minShrunkWidth = 44;

    static constexpr float tabFontHeight = 11.0f;

    std::function<void(int)> onTabClicked;
    std::function<void(int)> onTabClosed;

    std::vector<Tab> tabs;

    int activeTabId = -1;

private:

    int tabIndexAt(juce::Point<int> position) const;

    std::vector<juce::Rectangle<int>> tabAreas;
    std::vector<juce::Rectangle<int>> closeAreas;

    int hoveredTab = -1;

    bool closeHovered = false;
};

}
