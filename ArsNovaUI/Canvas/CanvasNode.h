#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <unordered_map>

#include "../Editors/ValueEditor.h"

/*
    The component that stands for one node of a CanvasDocument's tree on an EditCanvas. It holds
    the node's tree (nodeValueTree) and id, the CanvasArrows that start at it (nodeArrows, by the
    id of the node each ends at, or a negative index for a dangling arrow), one editor for the
    value it shows (nodeValueEditor), and how it looks right now: hovered, selected, inside a
    collapsed group, and the angle the arrow into it arrives from. It draws nothing itself, so the
    program that uses the canvas derives from it and paints the node.

    The canvas classes ask a node about itself through plain flags and a few virtuals. isRoot and
    takesArrows say whether an arrow may be snapped onto it, takesArrowHeads and arrowTipReach
    say where an arrow that ends at it stops, and connectsByDrag says it starts arrows by being
    dragged and not by the arrow tool. getNodeCentre, getVisualRadius and getBodyExtent say where
    its body is, which CanvasArrow uses to stop a shaft at the edge, and getBoundsAt where the
    component sits when its tree is at a position. setHoverVisual, respondToClick and
    incomingAngleChanged are the points a subclass adds its own reaction to.

    NodeManager creates every node through EditCanvas::makeNode and fills in its id and tree.
    CanvasController routes the mouse to it.
*/

namespace ArsNovaUI
{

class CanvasArrow;

class CanvasNode : public juce::Component
{
public:

    explicit CanvasNode(juce::UndoManager& undoManager);

    void setSelectVisual(bool isSelected);
    void setSelectVisual ();
    void refreshValueDisplay ();

    virtual void setHoverVisual(bool isHovered);
    virtual void respondToClick(juce::Point<int> localPoint);
    virtual void incomingAngleChanged ();

    virtual juce::Point<int> getNodeCentre() const;

    virtual juce::Rectangle<int> getBoundsAt(juce::Point<int> position) const;

    virtual float getVisualRadius () const;
    virtual float getBodyExtent(juce::Point<float> approachDirection) const;

    std::function<void(CanvasNode*, bool)> onSelected;

    std::unordered_multimap<int, CanvasArrow*> nodeArrows;

    juce::ValueTree nodeValueTree;
    ValueEditor     nodeValueEditor;

    int nodeId = -1;

    float incomingAngle = 0.0f;
    float arrowTipReach = 0.0f;

    bool isHovered           = false;
    bool isSelected          = false;
    bool isRoot              = false;
    bool takesArrows         = false;
    bool takesArrowHeads     = true;
    bool connectsByDrag      = false;
    bool isEncapsulated      = false;
    bool isEncapsulationExit = false;
    bool isAlternativeNode   = false;

protected:

    juce::UndoManager& undoManager;
};

}
