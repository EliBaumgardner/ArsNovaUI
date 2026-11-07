#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>

/*
    The node features of the canvas: creating a root, moving nodes, and creating a child by
    dragging. createRootNode makes a root where Shift+click landed. respondToDrag handles a drag
    that starts on a node: a plain drag moves the node and whatever the document moves with it
    (handleNodeDrag), snapped to the grid, while a Shift drag creates a new child under it through
    the document's addChildNode, and then moves that child with the cursor. A child dragged onto
    another tree's root snaps to it, and finishDrag replaces the new child with a connection to
    that root. The document's growFrom names the node a child grows from; where it names none, a
    Shift drag moves the node instead. onChildCreated reports each new child to the program that
    uses the canvas.

    dragArrowHead and finishArrowHeadDrag do the same moving for the node an arrow points at,
    when the arrow's head is grabbed. CanvasController owns it and routes to it. Every edit goes
    through CanvasDocument in an undo transaction.
*/

namespace ArsNovaUI
{

class CanvasNode;
class EditCanvas;
class CanvasDocument;

class NodeGestures
{
public:

    NodeGestures(EditCanvas& canvas, CanvasDocument& document);

    void createRootNode(juce::Point<float> clickPoint);

    void respondToDrag(CanvasNode& node, juce::Point<int> cursor, const juce::ModifierKeys& mods);
    void finishDrag ();

    void dragArrowHead(juce::Point<int> cursor);
    void finishArrowHeadDrag ();

    std::function<void(int, const juce::ValueTree&)> onChildCreated;

    juce::Component::SafePointer<CanvasNode> arrowHeadNode;

private:

    void handleNodeDrag     (int nodeId, juce::Point<int> cursor);
    void handleNodeDragStart(int nodeId, juce::Point<int> cursor, const juce::ModifierKeys& mods);

    void checkRootNodeSnap    (juce::Point<int> canvasPoint);
    void setDraggedNodeVisible(bool shouldBeVisible);
    void connectDraggedNodeToRoot ();

    juce::Point<int> snapToGrid(juce::Point<int> position, const juce::ValueTree& nodeTree) const;

    static constexpr float rootSnapThreshold = 20.0f;

    EditCanvas&        canvas;
    CanvasDocument&    document;
    juce::UndoManager& undoManager;

    juce::Component::SafePointer<CanvasNode> snapTargetRoot;

    juce::ValueTree draggedNodeTree;

    int snapSourceNodeId = -1;

    bool isDragStart = true;
};

}
