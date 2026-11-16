#include "TabBar.h"
#include "../Buttons/IconButton.h"
#include "../Theme/CustomLookAndFeel.h"
#include "../Theme/IconLibrary.h"

namespace ArsNovaUI
{

void TabBar::paint(juce::Graphics& graphics)
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const juce::Font font = lookAndFeel.font(Theme::FontStyle::Regular, tabFontHeight);

    const int separatorWidth = juce::roundToInt(Theme::borderThickness);

    for (int index = 0; index < static_cast<int>(tabs.size()) && index < static_cast<int>(tabAreas.size()); ++index) {
        const Tab& tab = tabs[static_cast<size_t>(index)];

        const juce::Rectangle<int> area = tabAreas[static_cast<size_t>(index)];

        const bool isActive  = tab.id == activeTabId;
        const bool isHovered = index == hoveredTab;

        juce::Colour textColour = lookAndFeel.mutedTextColour;

        auto content = area.reduced(tabPadding, 0);

        ButtonState closeState;

        if (area.isEmpty()) {
            continue;
        }

        if (isActive) {
            textColour = lookAndFeel.textColour;

            graphics.setColour(lookAndFeel.surfaceColour);
            graphics.fillRect(area);

            graphics.setColour(lookAndFeel.accentColour);
            graphics.fillRect(area.withHeight(accentHeight));
        }

        graphics.setColour(lookAndFeel.borderColour);
        graphics.fillRect(area.getRight() - separatorWidth, 0, separatorWidth, getHeight() - separatorWidth);

        content.removeFromRight(closeSize);

        if (isActive || isHovered) {
            closeState.isHovered = isHovered && closeHovered;

            const juce::Rectangle<float> closeBounds = closeAreas[static_cast<size_t>(index)].toFloat();

            lookAndFeel.drawIcon(graphics, closeBounds, IconLibrary::get().close, closeState);
        }

        if (tab.marked) {
            const juce::Rectangle<float> markBounds = content.removeFromLeft(markWidth).toFloat();

            lookAndFeel.drawIcon(graphics, markBounds, IconLibrary::get().marker, closeState);
        }

        graphics.setColour(textColour);
        graphics.setFont(font);
        graphics.drawText(tab.name, content, juce::Justification::centredLeft, true);
    }
}

void TabBar::resized()
{
    const juce::Font font = CustomLookAndFeel::get(*this).font(Theme::FontStyle::Regular, tabFontHeight);

    const int available   = getWidth();
    const int count       = static_cast<int>(tabs.size());
    int       total       = 0;
    int       firstShown  = 0;
    int       activeIndex = 0;
    int       x           = 0;

    std::vector<int> widths;

    for (int index = 0; index < count; ++index) {
        const Tab& tab = tabs[static_cast<size_t>(index)];

        const int textWidth = juce::GlyphArrangement::getStringWidthInt(font, tab.name);
        const int width     = juce::jlimit(minTabWidth, maxTabWidth, textWidth + tabPadding * 2 + closeSize + markWidth);

        if (tab.id == activeTabId) {
            activeIndex = index;
        }

        widths.push_back(width);

        total += width;
    }

    if (count > 0 && total > available) {
        const int shared = juce::jmax(minShrunkWidth, available / count);
        const int fitted = juce::jmax(1, available / shared);

        for (int& width : widths) {
            width = juce::jmin(width, shared);
        }

        firstShown = juce::jmax(0, activeIndex - fitted + 1);
    }

    tabAreas.assign(static_cast<size_t>(count), {});
    closeAreas.assign(static_cast<size_t>(count), {});

    for (int index = firstShown; index < count; ++index) {
        const int width = widths[static_cast<size_t>(index)];

        if (index > firstShown && x + width > available) {
            break;
        }

        tabAreas[static_cast<size_t>(index)]   = { x, 0, width, getHeight() };
        closeAreas[static_cast<size_t>(index)] = { x + width - tabPadding / 2 - closeSize, (getHeight() - closeSize) / 2, closeSize, closeSize };
        x                                     += width;
    }

    hoveredTab   = -1;
    closeHovered = false;

    repaint();
}

void TabBar::mouseMove(const juce::MouseEvent& event)
{
    const int index = tabIndexAt(event.getPosition());

    const bool onClose = index >= 0 && closeAreas[static_cast<size_t>(index)].contains(event.getPosition());

    if (index != hoveredTab || onClose != closeHovered) {
        hoveredTab   = index;
        closeHovered = onClose;

        repaint();
    }
}

void TabBar::mouseExit(const juce::MouseEvent&)
{
    hoveredTab   = -1;
    closeHovered = false;

    repaint();
}

void TabBar::mouseDown(const juce::MouseEvent& event)
{
    const int index = tabIndexAt(event.getPosition());
    int       tabId = -1;

    if (index < 0) {
        return;
    }

    tabId = tabs[static_cast<size_t>(index)].id;

    if (event.mods.isMiddleButtonDown() || closeAreas[static_cast<size_t>(index)].contains(event.getPosition())) {
        if (onTabClosed != nullptr) {
            onTabClosed(tabId);
        }

        return;
    }

    if (onTabClicked != nullptr) {
        onTabClicked(tabId);
    }
}

juce::String TabBar::getTooltip()
{
    if (hoveredTab < 0 || hoveredTab >= static_cast<int>(tabs.size())) {
        return {};
    }

    return tabs[static_cast<size_t>(hoveredTab)].name;
}

int TabBar::tabIndexAt(juce::Point<int> position) const
{
    for (int index = 0; index < static_cast<int>(tabAreas.size()) && index < static_cast<int>(tabs.size()); ++index) {
        if (tabAreas[static_cast<size_t>(index)].contains(position)) {
            return index;
        }
    }

    return -1;
}

}
