#include "GalleryPage.h"
#include <ArsNovaUI/Theme/CustomLookAndFeel.h>

#include <array>

GalleryPage::GalleryPage()
    : numberEditor(undoManager), wordEditor(undoManager), pitchEditor(undoManager), letterEditor(undoManager), pairEditor(undoManager),
      itemSelector(undoManager), colourSelector(colourPresets, undoManager), port(canvas), frostedBar(canvas, ArsNovaUI::Bar::Orientation::Horizontal, barInsetRatio)
{
    const std::vector<ArsNovaUI::IconButton::Icon> icons {
        &ArsNovaUI::CustomLookAndFeel::drawForkIcon, &ArsNovaUI::CustomLookAndFeel::drawRingDotIcon, &ArsNovaUI::CustomLookAndFeel::drawRouteIcon,
        &ArsNovaUI::CustomLookAndFeel::drawSettingsIcon, &ArsNovaUI::CustomLookAndFeel::drawPlayIcon, &ArsNovaUI::CustomLookAndFeel::drawLinkIcon,
        &ArsNovaUI::CustomLookAndFeel::drawMetronomeIcon, &ArsNovaUI::CustomLookAndFeel::drawSkipBackIcon, &ArsNovaUI::CustomLookAndFeel::drawReloadIcon,
        &ArsNovaUI::CustomLookAndFeel::drawUndoIcon, &ArsNovaUI::CustomLookAndFeel::drawRedoIcon, &ArsNovaUI::CustomLookAndFeel::drawCircleIcon,
        &ArsNovaUI::CustomLookAndFeel::drawRoundedSquareIcon, &ArsNovaUI::CustomLookAndFeel::drawTriangleIcon,
        &ArsNovaUI::CustomLookAndFeel::drawChevronDownIcon, &ArsNovaUI::CustomLookAndFeel::drawChevronUpIcon, &ArsNovaUI::CustomLookAndFeel::drawEyeIcon,
        &ArsNovaUI::CustomLookAndFeel::drawAddIcon, &ArsNovaUI::CustomLookAndFeel::drawRemoveIcon, &ArsNovaUI::CustomLookAndFeel::drawImportIcon,
        &ArsNovaUI::CustomLookAndFeel::drawBrushIcon, &ArsNovaUI::CustomLookAndFeel::drawDashedBoxIcon, &ArsNovaUI::CustomLookAndFeel::drawQuaverIcon,
        &ArsNovaUI::CustomLookAndFeel::drawCircledHButton, &ArsNovaUI::CustomLookAndFeel::drawArrowButton, &ArsNovaUI::CustomLookAndFeel::drawPencilButton,
        &ArsNovaUI::CustomLookAndFeel::drawUploadButton, &ArsNovaUI::CustomLookAndFeel::drawWaveButton, &ArsNovaUI::CustomLookAndFeel::drawPencilIcon,
        &ArsNovaUI::CustomLookAndFeel::drawEraserIcon
    };

    const std::array<Sample, sampleCount>                  samples { Sample::Normal, Sample::Hovered, Sample::Selected, Sample::Disabled };
    const std::array<ArsNovaUI::ValueEditor*, editorCount> editors { &numberEditor, &wordEditor, &pitchEditor, &letterEditor, &pairEditor };

    values.setProperty("Number", 64, nullptr);
    values.setProperty("Word", "gallery", nullptr);
    values.setProperty("Pitch", 60, nullptr);
    values.setProperty("Letter", 3, nullptr);
    values.setProperty("Pair", 4, nullptr);
    values.setProperty("PairSecond", 2, nullptr);

    for (Sample sample : samples) {
        const int rowStart = iconButtons.size();

        for (ArsNovaUI::IconButton::Icon icon : icons) {
            iconButtons.add(std::make_unique<ArsNovaUI::IconButton>())->icon = icon;
        }

        ArsNovaUI::IconButton* const textButton = iconButtons.add(std::make_unique<ArsNovaUI::IconButton>());

        textButton->setText("Aa");

        iconButtons.add(std::make_unique<ArsNovaUI::IconButton>())->painter = [this](juce::Graphics& graphics, juce::Rectangle<float> bounds, const ArsNovaUI::ButtonState& state) {
            ArsNovaUI::CustomLookAndFeel::get(*this).drawCodeButton(graphics, bounds, state);
        };

        for (int index = rowStart; index < iconButtons.size(); ++index) {
            ArsNovaUI::IconButton& button = *iconButtons[index];

            button.setSelected(sample == Sample::Selected);
            button.setEnabled(sample != Sample::Disabled);

            button.state.isHovered = sample == Sample::Hovered;

            addAndMakeVisible(button);
        }
    }

    numberEditor.setFormat(std::make_unique<ArsNovaUI::NumberFormat>(0, 127));
    wordEditor.setFormat(std::make_unique<ArsNovaUI::TextFormat>(12, "abcdefghijklmnopqrstuvwxyz"));
    pitchEditor.setFormat(std::make_unique<ArsNovaUI::PitchFormat>());
    letterEditor.setFormat(std::make_unique<ArsNovaUI::GreekLetterFormat>());
    pairEditor.setFormat(std::make_unique<ArsNovaUI::DualIntFormat>(1, 99, "PairSecond"));

    numberEditor.bindEditor(values, "Number");
    wordEditor.bindEditor(values, "Word");
    pitchEditor.bindEditor(values, "Pitch");
    letterEditor.bindEditor(values, "Letter");
    pairEditor.bindEditor(values, "Pair");

    for (ArsNovaUI::ValueEditor* editor : editors) {
        editor->backdrop = ArsNovaUI::ValueEditor::Backdrop::Field;

        addAndMakeVisible(editor);
    }

    itemSelector.addItem(1, "First item");
    itemSelector.addItem(2, "Second item");
    itemSelector.addItem(3, "Disabled item", nullptr, false);
    itemSelector.setSelectedItem(1);

    colourSelector.colour  = juce::Colour::fromRGB(96, 150, 220);
    menuButton.icon        = &ArsNovaUI::CustomLookAndFeel::drawSettingsIcon;
    port.initialViewCentre = { 340.0f, 260.0f };

    menuButton.onClick = [this]() {
        menu = std::make_unique<ArsNovaUI::ContextMenu>();

        menu->addItem("Show grid", ArsNovaUI::ContextMenu::ItemKind::Toggle, [this]() {
            canvas.grid.visible = ! canvas.grid.visible;

            ArsNovaUI::CustomLookAndFeel::get(*this).frostedBackdropStale = true;

            canvas.repaint();
            frostedBar.repaint();
        }, true, canvas.grid.visible);

        menu->addItem("Reset zoom", ArsNovaUI::ContextMenu::ItemKind::Action, [this]() { port.setZoom(1.0f, port.getLocalBounds().getCentre().toFloat()); });
        menu->addItem("Disabled action", ArsNovaUI::ContextMenu::ItemKind::Action, nullptr, false);

        menu->show(menuButton);
    };

    port.onTransformChanged = [this](const juce::AffineTransform& viewTransform, float) {
        canvas.grid.viewTransform  = viewTransform;
        canvas.grid.modelTransform = viewTransform.inverted();

        ArsNovaUI::CustomLookAndFeel::get(*this).frostedBackdropStale = true;

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
    graphics.fillAll(ArsNovaUI::CustomLookAndFeel::get(*this).windowColour);
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
