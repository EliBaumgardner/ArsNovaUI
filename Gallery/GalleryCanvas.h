#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ArsNovaUI/Theme/Visuals.h>
#include <map>
#include <vector>

class GalleryCanvas : public juce::Component
{
public:

    GalleryCanvas();

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    static constexpr int   plainCircleCount     = 6;
    static constexpr int   plainSquareCount     = 3;
    static constexpr float nodeSide             = 48.0f;
    static constexpr float nodeSpacing          = 80.0f;
    static constexpr float headLength           = 9.0f;
    static constexpr float emphasisedHeadLength = 11.0f;
    static constexpr float rulerSpacing         = 50.0f;
    static constexpr float symbolStrokeWidth    = 0.08f;

    CanvasGrid grid;

private:

    ArrowVisual makeArrow(juce::Point<float> start, juce::Point<float> tip, ArrowVisual::ShaftStyle shaftStyle) const;

    std::map<int, juce::Colour> noHighlights;
    std::map<int, juce::Colour> highlights;
    std::vector<NodeVisual>     circleNodes;
    std::vector<NodeVisual>     squareNodes;
    std::vector<ArrowVisual>    arrows;
    std::vector<SymbolStroke>   symbol;

    juce::Rectangle<float> panelBounds   { 20.0f, 20.0f, 600.0f, 112.0f };
    juce::Rectangle<float> symbolBounds  { 40.0f, 440.0f, 64.0f, 64.0f };
    juce::Rectangle<float> markBounds    { 140.0f, 456.0f, 32.0f, 32.0f };
    juce::Rectangle<float> missingBounds { 200.0f, 456.0f, 32.0f, 32.0f };
};
