#include "NodeManager.h"

#include "EditCanvas.h"
#include "ArrowManager.h"
#include "CanvasNode.h"
#include "CanvasDocument.h"
#include "CanvasIdentifiers.h"

namespace ArsNovaUI
{

NodeManager::NodeManager(EditCanvas& canvas) : canvas(canvas)
{
}

NodeManager::~NodeManager() = default;

CanvasNode* NodeManager::find(int nodeId) const
{
    const auto entry = nodes.find(nodeId);

    if (entry == nodes.end()) {
        return nullptr;
    }

    return entry->second.get();
}

CanvasNode* NodeManager::instantiateFromTree(const juce::ValueTree& nodeValueTree)
{
    jassert(nodeValueTree.isValid());

    const int nodeId = nodeValueTree.getProperty(CanvasIdentifiers::Id);

    std::unique_ptr<CanvasNode> node = canvas.makeNode(nodeValueTree);

    jassert(node);

    if (node == nullptr) {
        return nullptr;
    }

    CanvasNode* const createdNode = node.get();

    createdNode->nodeId        = nodeId;
    createdNode->nodeValueTree = nodeValueTree;

    createdNode->onSelected = [this](CanvasNode* selectedNode, bool isSelected) {
        for (auto& listener : nodeSelectedListeners) {
            listener(selectedNode, isSelected);
        }
    };

    canvas.nodeCreated(*createdNode);

    createdNode->setMouseCursor(juce::MouseCursor::ParentCursor);

    canvas.addAndMakeVisible(createdNode);

    nodes[nodeId] = std::move(node);

    setPosition(nodeId);

    return createdNode;
}

void NodeManager::add(int nodeId)
{
    const juce::ValueTree nodeChildTree = canvas.document.findNode(nodeId);

    jassert(nodeChildTree.isValid());

    CanvasNode* const childNode = instantiateFromTree(nodeChildTree);

    if (childNode == nullptr) {
        return;
    }

    connectIncomingArrows(nodeId, childNode);
    connectOutgoingArrows(nodeChildTree, childNode);

    canvas.nodeAdded(*childNode);
}

void NodeManager::remove(int nodeId)
{
    CanvasNode* const node = find(nodeId);

    if (node == nullptr) {
        return;
    }

    canvas.nodeRemoving(*node);

    canvas.arrowManager.removeForNode(node);
    canvas.removeChildComponent(node);
    nodes.erase(nodeId);
}

void NodeManager::clear()
{
    for (auto& [nodeId, node] : nodes) {
        canvas.removeChildComponent(node.get());
    }

    nodes.clear();
}

void NodeManager::setPosition(int nodeId)
{
    CanvasNode* const     node     = find(nodeId);
    const juce::ValueTree nodeTree = canvas.document.findNode(nodeId);

    if (node == nullptr || ! nodeTree.isValid()) {
        return;
    }

    node->setBounds(node->getBoundsAt(canvas.drawnPosition(nodeTree)));

    canvas.arrowManager.refreshFor(node);
}

void NodeManager::selectAll() const
{
    for (auto& [nodeId, node] : nodes) {
        if (node->isVisible() && ! node->isSelected) {
            node->setSelectVisual(true);
        }
    }
}

void NodeManager::clearAll() const
{
    for (auto& [nodeId, node] : nodes) {
        if (node->isSelected) {
            node->setSelectVisual(false);
        }
    }
}

void NodeManager::deselectAllExcept(const CanvasNode& keptNode) const
{
    for (auto& [nodeId, node] : nodes) {
        if (node.get() != &keptNode && node->isSelected) {
            node->setSelectVisual(false);
        }
    }
}

void NodeManager::connectIncomingArrows(int nodeId, CanvasNode* node)
{
    const juce::ValueTree nodeMapTree = canvas.document.nodes;

    for (int parentIndex = 0; parentIndex < nodeMapTree.getNumChildren(); ++parentIndex) {
        const juce::ValueTree parentTree        = nodeMapTree.getChild(parentIndex);
        const juce::ValueTree parentChildrenIds = parentTree.getChildWithName(CanvasIdentifiers::NodeChildrenIds);

        if (! parentChildrenIds.getChildWithProperty(CanvasIdentifiers::Id, nodeId).isValid()) {
            continue;
        }

        const int         parentNodeId = parentTree.getProperty(CanvasIdentifiers::Id);
        CanvasNode* const parentNode   = find(parentNodeId);

        if (parentNode == nullptr || parentNodeId == nodeId) {
            continue;
        }

        canvas.arrowManager.connectParentToChild(parentNode, node);
    }
}

void NodeManager::connectOutgoingArrows(const juce::ValueTree& nodeValueTree, CanvasNode* node)
{
    const int             nodeId          = nodeValueTree.getProperty(CanvasIdentifiers::Id);
    const juce::ValueTree nodeChildrenIds = nodeValueTree.getChildWithName(CanvasIdentifiers::NodeChildrenIds);

    for (int childIndex = 0; childIndex < nodeChildrenIds.getNumChildren(); ++childIndex) {
        const int         childNodeId = nodeChildrenIds.getChild(childIndex).getProperty(CanvasIdentifiers::Id);
        CanvasNode* const childNode   = find(childNodeId);

        if (childNode == nullptr || childNodeId == nodeId) {
            continue;
        }

        canvas.arrowManager.connectParentToChild(node, childNode);
    }
}

}
