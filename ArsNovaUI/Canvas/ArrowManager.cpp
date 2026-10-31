#include "ArrowManager.h"

#include "EditCanvas.h"
#include "NodeManager.h"
#include "CanvasArrow.h"
#include "CanvasNode.h"
#include "CanvasDocument.h"
#include "CanvasIdentifiers.h"

namespace ArsNovaUI
{

ArrowManager::ArrowManager(EditCanvas& canvas) : canvas(canvas)
{
}

ArrowManager::~ArrowManager() = default;

CanvasArrow* ArrowManager::find(int parentNodeId, int childNodeId) const
{
    for (CanvasArrow* const arrow : arrows) {
        if (arrow->startNode == nullptr || arrow->endNode == nullptr) {
            continue;
        }

        const int startId = arrow->startNode->nodeId;
        const int endId   = arrow->endNode->nodeId;

        if ((startId == parentNodeId && endId == childNodeId)
            || (startId == childNodeId && endId == parentNodeId)) {
            return arrow;
        }
    }

    return nullptr;
}

CanvasArrow* ArrowManager::connect(CanvasNode* parentNode, CanvasNode* childNode)
{
    std::unique_ptr<CanvasArrow> arrow        = canvas.makeArrow(parentNode, childNode);
    CanvasArrow* const           createdArrow = arrow.get();

    createdArrow->arrowTree = connectionTreeFor(parentNode->nodeId, childNode->nodeId);

    canvas.connectionCreated(*createdArrow);

    canvas.addAndMakeVisible(*createdArrow);

    createdArrow->toBack();
    createdArrow->setInterceptsMouseClicks(false, true);

    adopt(std::move(arrow));

    return createdArrow;
}

CanvasArrow* ArrowManager::connectParentToChild(CanvasNode* parentNode, CanvasNode* childNode)
{
    CanvasNode* startNode = parentNode;
    CanvasNode* endNode   = childNode;

    if (childNode->isAlternativeNode) {
        startNode = childNode;
        endNode   = parentNode;
    }

    if (startNode->nodeArrows.count(endNode->nodeId) > 0) {
        return nullptr;
    }

    CanvasArrow* const arrow = connect(startNode, endNode);

    refreshFor(endNode);

    return arrow;
}

void ArrowManager::adopt(std::unique_ptr<CanvasArrow> arrow)
{
    if (arrow == nullptr) {
        return;
    }

    if (arrow->startNode != nullptr) {
        arrow->startNode->nodeArrows.insert({ arrowKey(*arrow), arrow.get() });
    }

    arrows.add(arrow.release());
}

void ArrowManager::refreshFor(const CanvasNode* movedNode)
{
    for (CanvasArrow* const arrow : arrows) {
        CanvasNode* const parentNode = arrow->startNode;
        CanvasNode* const childNode  = arrow->endNode;

        if (parentNode != movedNode && childNode != movedNode) {
            continue;
        }

        if (! arrow->isDangling()) {
            parentNode->refreshValueDisplay();
            childNode->refreshValueDisplay();
        }

        arrow->dragging = nodesDragging;

        arrow->setArrowBounds();
    }
}

void ArrowManager::rebuildDanglingForNode(int nodeId)
{
    CanvasNode* const node = canvas.nodeManager.find(nodeId);

    removeMatching([node](CanvasArrow* arrow) {
        return arrow->isDangling() && arrow->startNode == node;
    });

    if (node == nullptr) {
        return;
    }

    const juce::ValueTree arrowList = node->nodeValueTree.getChildWithName(CanvasIdentifiers::DanglingArrows);

    if (! arrowList.isValid()) {
        return;
    }

    for (int danglingIndex = 0; danglingIndex < arrowList.getNumChildren(); ++danglingIndex) {
        const juce::ValueTree  arrowTree = arrowList.getChild(danglingIndex);
        const juce::Point<int> tipOffset { static_cast<int>(arrowTree.getProperty(CanvasIdentifiers::ArrowTipX)),
                                           static_cast<int>(arrowTree.getProperty(CanvasIdentifiers::ArrowTipY)) };

        std::unique_ptr<CanvasArrow> arrow = canvas.makeDanglingArrow(node, tipOffset);

        arrow->arrowTree     = arrowTree;
        arrow->danglingIndex = danglingIndex;

        canvas.prepareArrow(*arrow);

        canvas.addAndMakeVisible(*arrow);

        arrow->toBack();
        arrow->setVisible(! node->isEncapsulated || node->isEncapsulationExit);
        arrow->setArrowBounds();

        adopt(std::move(arrow));
    }
}

void ArrowManager::endNodeDrag()
{
    nodesDragging = false;

    for (CanvasArrow* const arrow : arrows) {
        if (arrow->dragging) {
            arrow->endDrag();
        }
    }
}

void ArrowManager::refreshEncapsulatedArrows()
{
    for (CanvasArrow* const arrow : arrows) {
        if (arrow->startNode == nullptr) {
            continue;
        }

        if (arrow->isDangling()) {
            arrow->setVisible(! arrow->startNode->isEncapsulated
                              || arrow->startNode->isEncapsulationExit);
            continue;
        }

        arrow->setVisible(! (arrow->startNode->isEncapsulated && arrow->endNode->isEncapsulated));
    }
}

void ArrowManager::remove(CanvasArrow* arrow)
{
    if (arrow == nullptr) {
        return;
    }

    const int index = arrows.indexOf(arrow);

    if (index >= 0) {
        detach(arrow);
        arrows.remove(index);
    }
}

void ArrowManager::removeForNode(const CanvasNode* node)
{
    if (preview != nullptr && preview->startNode == node) {
        preview.reset();
    }

    if (snapGhostArrow != nullptr
        && (snapGhostArrow->startNode == node || snapGhostArrow->endNode == node)) {
        hideSnapGhost();
    }

    removeMatching([node](CanvasArrow* arrow) {
        return arrow->startNode == node || arrow->endNode == node;
    });
}

void ArrowManager::removeMatching(const std::function<bool(CanvasArrow*)>& predicate)
{
    for (int arrowIndex = arrows.size() - 1; arrowIndex >= 0; --arrowIndex) {
        CanvasArrow* const arrow = arrows[arrowIndex];

        if (predicate(arrow)) {
            detach(arrow);

            arrows.remove(arrowIndex);
        }
    }
}

void ArrowManager::clear()
{
    hideSnapGhost();
    preview.reset();
    arrows.clear();
}

void ArrowManager::handleArrowAdded(int parentNodeId, int childNodeId)
{
    CanvasNode* const parentNode = canvas.nodeManager.find(parentNodeId);
    CanvasNode* const childNode  = canvas.nodeManager.find(childNodeId);

    if (parentNode == nullptr || childNode == nullptr) {
        return;
    }

    connectParentToChild(parentNode, childNode);
}

void ArrowManager::handleArrowInfoChanged(int parentNodeId, int childNodeId)
{
    CanvasArrow* const arrow = find(parentNodeId, childNodeId);

    if (arrow != nullptr) {
        arrow->arrowTree = connectionTreeFor(parentNodeId, childNodeId);

        arrow->repaint();
    }
}

void ArrowManager::updatePreview(CanvasNode* node, juce::Point<int> tipOffset)
{
    if (node == nullptr) {
        return;
    }

    if (preview == nullptr || preview->startNode != node) {
        preview = canvas.makeDanglingArrow(node, tipOffset);

        preview->dragging = true;

        canvas.addAndMakeVisible(*preview);

        preview->toBack();
    }

    preview->setTipOffset(tipOffset);
}

void ArrowManager::commitPreview()
{
    if (preview == nullptr) {
        return;
    }

    const CanvasNode* const node      = preview->startNode;
    const juce::Point<int>  tipOffset = preview->tipOffset;

    preview.reset();

    if (node == nullptr) {
        return;
    }

    canvas.document.addDanglingArrow(node->nodeValueTree, tipOffset);
}

void ArrowManager::showSnapGhost(CanvasNode* from, CanvasNode* to)
{
    if (snapGhostArrow != nullptr
        && snapGhostArrow->startNode == from && snapGhostArrow->endNode == to) {
        return;
    }

    hideSnapGhost();

    snapGhostArrow = canvas.makeArrow(from, to);

    snapGhostArrow->isGhost = true;

    canvas.addAndMakeVisible(*snapGhostArrow);

    snapGhostArrow->toBack();
    snapGhostArrow->setInterceptsMouseClicks(false, false);
    snapGhostArrow->setArrowBounds();
    snapGhostArrow->triggerSnapAnimation();
}

void ArrowManager::triggerSnapForNode(int nodeId)
{
    CanvasNode* const node = canvas.nodeManager.find(nodeId);

    if (node == nullptr) {
        return;
    }

    for (CanvasArrow* const arrow : arrows) {
        if (arrow->endNode == node) {
            arrow->triggerSnapAnimation();
            return;
        }
    }
}

void ArrowManager::connectWithSnapAnimation(int parentNodeId, int childNodeId)
{
    CanvasNode* const parentNode = canvas.nodeManager.find(parentNodeId);
    CanvasNode* const childNode  = canvas.nodeManager.find(childNodeId);

    if (parentNode == nullptr || childNode == nullptr) {
        return;
    }

    connect(parentNode, childNode);
    refreshFor(parentNode);

    auto arrowIterator = parentNode->nodeArrows.find(childNodeId);

    if (arrowIterator != parentNode->nodeArrows.end()) {
        arrowIterator->second->triggerSnapAnimation();
    }
}

void ArrowManager::hideSnapGhost()
{
    if (snapGhostArrow != nullptr) {
        canvas.removeChildComponent(snapGhostArrow.get());

        snapGhostArrow.reset();
    }
}

void ArrowManager::setSelected(CanvasArrow* arrow)
{
    clearSelection();

    if (arrow != nullptr && ! arrow->selected) {
        arrow->selected = true;

        arrow->repaint();
    }
}

void ArrowManager::clearSelection()
{
    for (CanvasArrow* const arrow : arrows) {
        if (arrow->selected) {
            arrow->selected = false;

            arrow->repaint();
        }
    }
}

void ArrowManager::detach(CanvasArrow* arrow)
{
    if (arrow->startNode != nullptr) {
        auto&      nodeArrows = arrow->startNode->nodeArrows;
        const auto arrowRange = nodeArrows.equal_range(arrowKey(*arrow));

        for (auto entry = arrowRange.first; entry != arrowRange.second; ++entry) {
            if (entry->second == arrow) {
                nodeArrows.erase(entry);
                break;
            }
        }
    }

    canvas.removeChildComponent(arrow);
}

juce::ValueTree ArrowManager::connectionTreeFor(int startNodeId, int endNodeId) const
{
    const juce::ValueTree connection = canvas.document.findNode(startNodeId).getChildWithName(CanvasIdentifiers::NodeChildrenIds).getChildWithProperty(CanvasIdentifiers::Id, endNodeId);

    if (connection.isValid()) {
        return connection;
    }

    return canvas.document.findNode(endNodeId).getChildWithName(CanvasIdentifiers::NodeChildrenIds).getChildWithProperty(CanvasIdentifiers::Id, startNodeId);
}

int ArrowManager::arrowKey(const CanvasArrow& arrow)
{
    if (arrow.isDangling()) {
        return -(arrow.danglingIndex + 1);
    }

    return arrow.endNode->nodeId;
}

}
