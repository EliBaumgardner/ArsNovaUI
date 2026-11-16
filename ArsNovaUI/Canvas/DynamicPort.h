#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "PanZoom.h"

namespace ArsNovaUI
{

class DynamicPort : public juce::Component
{
public:

    explicit DynamicPort(juce::Component& content);

    void resized() override;

    void mouseDown     (const juce::MouseEvent& event) override;
    void mouseDrag     (const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void mouseMagnify  (const juce::MouseEvent& event, float scaleFactor) override;

    void setZoom(float newZoom, juce::Point<float> pivot);

    static constexpr float minimumZoom = 0.1f;
    static constexpr float maximumZoom = 5.0f;

    std::function<void(const juce::AffineTransform& viewTransform, float zoom)> onTransformChanged;

    PanZoom view { minimumZoom, maximumZoom };

    juce::Point<float> initialViewCentre { 1500.0f, 1500.0f };

private:

    void applyTransform();

    juce::Component& content;

    bool centeredOnce = false;

    juce::Point<int> lastMousePosition;
};

}
