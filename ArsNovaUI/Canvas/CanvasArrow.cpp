#include "CanvasArrow.h"
#include "CanvasNode.h"
#include "../Theme/CustomLookAndFeel.h"

#include <algorithm>
#include <cmath>

namespace ArsNovaUI
{

CanvasArrow::CanvasArrow(CanvasNode* startNode, CanvasNode* endNode, juce::UndoManager& undoManager)
    : startNode(startNode), endNode(endNode)
{
    buildDurationEditor(undoManager);
}

CanvasArrow::CanvasArrow(CanvasNode* startNode, juce::Point<int> tipOffset, juce::UndoManager& undoManager)
    : startNode(startNode), tipOffset(tipOffset)
{
    setInterceptsMouseClicks(false, true);

    buildDurationEditor(undoManager);
}

void CanvasArrow::resized()
{
    const ArrowGeometry geometry = getGeometry(1.0f);

    if (geometry.valid) {
        const ArrowLabel label = getLabel(geometry, arrowHeadLength);

        const juce::Point<int> editorCentre = (label.centre - getPosition().toFloat()).roundToInt()
                                            - juce::Point<int>(0, valueEditorHeight / 2);

        durationEditor->setBounds(juce::Rectangle<int>(0, 0, valueEditorWidth, valueEditorHeight).withCentre(editorCentre));
    }
}

void CanvasArrow::setArrowBounds()
{
    if (startNode == nullptr) {
        return;
    }

    if (endNode != nullptr) {
        const juce::Point<int> delta = getTip() - startNode->getNodeCentre();

        endNode->incomingAngle = std::atan2(static_cast<float>(delta.y), static_cast<float>(delta.x));

        endNode->incomingAngleChanged();
    }

    const ArrowGeometry geometry = getGeometry(1.0f);

    if (! geometry.valid) {
        const juce::Point<int> centre = startNode->getNodeCentre();

        setBounds(juce::Rectangle<int>(centre, centre).expanded(arrowBoundsPadding));
        repaint();
        return;
    }

    const juce::Path shaft = buildShaftPath(geometry, 0.0f, {});

    setBounds(shaft.getBounds().expanded(static_cast<float>(arrowBoundsPadding)).toNearestInt());

    repaint();
}

void CanvasArrow::setTipOffset(juce::Point<int> offset)
{
    tipOffset = offset;

    setArrowBounds();
}

void CanvasArrow::endDrag()
{
    dragging            = false;
    animation.rulerFade = 1.0f;

    if (animationFrames.isEmpty()) {
        animationFrames = juce::VBlankAttachment(this, [this](double frameSec) { advanceAnimation(frameSec); });
    }
}

void CanvasArrow::triggerSnapAnimation()
{
    animation.snapProgress = 0.0f;
    animation.snapVelocity = 0.0f;

    if (dragging) {
        animation.rulerFade = 1.0f;
    }

    if (animationFrames.isEmpty()) {
        animationFrames = juce::VBlankAttachment(this, [this](double frameSec) { advanceAnimation(frameSec); });
    }
}

void CanvasArrow::advanceAnimation(double frameSec)
{
    const bool extraRunning   = advanceExtraAnimation();
    const bool stillAnimating = animation.advance(frameSec, extraRunning);

    setAlpha(animation.alpha);

    if (animation.alpha <= 0.0f && isVisible()) {
        setVisible(false);
    }

    repaint();

    if (! stillAnimating) {
        animationFrames = {};
    }
}

void CanvasArrow::setHoverFade(bool shouldBeVisible)
{
    animation.alphaTarget = 0.0f;

    if (shouldBeVisible) {
        animation.alphaTarget = 1.0f;
    }

    if (shouldBeVisible && ! isVisible()) {
        setVisible(true);
    }

    if (animationFrames.isEmpty()) {
        animationFrames = juce::VBlankAttachment(this, [this](double frameSec) { advanceAnimation(frameSec); });
    }
}

void CanvasArrow::initHoverState(bool visibleNow)
{
    animation.alpha = 0.0f;

    if (visibleNow) {
        animation.alpha = 1.0f;
    }

    animation.alphaTarget = animation.alpha;

    setAlpha(animation.alpha);
    setVisible(visibleNow);
}

bool CanvasArrow::advanceExtraAnimation()
{
    return false;
}

ArrowGeometry CanvasArrow::getGeometry(float animationProgress) const
{
    ArrowGeometry geometry;

    if (startNode == nullptr) {
        return geometry;
    }

    const juce::Point<float> centre = startNode->getNodeCentre().toFloat();
    const juce::Point<float> target = getTip().toFloat();
    const juce::Point<float> delta  = (target - centre) * animationProgress;

    const float length = delta.getDistanceFromOrigin();

    if (length < 1.0f) {
        return geometry;
    }

    const juce::Point<float> direction = delta / length;
    juce::Point<float>       start     = centre;
    juce::Point<float>       tip       = centre + delta;

    const bool endIsHeadless = endNode != nullptr && ! endNode->takesArrowHeads;

    if (isDangling()) {
        start += direction * startNode->getVisualRadius();
    }
    else if (endIsHeadless) {
        tip += direction * endNode->arrowTipReach;
    }
    else {
        tip -= direction * endNode->getBodyExtent(direction);
    }

    const juce::Point<float> shaft = tip - start;

    const float shaftLength = shaft.getDistanceFromOrigin();

    geometry.centre    = centre;
    geometry.start     = start;
    geometry.tip       = tip;
    geometry.direction = direction;
    geometry.chord     = direction;

    if (shaftLength > 0.0f) {
        geometry.chord = shaft / shaftLength;
    }

    geometry.length   = length;
    geometry.drawHead = ! endIsHeadless && animationProgress > headVisibleThreshold;
    geometry.straight = isDangling() || endIsHeadless
                      || std::abs(shaft.x) < 1.0f || std::abs(shaft.y) < 1.0f;
    geometry.valid    = true;

    if (geometry.straight) {
        return geometry;
    }

    const float sign   = std::copysign(1.0f, shaft.x);
    const float offset = shaftLength * curveOffsetFactor;

    const juce::Point<float> perpendicular { -direction.y * curvePerpScale * sign,
                                              direction.x * curvePerpScale * sign };

    geometry.control1 = start + shaft * 0.33f + perpendicular * offset;
    geometry.control2 = start + shaft * 0.67f - perpendicular * offset;

    const juce::Point<float> neck = tip - geometry.control2;

    const float neckLength = neck.getDistanceFromOrigin();

    if (neckLength > 0.0f) {
        geometry.direction = neck / neckLength;
    }

    return geometry;
}

ArrowLabel CanvasArrow::getLabel(const ArrowGeometry& geometry, float headLength) const
{
    ArrowLabel label;

    if (startNode == nullptr) {
        return label;
    }

    const juce::Point<float> delta = geometry.tip - geometry.centre;

    juce::Point<float> shaftStart = geometry.centre;
    juce::Point<float> shaftEnd   = geometry.tip;

    const float length = delta.getDistanceFromOrigin();

    if (length > 0.0f) {
        const juce::Point<float> unit = delta / length;

        shaftStart += unit * startNode->getVisualRadius();
        shaftEnd   -= unit * headLength;
    }

    label.centre = (shaftStart + shaftEnd) * 0.5f;

    float angle = std::atan2(delta.y, delta.x);

    const float halfPi = juce::MathConstants<float>::halfPi;

    while (angle > halfPi) {
        angle -= juce::MathConstants<float>::pi;
    }

    while (angle <= -halfPi) {
        angle += juce::MathConstants<float>::pi;
    }

    if (std::abs(delta.x) < std::abs(delta.y) * verticalLabelThreshold) {
        angle = 0.0f;
    }

    label.angle = angle;

    return label;
}

juce::Path CanvasArrow::buildShaftPath(const ArrowGeometry& geometry, float headLength, juce::Point<float> origin) const
{
    juce::Path path;

    if (! geometry.valid) {
        return path;
    }

    const juce::Point<float> start = geometry.start - origin;
    const juce::Point<float> tip   = geometry.tip   - origin;

    juce::Point<float> shaftEnd = tip;

    if (geometry.drawHead) {
        shaftEnd = tip - geometry.direction * headLength;
    }

    path.startNewSubPath(start);

    if (geometry.straight) {
        path.lineTo(shaftEnd);
        return path;
    }

    path.cubicTo(geometry.control1 - origin, geometry.control2 - origin, shaftEnd);
    return path;
}

juce::Point<int> CanvasArrow::getTip() const
{
    if (endNode != nullptr) {
        return endNode->getNodeCentre();
    }

    if (startNode == nullptr) {
        return tipOffset;
    }

    return startNode->getNodeCentre() + tipOffset;
}

juce::Point<float> CanvasArrow::getHeadAnchor() const
{
    if (isDangling()) {
        return getTip().toFloat();
    }

    const ArrowGeometry geometry = getGeometry(1.0f);

    if (! geometry.valid) {
        return getTip().toFloat();
    }

    const float endExtent = endNode->getBodyExtent(geometry.chord);

    return endNode->getNodeCentre().toFloat() - geometry.chord * (endExtent + headAnchorInset);
}

void CanvasArrow::buildDurationEditor(juce::UndoManager& undoManager)
{
    durationEditor = std::make_unique<ValueEditor>(undoManager);

    durationEditor->setInterceptsMouseClicks(true, false);
    durationEditor->setTooltip("Arrow Duration");

    durationEditor->onEditFinished = [this] {
        editingDuration = false;

        durationEditor->setVisible(false);
        repaint();
    };

    addChildComponent(*durationEditor);
}

}
