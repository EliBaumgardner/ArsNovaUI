#include "ArrowGestures.h"
#include "EditCanvas.h"
#include "CanvasNode.h"
#include "CanvasArrow.h"
#include "CanvasDocument.h"

namespace ArsNovaUI
{

ArrowGestures::ArrowGestures(EditCanvas& canvas, CanvasDocument& document, juce::UndoManager& undoManager)
    : canvas(canvas),
      document(document),
      undoManager(undoManager)
{
}

void ArrowGestures::beginMovingTip(CanvasArrow& danglingArrow)
{
    draggingDanglingArrow = &danglingArrow;
    tipGesture            = TipGesture::MovingDanglingTip;

    canvas.showGrid();
}

void ArrowGestures::dragTip(juce::Point<int> cursor)
{
    CanvasNode* sourceNode = newArrowSourceNode;

    if (tipGesture == TipGesture::MovingDanglingTip && draggingDanglingArrow == nullptr) {
        return;
    }

    if (tipGesture == TipGesture::MovingDanglingTip) {
        sourceNode = draggingDanglingArrow->startNode;
    }

    if (sourceNode == nullptr) {
        return;
    }

    tipSnapTarget = findTipSnapTarget(*sourceNode, cursor);

    juce::Point<int>       tip    = canvas.snapPointToGrid(cursor);
    const juce::Point<int> centre = sourceNode->getNodeCentre();

    if (tipSnapTarget != nullptr) {
        tip = tipSnapTarget->getNodeCentre();
    }

    if (tipGesture == TipGesture::MovingDanglingTip) {
        draggingDanglingArrow->dragging = true;

        draggingDanglingArrow->setTipOffset({ tip.x - centre.x, tip.y - centre.y });
        return;
    }

    if (tipGesture == TipGesture::CreatingDanglingArrow) {
        canvas.showGrid();
    }

    canvas.arrowManager.updatePreview(sourceNode, { tip.x - centre.x, tip.y - centre.y });
}

void ArrowGestures::finishTip()
{
    const TipGesture  finishedGesture = tipGesture;
    CanvasNode* const targetNode      = tipSnapTarget;
    CanvasArrow*      draggedArrow    = canvas.arrowManager.preview.get();

    if (finishedGesture == TipGesture::MovingDanglingTip) {
        draggedArrow = draggingDanglingArrow;
    }

    tipGesture            = TipGesture::None;
    draggingDanglingArrow = nullptr;
    newArrowSourceNode    = nullptr;
    tipSnapTarget         = nullptr;

    canvas.hideGrid();

    if (draggedArrow == nullptr) {
        return;
    }

    CanvasNode* const sourceNode = draggedArrow->startNode;

    draggedArrow->endDrag();

    if (targetNode == nullptr && finishedGesture == TipGesture::MovingDanglingTip) {
        undoManager.beginNewTransaction();

        document.setDanglingArrowTip(draggedArrow->arrowTree, draggedArrow->tipOffset);
        return;
    }

    if (targetNode == nullptr && finishedGesture != TipGesture::ConnectingNode) {
        undoManager.beginNewTransaction();

        canvas.arrowManager.commitPreview();
        return;
    }

    canvas.arrowManager.preview.reset();

    if (targetNode != nullptr && sourceNode != nullptr && sourceNode->nodeArrows.count(targetNode->nodeId) == 0) {
        undoManager.beginNewTransaction();

        document.connect(sourceNode->nodeId, targetNode->nodeId, CanvasDocument::ConnectionGesture::TipOntoNode);

        canvas.arrowManager.connectWithSnapAnimation(sourceNode->nodeId, targetNode->nodeId);
    }

    if (targetNode != nullptr && finishedGesture == TipGesture::MovingDanglingTip) {
        document.removeDanglingArrow(draggedArrow->arrowTree);
    }
}

void ArrowGestures::updateHover(juce::Point<float> cursor)
{
    CanvasArrow* const hoveredArrow = canvas.hitTester.arrowNear(cursor, arrowHoverRadius);

    for (CanvasArrow* arrow : canvas.arrowManager.all()) {
        const bool shouldBold = (arrow == hoveredArrow);

        if (shouldBold != arrow->hovered) {
            arrow->hovered = shouldBold;

            arrow->repaint();
        }
    }
}

void ArrowGestures::removeArrow(CanvasArrow& arrow)
{
    if (arrow.isDangling()) {
        undoManager.beginNewTransaction();

        document.removeDanglingArrow(arrow.arrowTree);
        return;
    }

    if (arrow.startNode == nullptr || arrow.endNode == nullptr) {
        return;
    }

    undoManager.beginNewTransaction();

    document.disconnect(arrow.startNode->nodeId, arrow.endNode->nodeId);
}

void ArrowGestures::setArrowMode(bool enabled)
{
    arrowMode = enabled;

    if (! enabled) {
        canvas.arrowManager.preview.reset();
    }
}

bool ArrowGestures::beginTipFromNode(CanvasNode& node)
{
    const int growthNodeId = document.growFrom(node.nodeId);

    if (node.connectsByDrag && ! arrowMode) {
        tipGesture         = TipGesture::ConnectingNode;
        newArrowSourceNode = &node;
        return true;
    }

    if (! arrowMode || growthNodeId < 0) {
        return false;
    }

    tipGesture         = TipGesture::CreatingDanglingArrow;
    newArrowSourceNode = canvas.nodeManager.find(growthNodeId);

    return true;
}

CanvasNode* ArrowGestures::findTipSnapTarget(const CanvasNode& startNode, juce::Point<int> tip) const
{
    const int startNodeId = startNode.nodeId;

    if (tipGesture == TipGesture::ConnectingNode) {
        return canvas.hitTester.nodeNear(tip.toFloat(), connectSnapThreshold, startNodeId);
    }

    CanvasNode* const snapTarget = canvas.hitTester.nodeContaining(tip.toFloat(), startNodeId);

    if (snapTarget == nullptr || ! document.canConnect(startNode, *snapTarget)) {
        return nullptr;
    }

    return snapTarget;
}

}
