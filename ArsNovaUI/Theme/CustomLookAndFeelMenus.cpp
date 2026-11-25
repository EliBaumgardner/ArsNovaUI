#include "CustomLookAndFeel.h"
#include "ArsNovaUIFonts.h"
#include "IconLibrary.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace ArsNovaUI
{

CustomLookAndFeel::CustomLookAndFeel()
{
    const int                  cursorPixels = static_cast<int>(glyphGridSize * cursorScale);
    const juce::PathStrokeType cursorInk(glyphGridSize * glyphStrokeRatio, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    const juce::PathStrokeType cursorHalo(cursorHaloWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    const juce::Path&          pencilGlyph  = IconLibrary::get().pencil.layers.front().path;
    const juce::Path&          eraserGlyph  = IconLibrary::get().eraser.layers.front().path;
    juce::Image                pencilImage(juce::Image::ARGB, cursorPixels, cursorPixels, true, juce::SoftwareImageType());
    juce::Image                eraserImage(juce::Image::ARGB, cursorPixels, cursorPixels, true, juce::SoftwareImageType());
    juce::Graphics             pencilGraphics(pencilImage);
    juce::Graphics             eraserGraphics(eraserImage);

    regularTypeface  = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistRegular_ttf, ArsNovaUIFonts::GeistRegular_ttfSize);
    semiBoldTypeface = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistSemiBold_ttf, ArsNovaUIFonts::GeistSemiBold_ttfSize);
    monoTypeface     = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistMonoRegular_ttf, ArsNovaUIFonts::GeistMonoRegular_ttfSize);

    pencilGraphics.addTransform(juce::AffineTransform::scale(cursorScale));
    pencilGraphics.setColour(windowColour);
    pencilGraphics.strokePath(pencilGlyph, cursorHalo);
    pencilGraphics.setColour(textColour);
    pencilGraphics.strokePath(pencilGlyph, cursorInk);

    eraserGraphics.addTransform(juce::AffineTransform::scale(cursorScale));
    eraserGraphics.setColour(windowColour);
    eraserGraphics.strokePath(eraserGlyph, cursorHalo);
    eraserGraphics.setColour(textColour);
    eraserGraphics.strokePath(eraserGlyph, cursorInk);

    pencilCursor = juce::MouseCursor(pencilImage, pencilHotSpotX, pencilHotSpotY, cursorScale);
    eraserCursor = juce::MouseCursor(eraserImage, eraserHotSpotX, eraserHotSpotY, cursorScale);

    applyThemeColour(accentColour);
}

void CustomLookAndFeel::applyThemeColour(juce::Colour themeColour)
{
    recolour(themeColour);

    setColour(juce::PopupMenu::backgroundColourId,            juce::Colours::transparentBlack);
    setColour(juce::PopupMenu::textColourId,                  popupMenuTextColour);
    setColour(juce::PopupMenu::headerTextColourId,            popupMenuTextColour);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, popupMenuHighlightColour);
    setColour(juce::PopupMenu::highlightedTextColourId,       popupMenuHighlightTextColour);
    setColour(juce::CaretComponent::caretColourId,            accentColour);
    setColour(juce::TextEditor::highlightColourId,            accentSoftColour);
    setColour(juce::TextEditor::highlightedTextColourId,      textColour);
    setColour(juce::TooltipWindow::backgroundColourId,        raisedColour);
    setColour(juce::TooltipWindow::textColourId,              textColour);
    setColour(juce::TooltipWindow::outlineColourId,           juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId,                      captionColour);
    setColour(juce::TextEditor::textColourId,                 textColour);
    setColour(juce::CodeEditorComponent::backgroundColourId,  surfaceColour);
    setColour(juce::CodeEditorComponent::defaultTextColourId, textColour);
    setColour(juce::CodeEditorComponent::highlightColourId,   accentColour.withAlpha(codeSelectionAlpha));
    setColour(juce::CodeEditorComponent::lineNumberTextId,    lineNumberColour);
}

void CustomLookAndFeel::drawPopupMenuBackgroundWithOptions(juce::Graphics& graphics, int width, int height,
                                                           const juce::PopupMenu::Options&)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    const auto box    = bounds.reduced(static_cast<float>(popupMenuShadowRadius));

    const juce::Colour shadowColour = dropShadowColour.withAlpha(callOutShadowAlpha);

    const juce::DropShadow shadow(shadowColour, popupMenuShadowRadius, {});

    juce::Path outline;

    outline.addRoundedRectangle(box, paneCornerRadius);

    shadow.drawForPath(graphics, outline);

    graphics.setColour(popupMenuColour);
    graphics.fillPath(outline);
}

void CustomLookAndFeel::drawResizableFrame(juce::Graphics&, int, int, const juce::BorderSize<int>&)
{
}

