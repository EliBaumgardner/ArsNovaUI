#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include "../Theme/Theme.h"

namespace ArsNovaUI
{

class Bar : public juce::Component
{
public:

    struct Item;
    struct Layout;

    enum class Orientation;

    using Group = std::vector<Item>;

    Bar(Orientation orientation, float contentInsetRatio);
    Bar(juce::Component& backdrop, Orientation orientation, float contentInsetRatio);

    void paint(juce::Graphics& graphics) final;

    enum class Orientation { Horizontal, Vertical };
    enum class Surface     { Solid, Frosted, Flat };

    struct Item
    {
        juce::Component* component;
        int              width;
        int              spaceAfter = 0;
    };

    struct Layout
    {
        std::vector<Group> start;
        std::vector<Group> end;
    };

    static constexpr int contentSpacing      = 12;
    static constexpr int unrelatedItemSpace = 6;

    Surface surface = Surface::Solid;

protected:

    virtual void paintOverBar(juce::Graphics& graphics) {}

    void placeLayout(const Layout& layout);

    int layoutWidth(const Layout& layout) const;

    juce::Rectangle<int> getContentBounds() const;

private:

    void placeGroups(const std::vector<Group>& groups, int startX, juce::Rectangle<int> content);

    int groupWidth(const Group& group) const;
    int sideWidth (const std::vector<Group>& groups) const;

    static constexpr int endPadding      = 6;
    static constexpr int groupPadding    = 6;
    static constexpr int itemGap         = 2;
    static constexpr int dividerWidth    = 1;
    static constexpr int groupSeparation = groupPadding * 2 + dividerWidth;

    static constexpr float dividerHeightRatio = 0.6f;

    Orientation      orientation;
    float            contentInsetRatio;
    juce::Component* backdrop = nullptr;

    std::vector<int> dividerPositions;
};

}
