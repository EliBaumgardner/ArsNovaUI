#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <memory>

/*
    The owner of the canvas's CanvasArrow components: one arrow for each connection between two
    nodes in the canvas's document and one for each dangling arrow (an arrow with a loose tip that
    is not connected to a node yet), added to the EditCanvas as children. Each arrow is also
    entered in its start node's nodeArrows, by the end node's id (dangling arrows by a negative
    index), so the program that uses the canvas can find it.

    connect and connectParentToChild create the component for a connection through
    EditCanvas::makeArrow, and an alternative child gets the arrow drawn from the child to its
    parent. refreshFor re-lays-out every arrow of a node that moved or changed, and
    rebuildDanglingForNode rebuilds a node's dangling arrows from its tree, set up by
    EditCanvas::prepareArrow. updatePreview shows the arrow being drawn and commitPreview writes it
    into the document as a dangling arrow. The snap-ghost functions show and remove the arrow that
    previews a connection to a root. setSelected selects one arrow.

    EditCanvas::handleAsyncUpdate calls it for arrow updates. NodeManager is its partner for nodes;
    each calls into the other.
*/

namespace ArsNovaUI
{

class EditCanvas;
class CanvasNode;
class CanvasArrow;

class ArrowManager
{
public:

    explicit ArrowManager(EditCanvas& canvas);
    ~ArrowManager();

    const juce::OwnedArray<CanvasArrow>& all() const
    {
        return arrows;
    }

    CanvasArrow* find                (int parentNodeId, int childNodeId) const;
    CanvasArrow* connect             (CanvasNode* startNode, CanvasNode* endNode);
    CanvasArrow* connectParentToChild(CanvasNode* parentNode, CanvasNode* childNode);

    void adopt(std::unique_ptr<CanvasArrow> arrow);

    void refreshFor            (const CanvasNode* movedNode);
    void rebuildDanglingForNode(int nodeId);
    void endNodeDrag ();
    void refreshEncapsulatedArrows ();

    void remove        (CanvasArrow* arrow);
    void removeForNode (const CanvasNode* node);
    void removeMatching(const std::function<bool(CanvasArrow*)>& predicate);
    void clear ();

    void handleArrowAdded      (int parentNodeId, int childNodeId);
    void handleArrowInfoChanged(int parentNodeId, int childNodeId);

    void updatePreview(CanvasNode* node, juce::Point<int> tipOffset);
    void commitPreview ();

    void showSnapGhost           (CanvasNode* from, CanvasNode* to);
    void triggerSnapForNode      (int nodeId);
    void connectWithSnapAnimation(int parentNodeId, int childNodeId);
    void hideSnapGhost ();

    void setSelected(CanvasArrow* arrow);
    void clearSelection ();

    std::unique_ptr<CanvasArrow> preview;

    bool nodesDragging = false;

private:

    void detach(CanvasArrow* arrow);

    juce::ValueTree connectionTreeFor(int startNodeId, int endNodeId) const;

    static int arrowKey(const CanvasArrow& arrow);

    EditCanvas& canvas;

    juce::OwnedArray<CanvasArrow> arrows;
    std::unique_ptr<CanvasArrow>  snapGhostArrow;
};

}
