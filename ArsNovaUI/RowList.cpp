#include "RowList.h"
#include "Theme/CustomLookAndFeel.h"
#include "Theme/IconLibrary.h"

namespace ArsNovaUI
{

void RowList::paint(juce::Graphics& graphics)
{
    const Theme& theme = CustomLookAndFeel::get(*this);

    if (rows.empty()) {
        graphics.setColour(theme.mutedTextColour);
        graphics.setFont(theme.font(Theme::FontStyle::Regular, rowFontHeight));
        graphics.drawFittedText(emptyText, getLocalBounds().reduced(rowInset * 2, rowInset), juce::Justification::centredTop, emptyRows);

        return;
    }

    for (int index = 0; index < static_cast<int>(rows.size()); ++index) {
        paintRowBackdrop(graphics, index);

        paintRowContent(graphics, index);
    }
}

void RowList::mouseMove(const juce::MouseEvent& event)
{
    const int index = rowIndexAt(event.y);

    if (index != hoveredRow) {
        hoveredRow = index;

        repaint();
    }
}

void RowList::mouseExit(const juce::MouseEvent&)
{
    hoveredRow = -1;

    repaint();
}

void RowList::mouseDown(const juce::MouseEvent& event)
{
    const int index = rowIndexAt(event.y);

    if (index >= 0 && onRowClicked != nullptr) {
        onRowClicked(index);
    }
}

void RowList::layoutRows()
{
    const int rowCount = static_cast<int>(rows.size());

    if (hoveredRow >= rowCount) {
        hoveredRow = -1;
    }

    setSize(getWidth(), juce::jmax(minimumHeight, (rowCount + 1) * rowHeight));

    repaint();
}

int RowList::rowIndexAt(int positionY) const
{
    const int index = positionY / rowHeight;

    if (positionY < 0 || index >= static_cast<int>(rows.size())) {
        return -1;
    }

    return index;
}

void RowList::paintRowBackdrop(juce::Graphics& graphics, int index) const
{
    const Theme& theme = CustomLookAndFeel::get(*this);
    const Row&   row   = rows[static_cast<size_t>(index)];

    const juce::Rectangle<int>   bounds    { 0, index * rowHeight, getWidth(), rowHeight };
    const juce::Rectangle<float> highlight = bounds.reduced(rowInset / 2, highlightGap).toFloat();

    juce::Colour fillColour = juce::Colours::transparentBlack;

    if (index == hoveredRow) {
        fillColour = theme.hoverColour;
    }

    switch (row.fill) {
        case RowFill::Selected: fillColour = theme.selectedColour;   break;
        case RowFill::Accent:   fillColour = theme.accentSoftColour; break;
        case RowFill::None:                                          break;
    }

    graphics.setColour(fillColour);
    graphics.fillRoundedRectangle(highlight, rowCorner);

    if (row.outlined) {
        graphics.setColour(theme.accentColour);
        graphics.drawRoundedRectangle(highlight, rowCorner, Theme::borderThickness);
    }

    graphics.setColour(theme.borderColour);

    for (int level = 0; level < row.depth; ++level) {
        graphics.fillRect(rowInset + level * indentWidth + leadingWidth / 2, bounds.getY(), juce::roundToInt(Theme::borderThickness), rowHeight);
    }
}

void RowList::paintRowContent(juce::Graphics& graphics, int index) const
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);
    const Row&         row         = rows[static_cast<size_t>(index)];

    const juce::Rectangle<int> bounds { 0, index * rowHeight, getWidth(), rowHeight };

    const ButtonState glyphState;

    juce::Font font = lookAndFeel.font(Theme::FontStyle::Regular, rowFontHeight);

    juce::Colour textColour  = lookAndFeel.softTextColour;
    juce::Colour badgeColour = lookAndFeel.accentColour;

    auto content = bounds.withTrimmedLeft(rowInset + row.depth * indentWidth).withTrimmedRight(rowInset);
    auto leading = content.removeFromLeft(leadingWidth);

    switch (row.style) {
        case RowStyle::Heading:    font = lookAndFeel.font(Theme::FontStyle::SemiBold, rowFontHeight); textColour = lookAndFeel.textColour; break;
        case RowStyle::Emphasised: textColour = lookAndFeel.textColour;                                                                   break;
        case RowStyle::Item:                                                                                                              break;
    }

    switch (row.badgeTone) {
        case BadgeTone::Success: badgeColour = lookAndFeel.successColour; break;
        case BadgeTone::Strong:  badgeColour = lookAndFeel.textColour;    break;
        case BadgeTone::Accent:                                           break;
    }

    if (row.icon != nullptr) {
        lookAndFeel.drawIcon(graphics, leading.toFloat(), *row.icon, glyphState);
    }

    if (row.badge.isNotEmpty()) {
        const juce::Rectangle<float> badgeBounds = leading.withSizeKeepingCentre(badgeSize, badgeSize).toFloat();

        graphics.setColour(badgeColour.withAlpha(badgeFillAlpha));
        graphics.fillRoundedRectangle(badgeBounds, rowCorner);

        graphics.setColour(badgeColour);
        graphics.setFont(lookAndFeel.font(Theme::FontStyle::Mono, badgeFontHeight));
        graphics.drawText(row.badge, badgeBounds, juce::Justification::centred, false);
    }

    if (row.marked) {
        const juce::Rectangle<float> markBounds = content.removeFromRight(markWidth).toFloat();

        lookAndFeel.drawIcon(graphics, markBounds, IconLibrary::get().marker, glyphState);
    }

    if (row.detail.isNotEmpty()) {
        const int detailWidth = juce::GlyphArrangement::getStringWidthInt(font, row.detail) + rowInset;

        graphics.setColour(lookAndFeel.mutedTextColour);
        graphics.setFont(lookAndFeel.font(Theme::FontStyle::Regular, rowFontHeight));
        graphics.drawText(row.detail, content.removeFromRight(detailWidth), juce::Justification::centredRight, false);
    }

    graphics.setColour(textColour);
    graphics.setFont(font);
    graphics.drawText(row.text, content, juce::Justification::centredLeft, true);
}

}
