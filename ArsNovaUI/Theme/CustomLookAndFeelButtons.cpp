#include "CustomLookAndFeel.h"
#include "IconLibrary.h"
#include "../Buttons/IconButton.h"
#include "../Editors/FileLabel.h"
#include "../Buttons/ValueSlider.h"

namespace ArsNovaUI
{

static void drawGlyph(juce::Graphics& graphics, juce::Rectangle<float> bounds, juce::Path glyph, GlyphPaint paint)
{
    const float          side     = juce::jmin(bounds.getWidth(), bounds.getHeight()) * Theme::glyphSizeRatio;
    const auto           area     = bounds.withSizeKeepingCentre(side, side);
    const float          scale    = side / Theme::glyphGridSize;
    const float          dashes[] = { 2.2f * scale, 2.4f * scale };
    juce::PathStrokeType stroke(side * Theme::glyphStrokeRatio, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    glyph.applyTransform(juce::AffineTransform::scale(scale).translated(area.getX(), area.getY()));

    switch (paint) {
        case GlyphPaint::Stroked:
            graphics.strokePath(glyph, stroke);
            break;

        case GlyphPaint::Dashed:
            stroke.createDashedStroke(glyph, glyph, dashes, 2);
            graphics.fillPath(glyph);
            break;

        case GlyphPaint::Filled:
            graphics.fillPath(glyph);
            break;

        case GlyphPaint::Tinted: {
            const juce::Graphics::ScopedSaveState savedState(graphics);

            graphics.setOpacity(Theme::glyphFillAlpha);
            graphics.fillPath(glyph);
            break;
        }
    }
}

static void drawLayers(juce::Graphics& graphics, juce::Rectangle<float> bounds, const std::vector<IconLayer>& layers)
{
    for (const IconLayer& layer : layers) {
        drawGlyph(graphics, bounds, layer.path, layer.paint);
    }
}

void CustomLookAndFeel::drawIcon(juce::Graphics& graphics, juce::Rectangle<float> bounds, const Icon& icon, const ButtonState& state)
{
    const bool usesSelectedLayers = state.isSelected && ! icon.selectedLayers.empty();

    juce::Colour colour = pressableButtonColour(state);

    if (icon.colour == IconColour::Accent) {
        colour = accentColour;
    }

    graphics.setColour(colour);

    if (usesSelectedLayers) {
        drawLayers(graphics, bounds, icon.selectedLayers);

        return;
    }

    drawLayers(graphics, bounds, icon.layers);
}

static void drawTitledButton(const CustomLookAndFeel& lookAndFeel, juce::Graphics& graphics, juce::Rectangle<float> bounds,
                             const ButtonState& state, const Icon& icon, float fontHeight)
{
    const auto       area       = bounds.reduced(lookAndFeel.borderThickness * 0.5f);
    const juce::Font labelFont  = lookAndFeel.font(Theme::FontStyle::Regular, fontHeight);
    const float      glyphWidth = area.getHeight() * Theme::glyphSizeRatio;
    const float      gap        = fontHeight * 0.5f;
    const float      groupWidth = glyphWidth + gap + juce::GlyphArrangement::getStringWidth(labelFont, state.text);
    auto             content    = area.withSizeKeepingCentre(juce::jmin(groupWidth, area.getWidth()), area.getHeight());
    const auto       glyphSlot  = content.removeFromLeft(glyphWidth);
    juce::Colour     fill       = lookAndFeel.raisedColour;

    if (state.isHovered || state.isDown || state.isSelected) {
        fill = lookAndFeel.selectedColour;
    }

    graphics.setColour(fill);
    graphics.fillRoundedRectangle(area, lookAndFeel.paneCornerRadius);

    graphics.setColour(lookAndFeel.borderStrongColour);
    graphics.drawRoundedRectangle(area, lookAndFeel.paneCornerRadius, lookAndFeel.borderThickness);

    graphics.setColour(lookAndFeel.textColour);

    drawLayers(graphics, glyphSlot.withSizeKeepingCentre(area.getHeight(), area.getHeight()), icon.layers);

    content.removeFromLeft(gap);

    graphics.setFont(labelFont);
    graphics.drawFittedText(state.text, content.toNearestInt(), juce::Justification::centredLeft, 1);
}

void CustomLookAndFeel::drawArrowButton(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state)
{
    drawTitledButton(*this, graphics, bounds, state, IconLibrary::get().arrow, labelFontHeight);
}

void CustomLookAndFeel::drawTextButton(juce::Graphics& graphics, const IconButton& button)
{
    const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();

    ButtonState tileState = button.state;

    tileState.text = {};

    drawButtonTile(graphics, bounds, tileState);

    graphics.setColour(pressableButtonColour(button.state));
    graphics.setFont(font(FontStyle::SemiBold, textHeight));
    graphics.drawFittedText(button.state.text, bounds.toNearestInt(), juce::Justification::centred, 1);
}

void CustomLookAndFeel::drawCodeButton(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state,
                                        float fontHeight)
{
    drawTitledButton(*this, graphics, bounds, state, IconLibrary::get().code, fontHeight);
}

void CustomLookAndFeel::drawIncrementIcon(juce::Graphics& graphics, juce::Rectangle<float> bounds, bool pointsUp)
{
    const float                  glyphSide   = bounds.getHeight() * incrementGlyphScale;
    const juce::Rectangle<float> glyphBounds = bounds.withSizeKeepingCentre(glyphSide, glyphSide);

    graphics.setColour(dimTextColour);

    if (pointsUp) {
        drawLayers(graphics, glyphBounds, IconLibrary::get().chevronUp.layers);

        return;
    }

    drawLayers(graphics, glyphBounds, IconLibrary::get().chevronDown.layers);
}

void CustomLookAndFeel::drawValueSlider(juce::Graphics& graphics, const ValueSlider& slider)
{
    const auto   track      = slider.getLocalBounds().toFloat().reduced(popupMenuItemInset, popupMenuItemGap);
    const double value      = slider.range.snapToLegalValue(static_cast<double>(slider.boundValue.getValue()));
    const float  proportion = static_cast<float>(slider.range.convertTo0to1(value));
    const float  handleX    = track.getX() + track.getWidth() * proportion;
    const auto   textArea   = track.reduced(popupMenuTextInset, 0.0f).toNearestInt();
    juce::Colour border     = borderColour;
    juce::Path   trackShape;

    if (slider.isMouseOverOrDragging()) {
        border = borderStrongColour;
    }

    trackShape.addRoundedRectangle(track, paneCornerRadius);

    graphics.setColour(surfaceColour);
    graphics.fillPath(trackShape);

    {
        const juce::Graphics::ScopedSaveState savedState(graphics);

        graphics.reduceClipRegion(trackShape);
        graphics.setColour(accentSoftColour);
        graphics.fillRect(track.withRight(handleX));
        graphics.setColour(accentColour);
        graphics.fillRect(juce::Rectangle<float>(valueSliderHandleWidth, track.getHeight()).withCentre({ handleX, track.getCentreY() }));
    }

    graphics.setColour(border);
    graphics.strokePath(trackShape, juce::PathStrokeType(borderThickness));

    graphics.setFont(getPopupMenuFont());
    graphics.setColour(captionColour);
    graphics.drawFittedText(slider.label, textArea, juce::Justification::centredLeft, 1);
    graphics.setColour(textColour);
    graphics.drawFittedText(juce::String(value, 2) + slider.suffix, textArea, juce::Justification::centredRight, 1);
}

void CustomLookAndFeel::drawFileLabel(juce::Graphics& graphics, const FileLabel& fileLabel)
{
    auto bounds = fileLabel.getLocalBounds().toFloat();

    juce::Colour background = surfaceColour;

    if (fileLabel.selected) {
        background = raisedColour;
    }

    if (fileLabel.grabbed) {
        background = selectedColour;
    }

    graphics.setColour(background);
    graphics.fillRect(bounds);

    if (fileLabel.selected) {
        graphics.setColour(accentColour);
        graphics.fillRect(bounds.withWidth(fileLabelMarkerWidth));
    }

    graphics.setColour(borderColour);
    graphics.drawHorizontalLine(static_cast<int>(bounds.getBottom()) - 1, bounds.getX(), bounds.getRight());
}

juce::Rectangle<float> CustomLookAndFeel::drawButtonTile(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state)
{
    juce::Colour fill = juce::Colours::transparentBlack;

    if (state.isHovered) {
        fill = hoverColour;
    }

    if (state.isDown) {
        fill = hoverColour.withMultipliedAlpha(2.0f);
    }

    if (state.isSelected) {
        switch (state.look) {
            case ButtonState::Look::Tinted: fill = accentSoftColour; break;
            case ButtonState::Look::Accent: fill = accentColour;     break;
            case ButtonState::Look::Raised: fill = selectedColour;   break;
        }
    }

    graphics.setColour(fill);
    graphics.fillRoundedRectangle(bounds, paneCornerRadius);

    if (state.isSelected && state.look == ButtonState::Look::Raised) {
        graphics.setColour(borderColour);
        graphics.drawRoundedRectangle(bounds.reduced(borderThickness * 0.5f), paneCornerRadius, borderThickness);
    }

    if (state.text.isEmpty()) {
        return bounds;
    }

    const juce::Font labelFont   = font(FontStyle::Regular, textHeight);
    const float      glyphInset  = bounds.getHeight() * (1.0f - Theme::glyphSizeRatio) * 0.5f;
    const float      labelIndent = bounds.getHeight() * 0.85f - glyphInset;
    const float      groupWidth  = labelIndent + juce::GlyphArrangement::getStringWidth(labelFont, state.text);
    const auto       content     = bounds.withSizeKeepingCentre(juce::jmin(groupWidth, bounds.getWidth()), bounds.getHeight());

    graphics.setColour(pressableButtonColour(state));
    graphics.setFont(labelFont);
    graphics.drawFittedText(state.text, content.withTrimmedLeft(labelIndent).toNearestInt(), juce::Justification::centredLeft, 1);

    return content.withWidth(bounds.getHeight()).translated(-glyphInset, 0.0f);
}

juce::Colour CustomLookAndFeel::pressableButtonColour(const ButtonState& state) const
{
    if (state.isSelected) {
        switch (state.look) {
            case ButtonState::Look::Tinted: return accentColour;
            case ButtonState::Look::Accent: return onAccentColour;
            case ButtonState::Look::Raised: return textColour;
        }
    }

    if (state.isHovered || state.isDown) {
        return textColour;
    }

    return softTextColour;
}

}
