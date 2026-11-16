#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Icon.h"
#include "Theme.h"
#include "Visuals.h"

namespace ArsNovaUI
{

class FileLabel;

class IconButton;

class ValueSlider;

struct ButtonState;

class CustomLookAndFeel : public juce::LookAndFeel_V4, public Theme
{
public:

    CustomLookAndFeel();

    void applyThemeColour(juce::Colour themeColour);

    static CustomLookAndFeel& get(const juce::Component& component)
    {
        return static_cast<CustomLookAndFeel&>(component.getLookAndFeel());
    }

    void drawPopupMenuBackgroundWithOptions(juce::Graphics& graphics, int width, int height, const juce::PopupMenu::Options& options) override;
    void getIdealPopupMenuItemSize         (const juce::String& text, bool isSeparator, int standardMenuItemHeight, int& idealWidth, int& idealHeight) override;

    void drawPopupMenuItem(juce::Graphics& graphics, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                           bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon,
                           const juce::Colour* textColourToUse) override;

    void drawScrollbar(juce::Graphics& graphics, juce::ScrollBar& scrollBar, int x, int y, int width, int height, bool isScrollbarVertical,
                       int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;

    void drawCallOutBoxBackground(juce::CallOutBox& box, juce::Graphics& graphics, const juce::Path& path, juce::Image& cachedShadow) override;

    void drawCanvas      (juce::Graphics& graphics, const CanvasGrid& grid);
    void drawPane        (juce::Graphics& graphics, juce::Rectangle<float> bounds);
    void drawDialog      (juce::Graphics& graphics, juce::Rectangle<float> area, juce::Rectangle<float> box);

    void drawFrostedGlass   (juce::Graphics& graphics, juce::Component& surface, juce::Component& backdrop, juce::Rectangle<int> area);
    void blurFrostedImage   (juce::Image& frosted);
    void repaintFrostChanges(juce::Component& backdrop, juce::Point<int> backdropOrigin);

    void drawCircleNode (juce::Graphics& graphics, const NodeVisual& visual);
    void drawSquareNode (juce::Graphics& graphics, const NodeVisual& visual);
    void drawRaisedPanel(juce::Graphics& graphics, juce::Rectangle<float> bounds);
    void drawArrow      (juce::Graphics& graphics, const ArrowVisual& visual);

    void drawIcon(juce::Graphics& graphics, juce::Rectangle<float> bounds, const Icon& icon, const ButtonState& state);

    void drawArrowButton(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state);

    void drawStrokeSymbol(juce::Graphics& graphics, juce::Rectangle<float> bounds, const std::vector<SymbolStroke>& strokes);
    void drawSymbolMark  (juce::Graphics& graphics, juce::Rectangle<float> bounds, SymbolMark mark, const std::vector<SymbolStroke>& strokes);

    void drawTextButton(juce::Graphics& graphics, const IconButton& button);
    void drawCodeButton(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state, float fontHeight = labelFontHeight);

    void drawIncrementIcon(juce::Graphics& graphics, juce::Rectangle<float> bounds, bool pointsUp);

    void drawValueSlider(juce::Graphics& graphics, const ValueSlider& slider);
    void drawFileLabel  (juce::Graphics& graphics, const FileLabel& fileLabel);

    int getPopupMenuBorderSize  () override;
    int getDefaultScrollbarWidth() override;

    juce::Font getPopupMenuFont() override;

    juce::CaretComponent* createCaretComponent(juce::Component* keyFocusOwner) override;

    juce::Rectangle<float> drawButtonTile(juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state);

    juce::Colour pressableButtonColour(const ButtonState& state) const;

    static juce::Rectangle<float> getNodeCircleBounds(juce::Rectangle<float> componentBounds);

    juce::Rectangle<int> unfrostedArea;
    juce::MouseCursor    pencilCursor;
    juce::MouseCursor    eraserCursor;
    bool                 frostedBackdropStale = true;

private:


    juce::Image frostedBackdrop;
    juce::Image shownFrostedBackdrop;
};

}
