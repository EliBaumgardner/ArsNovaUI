#include "GalleryPage.h"
#include <ArsNovaUI/Theme/CustomLookAndFeel.h>

#include <array>

GalleryPage::GalleryPage()
    : numberEditor(undoManager), wordEditor(undoManager), pitchEditor(undoManager), letterEditor(undoManager), pairEditor(undoManager),
      itemSelector(undoManager), colourSelector(colourPresets, undoManager), port(canvas), frostedBar(canvas, Bar::Orientation::Horizontal, barInsetRatio)
{
    const std::vector<IconButton::Icon> icons {
        &CustomLookAndFeel::drawForkIcon, &CustomLookAndFeel::drawRingDotIcon, &CustomLookAndFeel::drawRouteIcon, &CustomLookAndFeel::drawSettingsIcon,
        &CustomLookAndFeel::drawPlayIcon, &CustomLookAndFeel::drawLinkIcon, &CustomLookAndFeel::drawMetronomeIcon, &CustomLookAndFeel::drawSkipBackIcon,
        &CustomLookAndFeel::drawReloadIcon, &CustomLookAndFeel::drawUndoIcon, &CustomLookAndFeel::drawRedoIcon, &CustomLookAndFeel::drawCircleIcon,
        &CustomLookAndFeel::drawRoundedSquareIcon, &CustomLookAndFeel::drawTriangleIcon, &CustomLookAndFeel::drawChevronDownIcon,
        &CustomLookAndFeel::drawChevronUpIcon, &CustomLookAndFeel::drawEyeIcon, &CustomLookAndFeel::drawAddIcon, &CustomLookAndFeel::drawRemoveIcon,
        &CustomLookAndFeel::drawImportIcon, &CustomLookAndFeel::drawBrushIcon, &CustomLookAndFeel::drawDashedBoxIcon, &CustomLookAndFeel::drawQuaverIcon,
        &CustomLookAndFeel::drawCircledHButton, &CustomLookAndFeel::drawArrowButton, &CustomLookAndFeel::drawPencilButton,
        &CustomLookAndFeel::drawUploadButton, &CustomLookAndFeel::drawWaveButton, &CustomLookAndFeel::drawPencilIcon, &CustomLookAndFeel::drawEraserIcon
    };

    const std::array<Sample, sampleCount>       samples { Sample::Normal, Sample::Hovered, Sample::Selected, Sample::Disabled };
    const std::array<ValueEditor*, editorCount> editors { &numberEditor, &wordEditor, &pitchEditor, &letterEditor, &pairEditor };

    values.setProperty("Number", 64, nullptr);
    values.setProperty("Word", "gallery", nullptr);
    values.setProperty("Pitch", 60, nullptr);
    values.setProperty("Letter", 3, nullptr);
    values.setProperty("Pair", 4, nullptr);
    values.setProperty("PairSecond", 2, nullptr);

    for (Sample sample : samples) {
        const int rowStart = iconButtons.size();

        for (IconButton::Icon icon : icons) {
            iconButtons.add(std::make_unique<IconButton>())->icon = icon;
        }

        IconButton* const textButton = iconButtons.add(std::make_unique<IconButton>());

        textButton->setText("Aa");

        textButton->painter = [this](juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state) {
            CustomLookAndFeel::get(*this).drawTextButton(graphics, bounds, state);
        };

        iconButtons.add(std::make_unique<IconButton>())->painter = [this](juce::Graphics& graphics, juce::Rectangle<float> bounds, const ButtonState& state) {
            CustomLookAndFeel::get(*this).drawCodeButton(graphics, bounds, state);
        };

        for (int index = rowStart; index < iconButtons.size(); ++index) {
            IconButton& button = *iconButtons[index];

            button.setSelected(sample == Sample::Selected);
            button.setEnabled(sample != Sample::Disabled);

            button.state.isHovered = sample == Sample::Hovered;

            addAndMakeVisible(button);
        }
    }

    numberEditor.setFormat(std::make_unique<NumberFormat>(0, 127));
    wordEditor.setFormat(std::make_unique<TextFormat>(12, "abcdefghijklmnopqrstuvwxyz"));
    pitchEditor.setFormat(std::make_unique<PitchFormat>());
    letterEditor.setFormat(std::make_unique<GreekLetterFormat>());
    pairEditor.setFormat(std::make_unique<DualIntFormat>(1, 99, "PairSecond"));

    numberEditor.bindEditor(values, "Number");
    wordEditor.bindEditor(values, "Word");
    pitchEditor.bindEditor(values, "Pitch");
    letterEditor.bindEditor(values, "Letter");
    pairEditor.bindEditor(values, "Pair");

    for (ValueEditor* editor : editors) {
        editor->backdrop = ValueEditor::Backdrop::Field;

        addAndMakeVisible(editor);
    }

    itemSelector.addItem(1, "First item");
    itemSelector.addItem(2, "Second item");
    itemSelector.addItem(3, "Disabled item", nullptr, false);
    itemSelector.setSelectedItem(1);

    colourSelector.colour  = juce::Colour::fromRGB(96, 150, 220);
    menuButton.icon        = &CustomLookAndFeel::drawSettingsIcon;
    port.initialViewCentre = { 340.0f, 260.0f };

    menuButton.onClick = [this]() {
        menu = std::make_unique<ContextMenu>();

        menu->addItem("Show grid", ContextMenu::ItemKind::Toggle, [this]() {
            canvas.grid.visible = ! canvas.grid.visible;

            CustomLookAndFeel::get(*this).frostedBackdropStale = true;

            canvas.repaint();
            frostedBar.repaint();
        }, true, canvas.grid.visible);

        menu->addItem("Reset zoom", ContextMenu::ItemKind::Action, [this]() { port.setZoom(1.0f, port.getLocalBounds().getCentre().toFloat()); });
        menu->addItem("Disabled action", ContextMenu::ItemKind::Action, nullptr, false);

        menu->show(menuButton);
    };

    port.onTransformChanged = [this](const juce::AffineTransform& viewTransform, float) {
        canvas.grid.viewTransform  = viewTransform;
        canvas.grid.modelTransform = viewTransform.inverted();

        CustomLookAndFeel::get(*this).frostedBackdropStale = true;

        frostedBar.repaint();
    };

    addAndMakeVisible(itemSelector);
    addAndMakeVisible(colourSelector);
    addAndMakeVisible(menuButton);
    addAndMakeVisible(port);
    addAndMakeVisible(frostedBar);
}

