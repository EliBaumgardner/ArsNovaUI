#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <vector>

#include "Buttons/IconButton.h"

namespace ArsNovaUI
{

class RowList : public juce::Component
{
public:

    void paint(juce::Graphics& graphics) override;

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void layoutRows();

    int rowIndexAt(int positionY) const;

    enum class RowStyle  { Item, Emphasised, Heading };
    enum class RowFill   { None, Selected, Accent };
    enum class BadgeTone { Accent, Success, Strong };

    struct Row
    {
        RowStyle style = RowStyle::Item;

        RowFill fill = RowFill::None;

        BadgeTone badgeTone = BadgeTone::Accent;

        const Icon* icon = nullptr;

        juce::String text;
        juce::String detail;
        juce::String badge;

        int depth = 0;

        bool outlined = false;
        bool marked   = false;
    };

    static constexpr int rowHeight    = 18;
    static constexpr int rowInset     = 6;
    static constexpr int highlightGap = 1;
    static constexpr int indentWidth  = 12;
    static constexpr int leadingWidth = 16;
    static constexpr int badgeSize    = 12;
    static constexpr int markWidth    = 12;
    static constexpr int emptyRows    = 3;

    static constexpr float rowFontHeight   = 11.0f;
    static constexpr float badgeFontHeight = 8.0f;
    static constexpr float badgeFillAlpha  = 0.16f;
    static constexpr float rowCorner       = 3.0f;

    std::function<void(int)> onRowClicked;

    std::vector<Row> rows;

    juce::String emptyText;

    int minimumHeight = 0;
    int hoveredRow    = -1;

private:

    void paintRowBackdrop(juce::Graphics& graphics, int index) const;
    void paintRowContent (juce::Graphics& graphics, int index) const;
};

}
