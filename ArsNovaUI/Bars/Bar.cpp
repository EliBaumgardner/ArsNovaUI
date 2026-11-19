#include "Bar.h"
#include "../Theme/CustomLookAndFeel.h"

namespace ArsNovaUI
{

Bar::Bar(Orientation orientation, float contentInsetRatio)
    : orientation      (orientation),
      contentInsetRatio(contentInsetRatio)
{
}

Bar::Bar(juce::Component& backdrop, Orientation orientation, float contentInsetRatio)
    : surface          (Surface::Frosted),
      orientation      (orientation),
      contentInsetRatio(contentInsetRatio),
      backdrop         (&backdrop)
{
}

void Bar::paint(juce::Graphics& graphics)
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const float dividerHeight = getHeight() * dividerHeightRatio;
    const float dividerTop    = (getHeight() - dividerHeight) * 0.5f;
    const float dividerSpan   = static_cast<float>(dividerWidth);

    switch (surface) {
        case Surface::Frosted:
            lookAndFeel.drawFrostedGlass(graphics, *this, *backdrop, getLocalBounds());
            break;

        case Surface::Flat:
            graphics.fillAll(lookAndFeel.windowColour);
            break;

        case Surface::Solid:
            graphics.setColour(lookAndFeel.barColour);
            graphics.fillRect(getLocalBounds());
            break;
    }

    graphics.setColour(lookAndFeel.borderStrongColour);

    for (const int dividerX : dividerPositions) {
        const float dividerLeft = static_cast<float>(dividerX);

        const juce::Rectangle<float> divider(dividerLeft, dividerTop, dividerSpan, dividerHeight);

        graphics.fillRect(divider);
    }

    paintOverBar(graphics);
}

void Bar::placeLayout(const Layout& layout)
{
    const auto content = getContentBounds();

    const int startX = content.getX() + endPadding;
    const int endX   = content.getRight() - endPadding - sideWidth(layout.end);

    dividerPositions.clear();

    placeGroups(layout.start, startX, content);
    placeGroups(layout.end, endX, content);

    repaint();
}

int Bar::layoutWidth(const Layout& layout) const
{
    const int chromeWidth = getWidth() - getContentBounds().getWidth();

    return chromeWidth + endPadding * 2 + sideWidth(layout.start) + sideWidth(layout.end) + groupSeparation;
}

juce::Rectangle<int> Bar::getContentBounds() const
{
    if (orientation == Orientation::Horizontal) {
        return getLocalBounds().reduced(juce::roundToInt(getHeight() * contentInsetRatio));
    }

    return getLocalBounds().reduced(juce::roundToInt(getWidth() * contentInsetRatio));
}

void Bar::placeGroups(const std::vector<Group>& groups, int startX, juce::Rectangle<int> content)
{
    int cursor = startX;

    for (const Group& group : groups) {
        for (const Item& item : group) {
            item.component->setBounds(cursor, content.getY(), item.width, content.getHeight());

            cursor += item.width + itemGap + item.spaceAfter;
        }

        cursor -= itemGap + group.back().spaceAfter;

        if (&group != &groups.back()) {
            cursor += groupPadding;

            dividerPositions.push_back(cursor);

            cursor += dividerWidth + groupPadding;
        }
    }
}

int Bar::groupWidth(const Group& group) const
{
    int width = 0;

    for (const Item& item : group) {
        width += item.width + itemGap + item.spaceAfter;
    }

    return width - itemGap - group.back().spaceAfter;
}

int Bar::sideWidth(const std::vector<Group>& groups) const
{
    int width = 0;

    for (const Group& group : groups) {
        width += groupWidth(group) + groupSeparation;
    }

    return width - groupSeparation;
}

}
