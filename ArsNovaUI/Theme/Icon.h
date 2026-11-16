#pragma once

#include <juce_graphics/juce_graphics.h>

#include <vector>

/*
    An icon as data: the glyph layers it is drawn from, and how each layer is painted. The
    layers are drawn in order, over one another, in the colour CustomLookAndFeel::drawIcon picks
    for the button's state. selectedLayers, when it is not empty, replaces layers while the
    button is selected, which is how a play icon becomes a pause icon and how a shape gains its
    tint. colour says whether the layers follow the button's state or stay on the accent colour.

    IconLibrary holds the icons the library draws, one row each. A program that needs another
    builds an Icon from its own paths and gives it to an IconButton, a ButtonPane or an
    ItemSelector. An icon that reacts to more than the selected state still needs a painter on
    its IconButton.
*/

namespace ArsNovaUI
{

enum class GlyphPaint { Stroked, Dashed, Filled, Tinted };

enum class IconColour { Pressable, Accent };

struct IconLayer
{
    juce::Path path;

    GlyphPaint paint = GlyphPaint::Stroked;
};

struct Icon
{
    std::vector<IconLayer> layers;
    std::vector<IconLayer> selectedLayers;

    IconColour colour = IconColour::Pressable;
};

}
