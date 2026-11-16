#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "CanvasTreeListener.h"
#include "NodeManager.h"
#include "ArrowManager.h"
#include "CanvasHitTester.h"
#include "FrameClock.h"

/*
    A component that shows a CanvasDocument's tree of nodes as CanvasNode and CanvasArrow
    children and keeps them in step with it. It holds what every canvas shares: the view and
    model transforms of the pan and zoom (every child is drawn through viewTransform), the snap
    grid (gridOrigin, gridSpacing, whether it is shown, and whether it shows only during a drag or
    always), the selection rectangle it draws over the canvas while selectionBounds is not empty
    (BoxSelectionGestures sets it), and the queue of AsyncUpdates its tree listener adds for each
    change to the document's tree. Its parts are NodeManager and ArrowManager for the components and
    CanvasHitTester for picking.

    handleAsyncUpdate applies the queued updates, rebuildFromNodeMap builds every component from a
    whole tree, and cancelPendingUpdatesFor drops a node's queued updates. snapPointToGrid moves a
    point onto the nearest grid line within a few pixels.

    A subclass decides what is particular to its program through the hooks: makeNode, makeArrow
    and makeDanglingArrow pick the classes a tree becomes, nodeCreated, nodeAdded, nodeRemoving,
    prepareArrow and connectionCreated add the subclass's own setup and clean-up, drawnPosition
    says where a node is drawn when that is not where it is stored, treePropertyChanged turns a
    change to a property the shared code does not know into updates, and updateApplied reacts to
    the updates the shared code leaves alone. A PropertyChanged update carries the identifier of
    the property in property and the id of the node it belongs to, and the shared code does
    nothing with it, so updateApplied is where a program turns it into a redraw. No hook is
    called from a constructor or destructor.
*/

namespace ArsNovaUI
{

class CanvasDocument;
class CanvasNode;
class CanvasArrow;

class EditCanvas : public juce::Component
{
public:

    struct AsyncUpdate;

    explicit EditCanvas(CanvasDocument& document);

    void paint(juce::Graphics& graphics) override;

    void clearCanvas ();
    void childrenChanged () override;
    void cancelPendingUpdatesFor(int nodeId);

    void showGrid();
    void hideGrid();

    virtual void rebuildFromNodeMap(const juce::ValueTree& stateTree);
    virtual void handleAsyncUpdate ();

    virtual void nodeCreated        (CanvasNode& node);
    virtual void nodeAdded          (CanvasNode& node);
    virtual void nodeRemoving       (CanvasNode& node);
    virtual void prepareArrow       (CanvasArrow& arrow);
    virtual void connectionCreated  (CanvasArrow& arrow);
    virtual void updateApplied      (const AsyncUpdate& update);
    virtual void treePropertyChanged(const juce::ValueTree& tree, const juce::Identifier& propertyIdentifier);

    juce::Point<int>         snapPointToGrid(juce::Point<int> point) const;
    virtual juce::Point<int> drawnPosition  (const juce::ValueTree& nodeTree) const;

    virtual std::unique_ptr<CanvasNode> makeNode(const juce::ValueTree& nodeTree) = 0;

    virtual std::unique_ptr<CanvasArrow> makeArrow        (CanvasNode* startNode, CanvasNode* endNode) = 0;
    virtual std::unique_ptr<CanvasArrow> makeDanglingArrow(CanvasNode* startNode, juce::Point<int> tipOffset) = 0;

    enum class AsyncUpdateType
    {
        None,
        NodeAdded,
        NodeRemoved,
        NodeMoved,
        ValueChanged,
        DanglingArrowsChanged,
        ArrowAdded,
        ArrowRemoved,
        ArrowInfoChanged,
        PropertyChanged
    };

    enum class GridDisplay { WhileDragging, Always };

    struct AsyncUpdate
    {
        AsyncUpdateType type = AsyncUpdateType::None;

        int nodeId      = -1;
        int childNodeId = -1;

        juce::Identifier property;
    };

    struct NodePair
    {
        int parentNodeId;
        int childNodeId;
    };

    CanvasDocument& document;

    juce::AffineTransform viewTransform;
    juce::AffineTransform modelTransform;

    juce::Rectangle<int> selectionBounds;

    static constexpr float selectionFillAlpha = 0.15f;

    bool gridVisible   = false;
    bool gridOriginSet = false;

    GridDisplay gridDisplay = GridDisplay::WhileDragging;

    juce::Point<float> gridOrigin { 0.0f, 0.0f };

    float gridSpacing = 50.0f;

    std::vector<AsyncUpdate> asyncUpdates;

    CanvasTreeListener treeListener { *this };

    FrameClock frameClock { *this };

    NodeManager  nodeManager;
    ArrowManager arrowManager;

    CanvasHitTester hitTester { *this };
};

}
