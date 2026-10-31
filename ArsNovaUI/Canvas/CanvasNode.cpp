#include "CanvasNode.h"
#include "../Theme/CustomLookAndFeel.h"

#include <algorithm>
#include <cmath>

namespace ArsNovaUI
{

CanvasNode::CanvasNode(juce::UndoManager& undoManager)
    : nodeValueEditor(undoManager),
      undoManager(undoManager)
{
}

void CanvasNode::setSelectVisual(bool isSelected)
{
    this->isSelected = isSelected;

    if (onSelected) {
        onSelected(this, isSelected);
    }

    repaint();
}

void CanvasNode::setSelectVisual()
{
    isSelected = !isSelected;

    if (onSelected) {
        onSelected(this, isSelected);
    }

    repaint();
}

void CanvasNode::refreshValueDisplay()
{
    nodeValueEditor.repaint();
    repaint();
}

void CanvasNode::setHoverVisual(bool isHovered)
{
    this->isHovered = isHovered;

    repaint();
}

void CanvasNode::respondToClick(juce::Point<int> localPoint)
{
    if (nodeValueEditor.isVisible() && nodeValueEditor.getBounds().contains(localPoint)) {
        nodeValueEditor.beginEditing();
    }
}

void CanvasNode::incomingAngleChanged()
{
}

juce::Point<int> CanvasNode::getNodeCentre() const
{
    return getBounds().getCentre();
}

juce::Rectangle<int> CanvasNode::getBoundsAt(juce::Point<int> position) const
{
    const int radius = Theme::nodeRadius;

    return juce::Rectangle<int>(radius * 2, radius * 2).withCentre(position);
}

float CanvasNode::getVisualRadius() const
{
    return getHeight() * 0.5f;
}

float CanvasNode::getBodyExtent(juce::Point<float> approachDirection) const
{
    const juce::Rectangle<float> circle    = CustomLookAndFeel::getNodeCircleBounds(getLocalBounds().toFloat());
    const float                  radius    = std::max(0.0f, circle.getWidth() * 0.5f);
    const juce::Point<float>     toCircle  = circle.getCentre() - (getNodeCentre() - getPosition()).toFloat();
    const float                  along     = approachDirection.getDotProduct(toCircle);
    const float                  clearance = along * along - toCircle.getDistanceSquaredFromOrigin() + radius * radius;

    if (clearance <= 0.0f) {
        return radius;
    }

    return std::max(0.0f, std::sqrt(clearance) - along);
}

}
