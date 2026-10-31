#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

/*
    The owner of an EditCanvas's CanvasNode components: one node for each node in the canvas's
    document, kept in a map by node id and added to the canvas as a child. Which class a node
    becomes is the canvas's choice (EditCanvas::makeNode), and EditCanvas::nodeCreated finishes
    its setup.

    instantiateFromTree builds the component for a tree. add does that for a node just added to
    the document, connects its arrows to the nodes it already links to and calls
    EditCanvas::nodeAdded; remove calls EditCanvas::nodeRemoving and undoes it. setPosition moves
    a component to the bounds its getBoundsAt gives for EditCanvas::drawnPosition. selectAll,
    clearAll and deselectAllExcept change which nodes are selected, and every selection change is
    reported to nodeSelectedListeners.

    EditCanvas::handleAsyncUpdate calls it for NodeAdded, NodeRemoved and NodeMoved updates.
    ArrowManager is its partner for arrows; each calls into the other.
*/

namespace ArsNovaUI
{

class EditCanvas;
class CanvasNode;

class NodeManager
{
public:

    explicit NodeManager(EditCanvas& canvas);
    ~NodeManager();

    const std::unordered_map<int, std::unique_ptr<CanvasNode>>& all() const
    {
        return nodes;
    }

    CanvasNode* find               (int nodeId) const;
    CanvasNode* instantiateFromTree(const juce::ValueTree& nodeValueTree);

    void add   (int nodeId);
    void remove(int nodeId);
    void clear ();

    void setPosition(int nodeId);

    void selectAll () const;
    void clearAll () const;
    void deselectAllExcept(const CanvasNode& keptNode) const;

    std::vector<std::function<void(CanvasNode*, bool)>> nodeSelectedListeners;

private:

    void connectIncomingArrows(int nodeId, CanvasNode* node);
    void connectOutgoingArrows(const juce::ValueTree& nodeValueTree, CanvasNode* node);

    EditCanvas& canvas;

    std::unordered_map<int, std::unique_ptr<CanvasNode>> nodes;
};

}