void GalleryPage::paint(juce::Graphics& graphics)
{
    graphics.fillAll(CustomLookAndFeel::get(*this).windowColour);
}

void GalleryPage::resized()
{
    const int                                            iconsPerRow = iconButtons.size() / sampleCount;
    const std::array<juce::Component*, editorCount>      editors { &numberEditor, &wordEditor, &pitchEditor, &letterEditor, &pairEditor };
    const std::array<std::pair<juce::Component*, int>, 3> pickers { { { &itemSelector, selectorWidth }, { &colourSelector, controlHeight }, { &menuButton, controlHeight } } };
    juce::Rectangle<int>                                 area        = getLocalBounds().reduced(margin);
    juce::Rectangle<int>                                 controls;

    for (int index = 0; index < iconButtons.size(); ++index) {
        const int column = index % iconsPerRow;
        const int row    = index / iconsPerRow;

        iconButtons[index]->setBounds(area.getX() + column * (iconCell + iconGap), area.getY() + row * (iconCell + iconGap), iconCell, iconCell);
    }

    area.removeFromTop(sampleCount * (iconCell + iconGap) + margin);

    controls = area.removeFromTop(controlHeight);

    for (juce::Component* editor : editors) {
        editor->setBounds(controls.removeFromLeft(editorWidth).withSizeKeepingCentre(editorWidth, editorHeight));

        controls.removeFromLeft(margin);
    }

    for (const auto& [picker, width] : pickers) {
        picker->setBounds(controls.removeFromLeft(width));

        controls.removeFromLeft(margin);
    }

    area.removeFromTop(margin);

    port.setBounds(area);
    frostedBar.setBounds(area.removeFromTop(barHeight));
}
