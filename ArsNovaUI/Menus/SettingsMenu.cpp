#include "SettingsMenu.h"
#include "../Theme/CustomLookAndFeel.h"
#include "../Theme/InterfaceIdentifiers.h"
#include "../ScaledView.h"
#include "ColourSelector.h"

namespace ArsNovaUI
{

SettingsMenu::SettingsMenu(CustomLookAndFeel& lookAndFeel, ScaledView& scaledView, juce::ValueTree colourPresets, juce::UndoManager& undoManager)
    : lookAndFeel (lookAndFeel),
      scaledView  (scaledView),
      undoManager (undoManager)
{
    auto themeColourSelector = std::make_unique<ColourSelector>(colourPresets, undoManager);
    auto zoomFormat          = std::make_unique<NumberFormat>(ScaledView::minimumZoom, ScaledView::maximumZoom, ValueFormat::editableDecimalPlaces);

    zoomFormat->suffix = "x";

    themeColourSelector->colour = lookAndFeel.accentColour;

    themeColourSelector->onColourPicked = [this](juce::Colour pickedColour) {
        juce::Desktop& desktop = juce::Desktop::getInstance();

        this->lookAndFeel.applyThemeColour(pickedColour);

        this->scaledView.interfaceSettings.setValue(InterfaceIdentifiers::ThemeColour.toString(), pickedColour.toString());

        for (int index = 0; index < desktop.getNumComponents(); ++index) {
            desktop.getComponent(index)->sendLookAndFeelChange();
        }
    };

    setLookAndFeel(&lookAndFeel);

    addRow("Theme colour", std::move(themeColourSelector), selectorWidthRatio);

    ValueEditor& zoomEditor = addEditorRow("Interface zoom", std::move(zoomFormat));

    zoomEditor.boundValue = static_cast<double>(scaledView.zoom);

    zoomEditor.onEditFinished = [this, &zoomEditor]() {
        const double enteredZoom = zoomEditor.boundValue.getValue();

        if (! this->scaledView.zoomFits(static_cast<float>(enteredZoom))) {
            zoomEditor.boundValue = static_cast<double>(this->scaledView.zoom);
            return;
        }

        this->scaledView.interfaceSettings.setValue(InterfaceIdentifiers::InterfaceZoom.toString(), enteredZoom);
    };
}

void SettingsMenu::paint(juce::Graphics& graphics)
{
    const Theme& theme        = CustomLookAndFeel::get(*this);
    const int    padding      = juce::roundToInt(Theme::barHeight * Theme::menuSpacingRatio);
    const int    dividerCount = static_cast<int>(rows.size()) - 1;

    graphics.fillAll(theme.surfaceColour);

    graphics.setColour(theme.borderColour);

    for (int index = 0; index < dividerCount; ++index) {
        const juce::Rectangle<int> divider(padding, rows[static_cast<size_t>(index)]->caption.getBottom(), getWidth() - padding * 2, 1);

        graphics.fillRect(divider);
    }
}

void SettingsMenu::resized()
{
    const int padding   = juce::roundToInt(Theme::barHeight * Theme::menuSpacingRatio);
    const int rowHeight = juce::roundToInt(Theme::barHeight * Theme::menuRowHeightRatio);
    const int rowInset  = juce::roundToInt(rowHeight * selectorInsetRatio);

    juce::Rectangle<int> area = getLocalBounds().reduced(padding);

    for (const std::unique_ptr<Row>& row : rows) {
        const int controlWidth = juce::roundToInt(rowHeight * row->controlWidthRatio);

        juce::Rectangle<int> rowArea     = area.removeFromTop(rowHeight);
        juce::Rectangle<int> controlArea = rowArea.removeFromRight(controlWidth);

        row->caption.setBounds(rowArea);
        row->control->setBounds(controlArea.reduced(0, rowInset));
    }
}

void SettingsMenu::addRow(const juce::String& caption, std::unique_ptr<juce::Component> control, float controlWidthRatio)
{
    std::unique_ptr<Row> row = std::make_unique<Row>();

    row->control           = std::move(control);
    row->controlWidthRatio = controlWidthRatio;

    row->caption.setText(caption, juce::dontSendNotification);

    addAndMakeVisible(row->caption);
    addAndMakeVisible(*row->control);

    rows.push_back(std::move(row));
}

ValueEditor& SettingsMenu::addEditorRow(const juce::String& caption, std::unique_ptr<ValueFormat> format)
{
    std::unique_ptr<ValueEditor> editor      = std::make_unique<ValueEditor>(undoManager);
    ValueEditor&                 addedEditor = *editor;

    editor->backdrop = ValueEditor::Backdrop::Field;

    editor->setFormat(std::move(format));
    editor->setFontHeight(Theme::textHeight);

    addRow(caption, std::move(editor), editorWidthRatio);

    return addedEditor;
}

juce::Point<int> SettingsMenu::narrowestSize() const
{
    const juce::Font textFont  = lookAndFeel.font(Theme::FontStyle::Regular, Theme::textHeight);
    const int        padding   = juce::roundToInt(Theme::barHeight * Theme::menuSpacingRatio);
    const int        rowHeight = juce::roundToInt(Theme::barHeight * Theme::menuRowHeightRatio);
    const int        rowCount  = static_cast<int>(rows.size());

    int widestRow = 0;

    for (const std::unique_ptr<Row>& row : rows) {
        const int captionWidth = juce::GlyphArrangement::getStringWidthInt(textFont, row->caption.getText());
        const int controlWidth = juce::roundToInt(rowHeight * row->controlWidthRatio);

        widestRow = juce::jmax(widestRow, captionWidth + controlWidth);
    }

    const int contentWidth  = padding * horizontalPaddingCount + widestRow;
    const int contentHeight = padding * 2 + rowHeight * rowCount;

    return { contentWidth, contentHeight };
}

}
