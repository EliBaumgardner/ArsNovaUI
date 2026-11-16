#include "PanZoom.h"

#include <algorithm>

namespace ArsNovaUI
{

PanZoom::PanZoom(float minimumZoom, float maximumZoom)
    : minimumZoom(minimumZoom),
      maximumZoom(maximumZoom)
{
}

void PanZoom::scroll(const juce::MouseWheelDetails& wheel, bool zooming, juce::Point<float> pivot)
{
    float scrollX = wheel.deltaX;
    float scrollY = wheel.deltaY;

    if (wheel.isReversed) {
        scrollX = -wheel.deltaX;
        scrollY = -wheel.deltaY;
    }

    if (zooming) {
        const float zoomFactor = 1.0f + scrollY * wheelZoomStep;

        zoomBy(zoomFactor, pivot);
        return;
    }

    translation.x -= scrollX * wheelScrollDistance;
    translation.y -= scrollY * wheelScrollDistance;
}

void PanZoom::zoomBy(float factor, juce::Point<float> pivot)
{
    const float newZoom = std::clamp(zoom * factor, minimumZoom, maximumZoom);

    const juce::Point<float> contentPivot = (pivot - translation) / zoom;

    zoom        = newZoom;
    translation = pivot - contentPivot * zoom;
}

juce::AffineTransform PanZoom::transform() const
{
    return juce::AffineTransform::scale(zoom).translated(translation.x, translation.y);
}

}
