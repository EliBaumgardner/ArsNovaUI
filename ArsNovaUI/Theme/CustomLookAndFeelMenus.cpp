#include "CustomLookAndFeel.h"
#include "ArsNovaUIFonts.h"
#include "ArsNovaUIIcons.h"

#include <juce_gui_extra/juce_gui_extra.h>

static juce::Path loadGlyph(const char* svgData, int svgSize)
{
    const std::unique_ptr<juce::Drawable> svg = juce::Drawable::createFromImageData(svgData, static_cast<size_t>(svgSize));
    juce::Path                            glyph;

    for (juce::Component* shape : svg->getChildren()) {
        glyph.addPath(static_cast<juce::DrawablePath*>(shape)->getPath());
    }

    return glyph;
}

CustomLookAndFeel::CustomLookAndFeel()
{
    const int                  cursorPixels = static_cast<int>(glyphGridSize * cursorScale);
    const juce::PathStrokeType cursorInk(glyphGridSize * glyphStrokeRatio, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    const juce::PathStrokeType cursorHalo(cursorHaloWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Image                pencilImage(juce::Image::ARGB, cursorPixels, cursorPixels, true, juce::SoftwareImageType());
    juce::Image                eraserImage(juce::Image::ARGB, cursorPixels, cursorPixels, true, juce::SoftwareImageType());
    juce::Graphics             pencilGraphics(pencilImage);
    juce::Graphics             eraserGraphics(eraserImage);

    regularTypeface  = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistRegular_ttf, ArsNovaUIFonts::GeistRegular_ttfSize);
    semiBoldTypeface = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistSemiBold_ttf, ArsNovaUIFonts::GeistSemiBold_ttfSize);
    monoTypeface     = juce::Typeface::createSystemTypefaceFor(ArsNovaUIFonts::GeistMonoRegular_ttf, ArsNovaUIFonts::GeistMonoRegular_ttfSize);

    glyphs.add           = loadGlyph(ArsNovaUIIcons::Add_svg, ArsNovaUIIcons::Add_svgSize);
    glyphs.arrow         = loadGlyph(ArsNovaUIIcons::Arrow_svg, ArsNovaUIIcons::Arrow_svgSize);
    glyphs.boxDash       = loadGlyph(ArsNovaUIIcons::BoxDash_svg, ArsNovaUIIcons::BoxDash_svgSize);
    glyphs.boxDots       = loadGlyph(ArsNovaUIIcons::BoxDots_svg, ArsNovaUIIcons::BoxDots_svgSize);
    glyphs.brushHandle   = loadGlyph(ArsNovaUIIcons::BrushHandle_svg, ArsNovaUIIcons::BrushHandle_svgSize);
    glyphs.brushTip      = loadGlyph(ArsNovaUIIcons::BrushTip_svg, ArsNovaUIIcons::BrushTip_svgSize);
    glyphs.chevronDown   = loadGlyph(ArsNovaUIIcons::ChevronDown_svg, ArsNovaUIIcons::ChevronDown_svgSize);
    glyphs.chevronUp     = loadGlyph(ArsNovaUIIcons::ChevronUp_svg, ArsNovaUIIcons::ChevronUp_svgSize);
    glyphs.circle        = loadGlyph(ArsNovaUIIcons::Circle_svg, ArsNovaUIIcons::Circle_svgSize);
    glyphs.circledH      = loadGlyph(ArsNovaUIIcons::CircledH_svg, ArsNovaUIIcons::CircledH_svgSize);
    glyphs.code          = loadGlyph(ArsNovaUIIcons::Code_svg, ArsNovaUIIcons::Code_svgSize);
    glyphs.dashedBox     = loadGlyph(ArsNovaUIIcons::DashedBox_svg, ArsNovaUIIcons::DashedBox_svgSize);
    glyphs.dot           = loadGlyph(ArsNovaUIIcons::Dot_svg, ArsNovaUIIcons::Dot_svgSize);
    glyphs.eraser        = loadGlyph(ArsNovaUIIcons::Eraser_svg, ArsNovaUIIcons::Eraser_svgSize);
    glyphs.eye           = loadGlyph(ArsNovaUIIcons::Eye_svg, ArsNovaUIIcons::Eye_svgSize);
    glyphs.fork          = loadGlyph(ArsNovaUIIcons::Fork_svg, ArsNovaUIIcons::Fork_svgSize);
    glyphs.import        = loadGlyph(ArsNovaUIIcons::Import_svg, ArsNovaUIIcons::Import_svgSize);
    glyphs.link          = loadGlyph(ArsNovaUIIcons::Link_svg, ArsNovaUIIcons::Link_svgSize);
    glyphs.metronome     = loadGlyph(ArsNovaUIIcons::Metronome_svg, ArsNovaUIIcons::Metronome_svgSize);
    glyphs.pause         = loadGlyph(ArsNovaUIIcons::Pause_svg, ArsNovaUIIcons::Pause_svgSize);
    glyphs.pencil        = loadGlyph(ArsNovaUIIcons::Pencil_svg, ArsNovaUIIcons::Pencil_svgSize);
    glyphs.play          = loadGlyph(ArsNovaUIIcons::Play_svg, ArsNovaUIIcons::Play_svgSize);
    glyphs.quaverHead    = loadGlyph(ArsNovaUIIcons::QuaverHead_svg, ArsNovaUIIcons::QuaverHead_svgSize);
    glyphs.quaverStem    = loadGlyph(ArsNovaUIIcons::QuaverStem_svg, ArsNovaUIIcons::QuaverStem_svgSize);
    glyphs.redo          = loadGlyph(ArsNovaUIIcons::Redo_svg, ArsNovaUIIcons::Redo_svgSize);
    glyphs.reload        = loadGlyph(ArsNovaUIIcons::Reload_svg, ArsNovaUIIcons::Reload_svgSize);
    glyphs.remove        = loadGlyph(ArsNovaUIIcons::Remove_svg, ArsNovaUIIcons::Remove_svgSize);
    glyphs.ring          = loadGlyph(ArsNovaUIIcons::Ring_svg, ArsNovaUIIcons::Ring_svgSize);
    glyphs.roundedSquare = loadGlyph(ArsNovaUIIcons::RoundedSquare_svg, ArsNovaUIIcons::RoundedSquare_svgSize);
    glyphs.route         = loadGlyph(ArsNovaUIIcons::Route_svg, ArsNovaUIIcons::Route_svgSize);
    glyphs.settings      = loadGlyph(ArsNovaUIIcons::Settings_svg, ArsNovaUIIcons::Settings_svgSize);
    glyphs.skipBar       = loadGlyph(ArsNovaUIIcons::SkipBar_svg, ArsNovaUIIcons::SkipBar_svgSize);
    glyphs.skipTriangle  = loadGlyph(ArsNovaUIIcons::SkipTriangle_svg, ArsNovaUIIcons::SkipTriangle_svgSize);
    glyphs.triangle      = loadGlyph(ArsNovaUIIcons::Triangle_svg, ArsNovaUIIcons::Triangle_svgSize);
    glyphs.undo          = loadGlyph(ArsNovaUIIcons::Undo_svg, ArsNovaUIIcons::Undo_svgSize);
    glyphs.upload        = loadGlyph(ArsNovaUIIcons::Upload_svg, ArsNovaUIIcons::Upload_svgSize);
    glyphs.wave          = loadGlyph(ArsNovaUIIcons::Wave_svg, ArsNovaUIIcons::Wave_svgSize);

    pencilGraphics.addTransform(juce::AffineTransform::scale(cursorScale));
    pencilGraphics.setColour(windowColour);
    pencilGraphics.strokePath(glyphs.pencil, cursorHalo);
    pencilGraphics.setColour(textColour);
    pencilGraphics.strokePath(glyphs.pencil, cursorInk);

    eraserGraphics.addTransform(juce::AffineTransform::scale(cursorScale));
    eraserGraphics.setColour(windowColour);
    eraserGraphics.strokePath(glyphs.eraser, cursorHalo);
    eraserGraphics.setColour(textColour);
    eraserGraphics.strokePath(glyphs.eraser, cursorInk);

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

    graphics.setColour(popupMenuColour);
    graphics.fillRoundedRectangle(bounds, paneCornerRadius);
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
        idealHeight = itemHeight / 2;
        return;
    }

    idealHeight = itemHeight;
    idealWidth  = juce::GlyphArrangement::getStringWidthInt(getPopupMenuFont(), text)
                    + idealHeight
                    + static_cast<int>(popupMenuTextInset * 4.0f);
}

void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& graphics, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                                          bool hasSubMenu, const juce::String& text,
                                          const juce::String& shortcutKeyText,
                                          const juce::Drawable* icon, const juce::Colour* textColourToUse)
{
    if (isSeparator) {
        auto line = area.toFloat().reduced(menuEdgeInset, 0.0f).withHeight(1.0f);
        line.setY(area.toFloat().getCentreY());

        graphics.setColour(popupMenuTextColour.withAlpha(0.25f));
        graphics.fillRect(line);
        return;
    }

    const auto itemBounds = area.toFloat().reduced(popupMenuItemInset, popupMenuItemGap);

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
    return popupMenuPadding;
}

juce::Font CustomLookAndFeel::getPopupMenuFont()
{
    return font(FontStyle::Regular, labelFontHeight);
}
