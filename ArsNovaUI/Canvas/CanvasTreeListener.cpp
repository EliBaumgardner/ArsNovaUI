#include "CanvasTreeListener.h"
#include "EditCanvas.h"
#include "CanvasDocument.h"
#include "CanvasIdentifiers.h"

namespace ArsNovaUI
{

CanvasTreeListener::CanvasTreeListener(EditCanvas& canvas) : canvas(canvas)
{
}

void CanvasTreeListener::valueTreeChildAdded(juce::ValueTree& parent, juce::ValueTree& child)
{
    if (parent == canvas.document.nodes) {
        jassert(child.hasProperty(CanvasIdentifiers::Id));

        canvas.asyncUpdates.push_back({ .type = EditCanvas::AsyncUpdateType::NodeAdded, .nodeId = child.getProperty(CanvasIdentifiers::Id) });
    }
    else if (parent.getType() == CanvasIdentifiers::NodeChildrenIds) {
        canvas.asyncUpdates.push_back({ .type        = EditCanvas::AsyncUpdateType::ArrowAdded,
                                        .nodeId      = parent.getParent().getProperty(CanvasIdentifiers::Id),
                                        .childNodeId = child.getProperty(CanvasIdentifiers::Id) });
    }
    else if (child.getType() == CanvasIdentifiers::DanglingArrows) {
        enqueueDanglingArrowsChanged(parent);
    }
    else if (child.getType() == CanvasIdentifiers::DanglingArrow) {
        enqueueDanglingArrowsChanged(parent.getParent());
    }
}

void CanvasTreeListener::valueTreeChildRemoved(juce::ValueTree& parent, juce::ValueTree& child, int childIndex)
{
    if (parent == canvas.document.nodes) {
        canvas.asyncUpdates.push_back({ .type = EditCanvas::AsyncUpdateType::NodeRemoved, .nodeId = child.getProperty(CanvasIdentifiers::Id) });
    }
    else if (parent.getType() == CanvasIdentifiers::NodeChildrenIds) {
        canvas.asyncUpdates.push_back({ .type        = EditCanvas::AsyncUpdateType::ArrowRemoved,
                                        .nodeId      = parent.getParent().getProperty(CanvasIdentifiers::Id),
                                        .childNodeId = child.getProperty(CanvasIdentifiers::Id) });
    }
    else if (child.getType() == CanvasIdentifiers::DanglingArrows) {
        enqueueDanglingArrowsChanged(parent);
    }
    else if (child.getType() == CanvasIdentifiers::DanglingArrow) {
        enqueueDanglingArrowsChanged(parent.getParent());
    }
}

void CanvasTreeListener::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& propertyIdentifier)
{
    if (propertyIdentifier == CanvasIdentifiers::XPosition || propertyIdentifier == CanvasIdentifiers::YPosition) {
        jassert(tree.hasProperty(CanvasIdentifiers::Id));

        canvas.asyncUpdates.push_back({ .type = EditCanvas::AsyncUpdateType::NodeMoved, .nodeId = tree.getProperty(CanvasIdentifiers::Id) });
    }
    else if (propertyIdentifier == CanvasIdentifiers::ArrowTipX || propertyIdentifier == CanvasIdentifiers::ArrowTipY) {
        enqueueDanglingArrowsChanged(tree.getParent().getParent());
    }
    else {
        canvas.treePropertyChanged(tree, propertyIdentifier);
    }
}

void CanvasTreeListener::enqueueDanglingArrowsChanged(const juce::ValueTree& nodeTree) const
{
    if (! nodeTree.isValid()) {
        return;
    }

    canvas.asyncUpdates.push_back({ .type = EditCanvas::AsyncUpdateType::DanglingArrowsChanged, .nodeId = nodeTree.getProperty(CanvasIdentifiers::Id) });
}

}
