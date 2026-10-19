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
    juce::ValueTree   colourPresets { ArsNovaUI::InterfaceIdentifiers::ColourPresets };

    juce::OwnedArray<ArsNovaUI::IconButton> iconButtons;

    ArsNovaUI::ValueEditor numberEditor;
    ArsNovaUI::ValueEditor wordEditor;
    ArsNovaUI::ValueEditor pitchEditor;
    ArsNovaUI::ValueEditor letterEditor;
    ArsNovaUI::ValueEditor pairEditor;

    ArsNovaUI::ItemSelector   itemSelector;
    ArsNovaUI::ColourSelector colourSelector;
    ArsNovaUI::IconButton     menuButton;

    std::unique_ptr<ArsNovaUI::ContextMenu> menu;

    GalleryCanvas          canvas;
    ArsNovaUI::DynamicPort port;
    ArsNovaUI::Bar         frostedBar;
};
