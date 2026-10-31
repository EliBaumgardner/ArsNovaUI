#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*
    The edits a node canvas can make to the tree of nodes it shows, declared once so the shared
    canvas code can make them without knowing which document it edits. nodes is that tree: one
    child per node, holding its Id, XPosition and YPosition, a NodeChildrenIds list with an entry
    for each node it points at, and a DanglingArrows list of the arrows whose tip is loose (see
    CanvasIdentifiers). Every edit is recorded in undoManager, inside the transaction the calling
    gesture has begun.

    findNode returns a node's tree by id, and growFrom the id of the node a new child or dangling
    arrow starts from when the user grows one out of a node, or -1 when nothing may grow there.
    The add, move and remove functions change nodes and dangling arrows; moveNode moves a node
    and whatever the document moves along with it. canConnect says whether an arrow may join two
    nodes, connect adds one and says through ConnectionGesture which gesture made it, and
    disconnect takes one away.

    The program that uses the canvas implements it for its own document. NodeGestures,
    ArrowGestures and the canvas controllers call it on the message thread.
*/

namespace ArsNovaUI
{

class CanvasNode;

class CanvasDocument
{
public:

    enum class ConnectionGesture;

    CanvasDocument(juce::ValueTree nodes, juce::UndoManager& undoManager) : nodes(nodes), undoManager(undoManager)
    {
    }

    virtual ~CanvasDocument() = default;

    virtual juce::ValueTree findNode    (int nodeId) const = 0;
    virtual juce::ValueTree addChildNode(int parentNodeId, juce::Point<int> position, const juce::ModifierKeys& mods) = 0;

    virtual int growFrom(int nodeId) const = 0;

    virtual void addRootNode(juce::Point<int> position) = 0;
    virtual void moveNode   (juce::ValueTree nodeTree, juce::Point<int> position) = 0;
    virtual void removeNode (int nodeId) = 0;

    virtual void addDanglingArrow   (juce::ValueTree nodeTree, juce::Point<int> tipOffset) = 0;
    virtual void setDanglingArrowTip(juce::ValueTree arrowTree, juce::Point<int> tipOffset) = 0;
    virtual void removeDanglingArrow(juce::ValueTree arrowTree) = 0;

    virtual void connect   (int parentNodeId, int childNodeId, ConnectionGesture gesture) = 0;
    virtual void disconnect(int startNodeId, int endNodeId) = 0;

    virtual bool canConnect(const CanvasNode& startNode, const CanvasNode& endNode) const = 0;

    enum class ConnectionGesture { TipOntoNode, NodeOntoRoot };

    juce::ValueTree    nodes;
    juce::UndoManager& undoManager;
};

}
