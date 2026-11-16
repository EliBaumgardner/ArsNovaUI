#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Editors/CaptionLabel.h"
#include "../Editors/ValueEditor.h"

#include <memory>
#include <vector>

namespace ArsNovaUI
{

class CustomLookAndFeel;
class ScaledView;

class SettingsMenu : public juce::Component
{
public:

    SettingsMenu(CustomLookAndFeel& lookAndFeel, ScaledView& scaledView, juce::ValueTree colourPresets, juce::UndoManager& undoManager);

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    void addRow(const juce::String& caption, std::unique_ptr<juce::Component> control, float controlWidthRatio);

    ValueEditor& addEditorRow(const juce::String& caption, std::unique_ptr<ValueFormat> format);

    juce::Point<int> narrowestSize() const;

    struct Row
    {
        CaptionLabel caption;

        std::unique_ptr<juce::Component> control;

        float controlWidthRatio = 1.0f;
    };

    static constexpr int horizontalPaddingCount = 3;

    static constexpr float selectorWidthRatio = 1.6f;
    static constexpr float editorWidthRatio   = 3.0f;
    static constexpr float selectorInsetRatio = 0.15f;

    static constexpr juce::Point<float> openingScale { 1.3f, 1.3f };

private:

    CustomLookAndFeel& lookAndFeel;
    ScaledView&        scaledView;
    juce::UndoManager& undoManager;

    std::vector<std::unique_ptr<Row>> rows;
};

}
