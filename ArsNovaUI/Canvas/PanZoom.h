#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ArsNovaUI
{

class PanZoom
{
public:

    PanZoom(float minimumZoom, float maximumZoom);

    void scroll(const juce::MouseWheelDetails& wheel, bool zooming, juce::Point<float> pivot);
    void zoomBy(float factor, juce::Point<float> pivot);

    juce::AffineTransform transform() const;

    static constexpr float wheelZoomStep       = 0.15f;
    static constexpr float wheelScrollDistance = 100.0f;

    juce::Point<float> translation;

    float minimumZoom;
    float maximumZoom;
    float zoom = 1.0f;
};

}
