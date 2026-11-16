#include "CustomLookAndFeel.h"
#include "CustomTextCaret.h"

namespace ArsNovaUI
{

void CustomLookAndFeel::drawScrollbar(juce::Graphics& graphics, juce::ScrollBar&, int x, int y, int width, int height,
                                      bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                                      bool isMouseOver, bool isMouseDown)
{
    graphics.setColour(scrollBarTrackColour);
    graphics.fillRect(juce::Rectangle<int>(x, y, width, height));

    if (thumbSize <= 0) {
        return;
    }

    juce::Rectangle<int> thumb(thumbStartPosition, y, thumbSize, height);

    if (isScrollbarVertical) {
        thumb = juce::Rectangle<int>(x, thumbStartPosition, width, thumbSize);
    }

    graphics.setColour(scrollBarThumbColour);

    if (isMouseOver || isMouseDown) {
        graphics.setColour(scrollBarThumbHoverColour);
    }

    graphics.fillRoundedRectangle(thumb.toFloat().reduced(scrollBarThumbInset), scrollBarCornerRadius);
}

void CustomLookAndFeel::drawCanvas(juce::Graphics& graphics, const CanvasGrid& grid)
{
    const float                     spacing = grid.spacing;
    const auto                      bounds  = grid.bounds.transformedBy(grid.modelTransform);
    const auto                      visible = graphics.getClipBounds().toFloat().transformedBy(grid.modelTransform).expanded(gridDotDiameter).getIntersection(bounds);
    const auto                      dot     = juce::Rectangle<float>(gridDotDiameter, gridDotDiameter);
    float                           originX = bounds.getCentreX();
    float                           originY = bounds.getCentreY();
    juce::Graphics::ScopedSaveState savedState(graphics);

    graphics.fillAll(canvasColour);

    if (!grid.visible || spacing < 15.0f) {
        return;
    }

    graphics.addTransform(grid.viewTransform);

    if (grid.originSet) {
        originX = grid.origin.x;
        originY = grid.origin.y;
    }

    const float firstX = originX - std::ceil((originX - visible.getX()) / spacing) * spacing;
    const float firstY = originY - std::ceil((originY - visible.getY()) / spacing) * spacing;

    graphics.setColour(gridColour);

    for (float dotX = firstX; dotX <= visible.getRight(); dotX += spacing) {
        for (float dotY = firstY; dotY <= visible.getBottom(); dotY += spacing) {
            graphics.fillEllipse(dot.withCentre({ dotX, dotY }));
        }
    }
}

void CustomLookAndFeel::drawPane(juce::Graphics& graphics, juce::Rectangle<float> bounds)
{
    const auto area = bounds.reduced(borderThickness * 0.5f);

    graphics.setColour(surfaceColour);
    graphics.fillRoundedRectangle(area, groupCornerRadius);

    graphics.setColour(borderColour);
    graphics.drawRoundedRectangle(area, groupCornerRadius, borderThickness);
}

void CustomLookAndFeel::drawDialog(juce::Graphics& graphics, juce::Rectangle<float> area, juce::Rectangle<float> box)
{
    const juce::Colour shadowColour = dropShadowColour.withAlpha(callOutShadowAlpha);

    const juce::DropShadow shadow(shadowColour, callOutShadowRadius, {});

    juce::Path outline;

    outline.addRoundedRectangle(box, paneCornerRadius);

    graphics.setColour(dialogScrimColour);
    graphics.fillRect(area);

    shadow.drawForPath(graphics, outline);

    graphics.setColour(popupMenuColour);
    graphics.fillPath(outline);

    graphics.setColour(popupMenuBorderColour);
    graphics.strokePath(outline, juce::PathStrokeType(popupMenuBorderThickness));
}

void CustomLookAndFeel::drawFrostedGlass(juce::Graphics& graphics, juce::Component& surface, juce::Component& backdrop, juce::Rectangle<int> area)
{
    const int  margin         = juce::roundToInt(frostKernelSize / frostScale);
    const auto backdropArea   = backdrop.getLocalBounds().expanded(margin);
    const auto backdropOrigin = surface.getLocalPoint(&backdrop, backdropArea.getPosition()).toFloat();
    const auto frostedBounds  = (backdropArea.withZeroOrigin().toFloat() * frostScale).toNearestInt();

    juce::Graphics::ScopedSaveState savedState(graphics);

    if (! graphics.clipRegionIntersects(area)) {
        return;
    }

    if (frostedBackdrop.getBounds() != frostedBounds) {
        frostedBackdrop      = juce::Image(juce::Image::ARGB, frostedBounds.getWidth(), frostedBounds.getHeight(), true, juce::SoftwareImageType());
        frostedBackdropStale = true;
    }

    if (frostedBackdropStale) {
        juce::Graphics backdropGraphics(frostedBackdrop);

        frostedBackdrop.clear(frostedBackdrop.getBounds());

        backdropGraphics.addTransform(juce::AffineTransform::scale(frostScale));
        backdropGraphics.setOrigin(-backdropArea.getPosition());
        backdropGraphics.excludeClipRegion(unfrostedArea.reduced(margin));

        backdrop.paint(backdropGraphics);

        for (juce::Component* child : backdrop.getChildren()) {
            if (! child->isVisible() || ! backdropGraphics.clipRegionIntersects(child->getBoundsInParent())) {
                continue;
            }

            juce::Graphics::ScopedSaveState childState(backdropGraphics);

            backdropGraphics.addTransform(child->getTransform());
            backdropGraphics.setOrigin(child->getPosition());

            if (child->getAlpha() < 1.0f) {
                backdropGraphics.beginTransparencyLayer(child->getAlpha());
            }

            child->paint(backdropGraphics);

            if (child->getAlpha() < 1.0f) {
                backdropGraphics.endTransparencyLayer();
            }
        }

        blurFrostedImage(frostedBackdrop);

        repaintFrostChanges(backdrop, backdropArea.getPosition());

        frostedBackdropStale = false;
    }

    graphics.reduceClipRegion(area);
    graphics.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    graphics.drawImageTransformed(frostedBackdrop, juce::AffineTransform::scale(1.0f / frostScale).translated(backdropOrigin));

    graphics.setColour(barColour);
    graphics.fillRect(area);
}

void CustomLookAndFeel::blurFrostedImage(juce::Image& frosted)
{
    const int                          centreTap    = frostKernelSize / 2;
    const double                       radiusFactor = -1.0 / (frostBlurRadius * frostBlurRadius * 2.0);
    juce::Image::BitmapData            pixels(frosted, juce::Image::BitmapData::readWrite);
    const int                          rowLength    = pixels.width * pixels.pixelStride;
    std::vector<float>                 rowPass(static_cast<size_t>(rowLength * pixels.height), 0.0f);
    std::vector<float>                 columnPass(static_cast<size_t>(rowLength), 0.0f);
    std::array<float, frostKernelSize> weights {};
    float                              weightSum    = 0.0f;

    for (int tap = 0; tap < frostKernelSize; ++tap) {
        const int offset = tap - centreTap;

        weights[static_cast<size_t>(tap)]  = static_cast<float>(std::exp(radiusFactor * offset * offset));
        weightSum                         += weights[static_cast<size_t>(tap)];
    }

    for (float& weight : weights) {
        weight /= weightSum;
    }

    for (int y = 0; y < pixels.height; ++y) {
        for (int tap = 0; tap < frostKernelSize; ++tap) {
            const int                shift   = tap - centreTap;
            const int                firstX  = std::max(0, -shift);
            const int                lastX   = std::min(pixels.width, pixels.width - shift);
            const float              weight  = weights[static_cast<size_t>(tap)];
            const juce::uint8* const source  = pixels.getLinePointer(y) + (firstX + shift) * pixels.pixelStride;
            float* const             blurred = rowPass.data() + y * rowLength + firstX * pixels.pixelStride;

            for (int index = 0; index < (lastX - firstX) * pixels.pixelStride; ++index) {
                blurred[index] += weight * source[index];
            }
        }
    }

    for (int y = 0; y < pixels.height; ++y) {
        juce::uint8* const target = pixels.getLinePointer(y);

        std::fill(columnPass.begin(), columnPass.end(), 0.0f);

        for (int tap = 0; tap < frostKernelSize; ++tap) {
            const int sourceY = y + tap - centreTap;

            if (sourceY < 0 || sourceY >= pixels.height) {
                continue;
            }

            const float        weight = weights[static_cast<size_t>(tap)];
            const float* const source = rowPass.data() + sourceY * rowLength;

            for (int index = 0; index < rowLength; ++index) {
                columnPass[static_cast<size_t>(index)] += weight * source[index];
            }
        }

        for (int index = 0; index < rowLength; ++index) {
            target[index] = static_cast<juce::uint8>(juce::jmin(0xff, juce::roundToInt(columnPass[static_cast<size_t>(index)])));
        }
    }
}

void CustomLookAndFeel::repaintFrostChanges(juce::Component& backdrop, juce::Point<int> backdropOrigin)
{
    if (shownFrostedBackdrop.getBounds() != frostedBackdrop.getBounds()) {
        shownFrostedBackdrop = juce::Image(juce::Image::ARGB, frostedBackdrop.getWidth(), frostedBackdrop.getHeight(), true, juce::SoftwareImageType());
    }

    const juce::Image::BitmapData frosted(frostedBackdrop, juce::Image::BitmapData::readOnly);
    juce::Image::BitmapData       shown(shownFrostedBackdrop, juce::Image::BitmapData::readWrite);

    const int tileColumns = (frosted.width + frostTileSize - 1) / frostTileSize;
    const int tileRows    = (frosted.height + frostTileSize - 1) / frostTileSize;

    const size_t pixelBytes = static_cast<size_t>(frosted.pixelStride);
    const size_t rowBytes   = static_cast<size_t>(frosted.width) * pixelBytes;

    std::vector<bool> changedTiles(static_cast<size_t>(tileColumns * tileRows), false);

    for (int y = 0; y < frosted.height; ++y) {
        const juce::uint8* const source = frosted.getLinePointer(y);
        juce::uint8* const       target = shown.getLinePointer(y);

        for (int x = 0; x < frosted.width; ++x) {
            const size_t pixel = static_cast<size_t>(x) * pixelBytes;

            if (std::memcmp(source + pixel, target + pixel, pixelBytes) != 0) {
                changedTiles[static_cast<size_t>((y / frostTileSize) * tileColumns + x / frostTileSize)] = true;
            }
        }

        std::memcpy(target, source, rowBytes);
    }

    for (int row = 0; row < tileRows; ++row) {
        for (int column = 0; column < tileColumns; ++column) {
            int runEnd = column;

            while (runEnd < tileColumns && changedTiles[static_cast<size_t>(row * tileColumns + runEnd)]) {
                ++runEnd;
            }

            if (runEnd > column) {
                const juce::Rectangle<float> tiles(static_cast<float>(column * frostTileSize), static_cast<float>(row * frostTileSize),
                                                   static_cast<float>((runEnd - column) * frostTileSize), static_cast<float>(frostTileSize));

                backdrop.repaint((tiles / frostScale).getSmallestIntegerContainer() + backdropOrigin);

                column = runEnd;
            }
        }
    }
}

int CustomLookAndFeel::getDefaultScrollbarWidth()
{
    return scrollBarThickness;
}

juce::CaretComponent* CustomLookAndFeel::createCaretComponent(juce::Component* keyFocusOwner)
{
    auto caret = std::make_unique<CustomTextCaret>(keyFocusOwner);
    caret->caretWidth = 1.0f;
    return caret.release();
}

}
