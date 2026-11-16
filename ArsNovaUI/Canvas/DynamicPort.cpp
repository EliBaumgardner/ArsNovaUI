#include "DynamicPort.h"

namespace ArsNovaUI
{

DynamicPort::DynamicPort(juce::Component& content)
    : content(content)
{
    setOpaque(false);

    addAndMakeVisible(content);
}

void DynamicPort::resized()
{
    content.setBounds(getLocalBounds());

    if (getWidth() <= 0 || getHeight() <= 0) {
        return;
    }

    if (!centeredOnce) {
        const juce::Point<float> portCentre { static_cast<float>(getWidth()) * 0.5f, static_cast<float>(getHeight()) * 0.5f };

        centeredOnce     = true;
        view.translation = portCentre - initialViewCentre * view.zoom;
    }

    applyTransform();
}

void DynamicPort::mouseDown(const juce::MouseEvent& event)
{
    lastMousePosition = event.getPosition();
}

void DynamicPort::mouseDrag(const juce::MouseEvent& event)
{
    if (!event.mods.isLeftButtonDown()) {
        return;
    }

    const juce::Point<int> delta = event.getPosition() - lastMousePosition;

    lastMousePosition = event.getPosition();

    view.translation += delta.toFloat();

    applyTransform();
}

void DynamicPort::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    const juce::Point<float> pivot = event.getEventRelativeTo(this).getPosition().toFloat();

    view.scroll(wheel, event.mods.isShiftDown(), pivot);

    applyTransform();
}

void DynamicPort::mouseMagnify(const juce::MouseEvent& event, float scaleFactor)
{
    const juce::Point<float> pivot = event.getEventRelativeTo(this).getPosition().toFloat();

    view.zoomBy(scaleFactor, pivot);

    applyTransform();
}

void DynamicPort::setZoom(float newZoom, juce::Point<float> pivot)
{
    const float zoomFactor = newZoom / view.zoom;

    view.zoomBy(zoomFactor, pivot);

    applyTransform();
}

void DynamicPort::applyTransform()
{
    const juce::AffineTransform viewTransform = view.transform();

    if (onTransformChanged) {
        onTransformChanged(viewTransform, view.zoom);
    }

    for (juce::Component* child : content.getChildren()) {
        child->setTransform(viewTransform);
    }

    content.repaint();
}

}
