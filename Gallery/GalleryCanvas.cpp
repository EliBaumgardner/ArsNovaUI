#include "GalleryCanvas.h"
#include <ArsNovaUI/Theme/CustomLookAndFeel.h>

GalleryCanvas::GalleryCanvas()
{
    const juce::Rectangle<float> nodeBounds   { 40.0f, 52.0f, nodeSide, nodeSide };
    const juce::Rectangle<float> squareBounds { 40.0f, 168.0f, nodeSide, nodeSide };
    const juce::Colour           nodeColour = juce::Colour::fromRGB(96, 150, 220);
    const juce::Colour           ringColour = juce::Colour::fromRGB(220, 170, 90);
    juce::Path                   wave;
    juce::Path                   dot;

    setInterceptsMouseClicks(false, false);

    grid.spacing = rulerSpacing;
    grid.visible = true;

    highlights[1] = juce::Colour::fromRGB(220, 96, 110);
    highlights[2] = juce::Colour::fromRGB(110, 200, 140);

    for (int index = 0; index < plainCircleCount; ++index) {
        circleNodes.push_back({ nodeBounds.translated(nodeSpacing * index, 0.0f), nodeColour, noHighlights, false, false, false, false, false, ringColour });
    }

    for (int index = 0; index < plainSquareCount; ++index) {
        squareNodes.push_back({ squareBounds.translated(nodeSpacing * index, 0.0f), nodeColour, noHighlights, false, false, false, false, false, ringColour });
    }

    circleNodes.push_back({ nodeBounds.translated(nodeSpacing * plainCircleCount, 0.0f), nodeColour, highlights, false, false, false, false, false, ringColour });
    squareNodes.push_back({ squareBounds.translated(nodeSpacing * plainSquareCount, 0.0f), nodeColour, highlights, false, false, false, false, false, ringColour });

    circleNodes[1].isHovered    = true;
    circleNodes[2].isSelected   = true;
    circleNodes[3].isOutlined   = true;
    circleNodes[4].hasOuterRing = true;
    circleNodes[5].hasInnerRim  = true;
    squareNodes[1].isHovered    = true;
    squareNodes[2].isSelected   = true;

    arrows.push_back(makeArrow({ 40.0f, 280.0f }, { 160.0f, 280.0f }, ArrowVisual::ShaftStyle::Solid));
    arrows.push_back(makeArrow({ 200.0f, 280.0f }, { 320.0f, 280.0f }, ArrowVisual::ShaftStyle::Dashed));
    arrows.push_back(makeArrow({ 360.0f, 280.0f }, { 480.0f, 280.0f }, ArrowVisual::ShaftStyle::Ruler));
    arrows.push_back(makeArrow({ 520.0f, 280.0f }, { 640.0f, 280.0f }, ArrowVisual::ShaftStyle::Solid));
    arrows.push_back(makeArrow({ 40.0f, 370.0f }, { 160.0f, 370.0f }, ArrowVisual::ShaftStyle::Solid));
    arrows.push_back(makeArrow({ 200.0f, 370.0f }, { 320.0f, 370.0f }, ArrowVisual::ShaftStyle::Dashed));
    arrows.push_back(makeArrow({ 360.0f, 370.0f }, { 480.0f, 370.0f }, ArrowVisual::ShaftStyle::Solid));
    arrows.push_back(makeArrow({ 520.0f, 370.0f }, { 640.0f, 370.0f }, ArrowVisual::ShaftStyle::Solid));

    arrows[3].head       = ArrowVisual::HeadStyle::Outlined;
    arrows[4].emphasised = true;
    arrows[4].headLength = emphasisedHeadLength;
    arrows[5].ghost      = true;
    arrows[6].showsLabel = true;
    arrows[6].labelText  = "1/4";
    arrows[6].label      = { { 420.0f, 370.0f }, 0.0f };

    arrows[7].trails.push_back({ highlights[1], 0.75f, ArrowTrail::Stroke::Solid });
    arrows[7].trails.push_back({ highlights[2], 0.45f, ArrowTrail::Stroke::Dashed });

    wave.startNewSubPath(0.15f, 0.5f);
    wave.cubicTo(0.3f, 0.2f, 0.4f, 0.2f, 0.5f, 0.5f);
    wave.cubicTo(0.6f, 0.8f, 0.7f, 0.8f, 0.85f, 0.5f);

    dot.startNewSubPath(0.5f, 0.78f);
    dot.lineTo(0.5f, 0.8f);

    symbol.push_back({ wave, symbolStrokeWidth });
    symbol.push_back({ dot, symbolStrokeWidth });
}

void GalleryCanvas::paint(juce::Graphics& graphics)
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    lookAndFeel.drawCanvas(graphics, grid);

    graphics.addTransform(grid.viewTransform);

    lookAndFeel.drawRaisedPanel(graphics, panelBounds);

    for (const NodeVisual& visual : circleNodes) {
        lookAndFeel.drawCircleNode(graphics, visual);
    }

    for (const NodeVisual& visual : squareNodes) {
        lookAndFeel.drawSquareNode(graphics, visual);
    }

    for (const ArrowVisual& visual : arrows) {
        lookAndFeel.drawArrow(graphics, visual);
    }

    graphics.setColour(lookAndFeel.textColour);

    lookAndFeel.drawStrokeSymbol(graphics, symbolBounds, symbol);
    lookAndFeel.drawSymbolMark(graphics, markBounds, SymbolMark::Symbol, symbol);
    lookAndFeel.drawSymbolMark(graphics, missingBounds, SymbolMark::Missing, symbol);
}

void GalleryCanvas::resized()
{
    grid.bounds = getLocalBounds().toFloat();
}

ArrowVisual GalleryCanvas::makeArrow(juce::Point<float> start, juce::Point<float> tip, ArrowVisual::ShaftStyle shaftStyle) const
{
    const juce::Point<float> delta     = tip - start;
    const float              length    = delta.getDistanceFromOrigin();
    const juce::Point<float> direction = delta / length;
    ArrowVisual              visual;

    visual.geometry.centre    = start;
    visual.geometry.start     = start;
    visual.geometry.tip       = tip;
    visual.geometry.direction = direction;
    visual.geometry.chord     = direction;
    visual.geometry.length    = length;
    visual.geometry.drawHead  = true;
    visual.geometry.valid     = true;
    visual.shaftStyle         = shaftStyle;
    visual.headLength         = headLength;
    visual.rulerSpacing       = rulerSpacing;

    if (shaftStyle == ArrowVisual::ShaftStyle::Ruler) {
        visual.rulerOpacity = 1.0f;
    }

    visual.shaft.startNewSubPath(start);
    visual.shaft.lineTo(tip - direction * headLength);

    return visual;
}
