#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*
    The arrow features of the canvas: drawing a new arrow, moving a dangling arrow's tip,
    removing an arrow, and the hover highlight. Drawing and moving are one drag in three calls.
    beginMovingTip or beginTipFromNode starts it, dragTip follows the cursor and snaps the tip to a
    nearby node, and finishTip ends it by connecting to the snapped node, fixing a dangling
    arrow's tip, or keeping the new arrow dangling. Arrows are drawn from a node only in
    arrowMode, or from a node whose connectsByDrag is set, whose arrow is kept only if it lands on
    a node.

    updateHover bolds the arrow under the cursor. removeArrow deletes a dangling arrow, or the
    arrow between two nodes through the document's disconnect. A tip snaps only to a node the
    document's canConnect accepts, and a new arrow starts from the node its growFrom names.

    CanvasController owns it, decides when a press belongs here and uses the grab radii for its
    hit tests; the program that uses the canvas sets arrowMode. Every edit goes through
    CanvasDocument in its own undo transaction.
*/

namespace ArsNovaUI
{

class CanvasNode;
class CanvasArrow;
class EditCanvas;
class CanvasDocument;

class ArrowGestures
{
public:

    ArrowGestures(EditCanvas& canvas, CanvasDocument& document, juce::UndoManager& undoManager);

    void beginMovingTip(CanvasArrow& danglingArrow);
    void dragTip       (juce::Point<int> cursor);
    void finishTip ();

    void updateHover (juce::Point<float> cursor);
    void removeArrow (CanvasArrow& arrow);
    void setArrowMode(bool enabled);

    bool beginTipFromNode(CanvasNode& node);

    static constexpr float danglingArrowGrabRadius = 14.0f;
    static constexpr float arrowHoverRadius        = 8.0f;
    static constexpr float arrowHeadGrabRadius     = 16.0f;
    static constexpr float arrowLabelGrabRadius    = 10.0f;

    bool arrowMode = false;

private:

    CanvasNode* findTipSnapTarget(const CanvasNode& startNode, juce::Point<int> tip) const;

    enum class TipGesture { None, MovingDanglingTip, CreatingDanglingArrow, ConnectingNode };

    static constexpr float connectSnapThreshold = 20.0f;

    EditCanvas&        canvas;
    CanvasDocument&    document;
    juce::UndoManager& undoManager;

    TipGesture tipGesture = TipGesture::None;

    juce::Component::SafePointer<CanvasArrow> draggingDanglingArrow;
    juce::Component::SafePointer<CanvasNode>  tipSnapTarget;
    juce::Component::SafePointer<CanvasNode>  newArrowSourceNode;
};

}
