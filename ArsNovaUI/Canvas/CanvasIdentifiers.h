#pragma once

#include <juce_core/juce_core.h>

/*
    The names of the properties and child trees that make up the tree of nodes an EditCanvas
    shows, which the canvas classes read and a CanvasDocument's tree must therefore use. Each node
    is one child of the document's tree with an Id, an XPosition and a YPosition, a
    NodeChildrenIds child holding one entry (with that node's Id) for each node it points at, and
    a DanglingArrows child holding one DanglingArrow, with ArrowTipX and ArrowTipY, for each arrow
    whose tip is loose.

    Every member is a juce::Identifier, so a name is spelled in one place. The strings are saved
    in every document that uses them, so changing one makes older saves load without that value.
*/

namespace ArsNovaUI
{

class CanvasIdentifiers
{
public:

    static const juce::Identifier Id;
    static const juce::Identifier XPosition;
    static const juce::Identifier YPosition;

    static const juce::Identifier NodeChildrenIds;
    static const juce::Identifier DanglingArrows;
    static const juce::Identifier DanglingArrow;

    static const juce::Identifier ArrowTipX;
    static const juce::Identifier ArrowTipY;
};

}
