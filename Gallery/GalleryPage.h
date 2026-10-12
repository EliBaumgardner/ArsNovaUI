#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ArsNovaUI/Bars/Bar.h>
#include <ArsNovaUI/Buttons/IconButton.h>
#include <ArsNovaUI/Canvas/DynamicPort.h>
#include <ArsNovaUI/Editors/ValueEditor.h>
#include <ArsNovaUI/Menus/ColourSelector.h>
#include <ArsNovaUI/Menus/ContextMenu.h>
#include <ArsNovaUI/Menus/ItemSelector.h>
#include <ArsNovaUI/Theme/InterfaceIdentifiers.h>
#include "GalleryCanvas.h"

class GalleryPage : public juce::Component
{
public:

    GalleryPage();

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    enum class Sample { Normal, Hovered, Selected, Disabled };

    static constexpr int   sampleCount   = 4;
    static constexpr int   editorCount   = 5;
    static constexpr int   defaultWidth  = 1100;
    static constexpr int   defaultHeight = 760;
    static constexpr int   iconCell      = 28;
    static constexpr int   iconGap       = 4;
    static constexpr int   margin        = 16;
    static constexpr int   editorWidth   = 72;
    static constexpr int   editorHeight  = 22;
    static constexpr int   selectorWidth = 150;
    static constexpr int   controlHeight = 28;
    static constexpr int   barHeight     = 44;
    static constexpr float barInsetRatio = 0.2f;

private:

    juce::UndoManager undoManager;
    juce::ValueTree   values        { "GalleryValues" };
    juce::ValueTree   colourPresets { InterfaceIdentifiers::ColourPresets };

    juce::OwnedArray<IconButton> iconButtons;

    ValueEditor numberEditor;
    ValueEditor wordEditor;
    ValueEditor pitchEditor;
    ValueEditor letterEditor;
    ValueEditor pairEditor;

    ItemSelector   itemSelector;
    ColourSelector colourSelector;
    IconButton     menuButton;

    std::unique_ptr<ContextMenu> menu;

    GalleryCanvas canvas;
    DynamicPort   port;
    Bar           frostedBar;
};
