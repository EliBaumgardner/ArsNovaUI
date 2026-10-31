#pragma once

#include <juce_graphics/juce_graphics.h>

/*
    The picking queries of the canvas: given a point in canvas coordinates, each function returns
    the closest visible arrow or node of one kind within a radius, or nullptr. arrowNear measures
    to the line from an arrow's start node to its tip, arrowHeadNear to the head of an arrow
    between two nodes, danglingHeadNear to the tip of a dangling arrow, and arrowLabelNear to the
    centre of a duration label. nodeNear and rootNear measure to node centres, rootNear only for
    nodes whose isRoot is set and nodeContaining only for nodes the point is inside whose
    takesArrows is set, which are the ones an arrow may connect to. A node's id in excludeId is
    skipped.

    It reads the nodes and arrows through EditCanvas::nodeManager and arrowManager and changes
    nothing. EditCanvas owns it; the gesture classes and the canvas controllers call it.
*/

namespace ArsNovaUI
{

class EditCanvas;
class CanvasArrow;
class CanvasNode;

class CanvasHitTester
{
public:

    explicit CanvasHitTester(EditCanvas& canvas) : canvas(canvas)
    {
    }

    CanvasArrow* arrowNear       (juce::Point<float> point, float radius) const;
    CanvasArrow* arrowHeadNear   (juce::Point<float> point, float radius) const;
    CanvasArrow* danglingHeadNear(juce::Point<float> point, float radius) const;
    CanvasArrow* arrowLabelNear  (juce::Point<float> point, float radius) const;

    CanvasNode* nodeNear      (juce::Point<float> point, float radius, int excludeId) const;
    CanvasNode* rootNear      (juce::Point<float> point, float radius, int excludeId) const;
    CanvasNode* nodeContaining(juce::Point<float> point, int excludeId) const;

    static float distanceToSegment(juce::Point<float> point, juce::Point<float> segmentStart, juce::Point<float> segmentEnd);

private:

    EditCanvas& canvas;
};

}
