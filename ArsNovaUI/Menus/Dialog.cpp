#include "Dialog.h"
#include "../ScaledView.h"
#include "../Theme/CustomLookAndFeel.h"

#include <cmath>

namespace ArsNovaUI
{

void Dialog::addButton(juce::String label, std::function<void()> action)
{
    buttons.push_back({ std::move(label), std::move(action) });
}

void Dialog::show(juce::Component& target) const
{
    ScaledView* const      scaledView = target.findParentComponentOfClass<ScaledView>();
    juce::Component* const topLevel   = target.getTopLevelComponent();

    juce::Component* host = topLevel;

    auto panel = std::make_unique<Panel>(*this);

    auto runChosenAction = [choices = buttons](int result) {
        const int choiceCount = static_cast<int>(choices.size());

        const std::size_t chosenIndex = static_cast<std::size_t>(result - 1);

        if (result <= 0 || result > choiceCount) {
            return;
        }

        const Button& chosen = choices[chosenIndex];

        if (chosen.action) {
            chosen.action();
        }
    };

    if (scaledView != nullptr) {
        host = &scaledView->content;
    }

    host->addAndMakeVisible(*panel);

    panel->setBounds(host->getLocalBounds());

    panel->enterModalState(true, juce::ModalCallbackFunction::create(runChosenAction), true);

    panel.release();
}

Dialog::Panel::Panel(const Dialog& dialog)
    : title  (dialog.title),
      message(dialog.message)
{
    setWantsKeyboardFocus(true);

    for (const Button& choice : dialog.buttons) {
        const int result = static_cast<int>(buttons.size()) + firstButtonResult;

        ButtonState::Look look = ButtonState::Look::Raised;

        auto button = std::make_unique<IconButton>();

        if (buttons.empty()) {
            look = ButtonState::Look::Accent;
        }

        button->state.look = look;

        button->setText(choice.label);
        button->setSelected(true);

        button->onClick = [this, result]() { exitModalState(result); };

        addAndMakeVisible(*button);

        buttons.push_back(std::move(button));
    }
}

void Dialog::Panel::paint(juce::Graphics& graphics)
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const juce::Rectangle<float> area        = getLocalBounds().toFloat();
    const juce::Rectangle<float> boxBounds   = box.toFloat();
    const juce::Rectangle<float> titleBounds = titleArea.toFloat();
    const juce::Rectangle<float> textBounds  = messageArea.toFloat();

    lookAndFeel.drawDialog(graphics, area, boxBounds);

    titleLayout.draw(graphics, titleBounds);
    messageLayout.draw(graphics, textBounds);
}

void Dialog::Panel::resized()
{
    CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const juce::Font titleFont   = lookAndFeel.font(Theme::FontStyle::SemiBold, Theme::dialogTitleHeight);
    const juce::Font messageFont = lookAndFeel.font(Theme::FontStyle::Regular, Theme::textHeight);
    const juce::Font buttonFont  = lookAndFeel.font(Theme::FontStyle::SemiBold, Theme::textHeight);

    const int textWidth = Theme::dialogWidth - 2 * Theme::dialogPadding;

    const float layoutWidth = static_cast<float>(textWidth);

    juce::AttributedString titleText;
    juce::AttributedString messageText;

    titleText.append(title, titleFont, lookAndFeel.textColour);
    messageText.append(message, messageFont, lookAndFeel.softTextColour);

    titleLayout.createLayout(titleText, layoutWidth);
    messageLayout.createLayout(messageText, layoutWidth);

    const int titleHeight   = static_cast<int>(std::ceil(titleLayout.getHeight()));
    const int messageHeight = static_cast<int>(std::ceil(messageLayout.getHeight()));
    const int boxHeight     = Theme::dialogPadding * 3 + titleHeight + Theme::dialogTextGap + messageHeight + Theme::dialogButtonHeight;

    box = getLocalBounds().withSizeKeepingCentre(Theme::dialogWidth, boxHeight);

    juce::Rectangle<int> content   = box.reduced(Theme::dialogPadding);
    juce::Rectangle<int> buttonRow = content.removeFromBottom(Theme::dialogButtonHeight);

    titleArea = content.removeFromTop(titleHeight);

    content.removeFromTop(Theme::dialogTextGap);

    messageArea = content.removeFromTop(messageHeight);

    for (const std::unique_ptr<IconButton>& button : buttons) {
        const int labelWidth  = juce::GlyphArrangement::getStringWidthInt(buttonFont, button->state.text);
        const int buttonWidth = labelWidth + 2 * Theme::dialogButtonTextInset;

        button->setBounds(buttonRow.removeFromRight(buttonWidth));

        buttonRow.removeFromRight(Theme::buttonGap);
    }
}

void Dialog::Panel::parentSizeChanged()
{
    const juce::Rectangle<int> parentArea = getParentComponent()->getLocalBounds();

    setBounds(parentArea);
}

bool Dialog::Panel::keyPressed(const juce::KeyPress& key)
{
    const int lastButtonResult = static_cast<int>(buttons.size());

    if (key.isKeyCode(juce::KeyPress::escapeKey)) {
        exitModalState(lastButtonResult);

        return true;
    }

    if (key.isKeyCode(juce::KeyPress::returnKey)) {
        exitModalState(firstButtonResult);

        return true;
    }

    return false;
}

}
