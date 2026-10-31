#include "EditCanvas.h"
#include "CanvasNode.h"
#include "CanvasArrow.h"
#include "CanvasDocument.h"
#include "CanvasIdentifiers.h"
#include "../Theme/CustomLookAndFeel.h"

#include <cmath>

namespace ArsNovaUI
{

EditCanvas::EditCanvas(CanvasDocument& document)
    : document(document),
      nodeManager(*this),
      arrowManager(*this)
{
}

void EditCanvas::paint(juce::Graphics& graphics)
{
    const CanvasGrid grid { getLocalBounds().toFloat(), modelTransform, viewTransform, gridOrigin, gridSpacing, gridVisible, gridOriginSet };

    CustomLookAndFeel::get(*this).drawCanvas(graphics, grid);
}

void EditCanvas::clearCanvas()
{
    arrowManager.clear();
    nodeManager .clear();

    gridOriginSet = false;
    gridVisible   = false;
}

void EditCanvas::childrenChanged()
{
    for (juce::Component* child : getChildren()) {
        child->setTransform(viewTransform);
    }
}

void EditCanvas::cancelPendingUpdatesFor(int nodeId)
{
    std::erase_if(asyncUpdates, [nodeId](const AsyncUpdate& update) {
        return update.nodeId == nodeId
            && update.type != AsyncUpdateType::NodeRemoved;
    });
}

void EditCanvas::showGrid()
{
    if (gridOriginSet) {
        gridVisible = true;

        repaint();
    }
}

void EditCanvas::hideGrid()
{
    if (gridVisible && gridDisplay == GridDisplay::WhileDragging) {
        gridVisible = false;

        repaint();
    }
}

void EditCanvas::rebuildFromNodeMap(const juce::ValueTree& stateTree)
{
    std::vector<NodePair> nodePairs;

    asyncUpdates.clear();

    clearCanvas();

    for (int nodeIndex = 0; nodeIndex < stateTree.getNumChildren(); ++nodeIndex) {
        const juce::ValueTree nodeValueTree = stateTree.getChild(nodeIndex);
        const juce::ValueTree childIds      = nodeValueTree.getChildWithName(CanvasIdentifiers::NodeChildrenIds);

        const int nodeId = nodeValueTree.getProperty(CanvasIdentifiers::Id);

        for (int childIndex = 0; childIndex < childIds.getNumChildren(); ++childIndex) {
            nodePairs.push_back({ nodeId, childIds.getChild(childIndex).getProperty(CanvasIdentifiers::Id) });
        }

        nodeManager.instantiateFromTree(nodeValueTree);
    }

    for (auto [parentNodeId, childNodeId] : nodePairs) {
        CanvasNode* const parentNode = nodeManager.find(parentNodeId);
        CanvasNode* const childNode  = nodeManager.find(childNodeId);

        if (parentNode == nullptr || childNode == nullptr) {
            continue;
        }

        arrowManager.connectParentToChild(parentNode, childNode);
    }

    for (auto& [nodeId, node] : nodeManager.all()) {
        arrowManager.rebuildDanglingForNode(nodeId);
    }
}

void EditCanvas::handleAsyncUpdate()
{
    std::vector<AsyncUpdate> pendingUpdates;

    pendingUpdates.swap(asyncUpdates);

    for (const AsyncUpdate& asyncUpdate : pendingUpdates) {
        const int nodeId = asyncUpdate.nodeId;

        switch (asyncUpdate.type) {
            case AsyncUpdateType::NodeAdded:
                nodeManager.add(nodeId);
                break;

            case AsyncUpdateType::NodeRemoved:
                nodeManager.remove(nodeId);
                break;

            case AsyncUpdateType::NodeMoved:
                nodeManager.setPosition(nodeId);
                break;

            case AsyncUpdateType::ValueChanged:

            case AsyncUpdateType::ArrowDurationChanged:
                if (CanvasNode* const changedNode = nodeManager.find(nodeId)) {
                    arrowManager.refreshFor(changedNode);
                }
                break;

            case AsyncUpdateType::DanglingArrowsChanged:
                arrowManager.rebuildDanglingForNode(nodeId);
                break;

            case AsyncUpdateType::ArrowAdded:
                arrowManager.handleArrowAdded(nodeId, asyncUpdate.childNodeId);
                break;

            case AsyncUpdateType::ArrowRemoved:
                arrowManager.remove(arrowManager.find(nodeId, asyncUpdate.childNodeId));
                break;

            case AsyncUpdateType::ArrowInfoChanged:
                arrowManager.handleArrowInfoChanged(nodeId, asyncUpdate.childNodeId);
                break;

            case AsyncUpdateType::NodeColourChanged:

            case AsyncUpdateType::NodeMarkChanged:

            case AsyncUpdateType::None:
                break;
        }

        updateApplied(asyncUpdate);
    }
}

void EditCanvas::nodeCreated(CanvasNode&)
{
}

void EditCanvas::nodeAdded(CanvasNode&)
{
}

void EditCanvas::nodeRemoving(CanvasNode&)
{
}

void EditCanvas::prepareArrow(CanvasArrow&)
{
}

void EditCanvas::connectionCreated(CanvasArrow&)
{
}

void EditCanvas::updateApplied(const AsyncUpdate&)
{
}

void EditCanvas::treePropertyChanged(const juce::ValueTree&, const juce::Identifier&)
{
}

juce::Point<int> EditCanvas::snapPointToGrid(juce::Point<int> point) const
{
    const float snapThreshold = 5.0f;
    const float snappedX      = gridOrigin.x + std::round((static_cast<float>(point.x) - gridOrigin.x) / gridSpacing) * gridSpacing;
    const float snappedY      = gridOrigin.y + std::round((static_cast<float>(point.y) - gridOrigin.y) / gridSpacing) * gridSpacing;

    juce::Point<int> result = point;

    if (!gridOriginSet) {
        return point;
    }

    if (std::abs(static_cast<float>(point.x) - snappedX) < snapThreshold) {
        result.x = static_cast<int>(snappedX);
    }

    if (std::abs(static_cast<float>(point.y) - snappedY) < snapThreshold) {
        result.y = static_cast<int>(snappedY);
    }

    return result;
}

juce::Point<int> EditCanvas::drawnPosition(const juce::ValueTree& nodeTree) const
{
    return { nodeTree.getProperty(CanvasIdentifiers::XPosition), nodeTree.getProperty(CanvasIdentifiers::YPosition) };
}

}