void CustomLookAndFeel::getIdealPopupMenuItemSize(const juce::String& text, bool isSeparator,
                                                  int standardMenuItemHeight,
                                                  int& idealWidth, int& idealHeight)
{
    int itemHeight = popupMenuItemHeight;

    if (standardMenuItemHeight > 0) {
        itemHeight = standardMenuItemHeight;
    }

    if (isSeparator) {
        idealWidth  = 50;
        idealHeight = separatorThickness;

        return;
    }

    const int textWidth = juce::GlyphArrangement::getStringWidthInt(getPopupMenuFont(), text)
                            + itemHeight
                            + static_cast<int>(popupMenuTextInset * 4.0f);

    idealHeight = itemHeight;
    idealWidth  = juce::jmax(textWidth, menuContentWidth);
}

void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& graphics, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                                          bool hasSubMenu, const juce::String& text,
                                          const juce::String& shortcutKeyText,
                                          const juce::Drawable* icon, const juce::Colour* textColourToUse)
{
    const float shadowWidth = static_cast<float>(popupMenuShadowRadius);

    const auto boxArea = area.toFloat().reduced(shadowWidth, 0.0f);

    if (isSeparator) {
        const auto line = boxArea.reduced(menuEdgeInset, 0.0f).withHeight(static_cast<float>(separatorThickness));

        graphics.setColour(popupMenuTextColour.withAlpha(0.25f));
        graphics.fillRect(line);
        return;
    }

    const auto itemBounds = boxArea.reduced(popupMenuItemInset, popupMenuItemGap);

    juce::Colour itemTextColour = popupMenuTextColour;

    if (textColourToUse != nullptr) {
        itemTextColour = *textColourToUse;
    }

    if (isHighlighted && isActive) {
        graphics.setColour(popupMenuHighlightColour);
        graphics.fillRoundedRectangle(itemBounds, paneCornerRadius);

        itemTextColour = popupMenuHighlightTextColour;
    }

    float textAlpha = 0.4f;

    if (isActive) {
        textAlpha = 1.0f;
    }

    graphics.setColour(itemTextColour.withMultipliedAlpha(textAlpha));
    graphics.setFont(getPopupMenuFont());

    auto textBounds = itemBounds.reduced(popupMenuTextInset, 0.0f);

    const auto gutter = textBounds.removeFromLeft(textBounds.getHeight());

    if (icon != nullptr) {
        icon->drawWithin(graphics, gutter, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
    }
    else if (isTicked) {
        const juce::Path tick = getTickShape(1.0f);
        graphics.fillPath(tick, tick.getTransformToScaleToFit(gutter.reduced(gutter.getWidth() / 4.0f), true));
    }

    if (hasSubMenu) {
        const float arrowHeight = 0.6f * getPopupMenuFont().getAscent();
        const float arrowX      = textBounds.removeFromRight(arrowHeight).getX();
        const float centreY     = textBounds.getCentreY();
        juce::Path  arrow;

        arrow.startNewSubPath(arrowX, centreY - arrowHeight * 0.5f);
        arrow.lineTo(arrowX + arrowHeight * 0.6f, centreY);
        arrow.lineTo(arrowX, centreY + arrowHeight * 0.5f);

        graphics.strokePath(arrow, juce::PathStrokeType(1.5f));
    }

    graphics.drawFittedText(text, textBounds.toNearestInt(), juce::Justification::centredLeft, 1);

    if (shortcutKeyText.isNotEmpty()) {
        graphics.setFont(font(FontStyle::Mono, labelFontHeight * 0.85f));
        graphics.drawText(shortcutKeyText, textBounds, juce::Justification::centredRight, true);
    }
}

void CustomLookAndFeel::drawCallOutBoxBackground(juce::CallOutBox& box, juce::Graphics& graphics, const juce::Path& path,
                                                 juce::Image& cachedShadow)
{
    if (cachedShadow.isNull()) {
        cachedShadow = juce::Image(juce::Image::ARGB, box.getWidth(), box.getHeight(), true);

        juce::Graphics shadowGraphics(cachedShadow);

        juce::DropShadow(dropShadowColour.withAlpha(callOutShadowAlpha), callOutShadowRadius, {}).drawForPath(shadowGraphics, path);
    }

    graphics.setColour(dropShadowColour);
    graphics.drawImageAt(cachedShadow, 0, 0);

    graphics.setColour(popupMenuColour);
    graphics.fillPath(path);

    graphics.setColour(popupMenuBorderColour);
    graphics.strokePath(path, juce::PathStrokeType(popupMenuBorderThickness));
}

int CustomLookAndFeel::getPopupMenuBorderSize()
{
    return popupMenuBorderSize;
}

juce::Font CustomLookAndFeel::getPopupMenuFont()
{
    return font(FontStyle::Regular, labelFontHeight);
}

}
