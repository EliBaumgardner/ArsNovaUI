#include "ToggleSwitch.h"
#include "../Theme/CustomLookAndFeel.h"

namespace ArsNovaUI
{

void ToggleSwitch::paint(juce::Graphics& graphics)
{
    const Theme&                 theme = CustomLookAndFeel::get(*this);
    const juce::Rectangle<float> track = getLocalBounds().toFloat().reduced(trackInset);

    juce::Colour trackColour = theme.raisedColour;
    juce::Colour textColour  = theme.mutedTextColour;

    juce::String stateText = "off";

    if (isOn) {
        trackColour = theme.accentColour;
        textColour  = theme.onAccentColour;
        stateText   = "on";
    }

    graphics.setColour(trackColour);
    graphics.fillRoundedRectangle(track, Theme::paneCornerRadius);

    graphics.setColour(theme.borderStrongColour);
    graphics.drawRoundedRectangle(track, Theme::paneCornerRadius, Theme::borderThickness);

    graphics.setColour(textColour);
    graphics.setFont(theme.font(Theme::FontStyle::Regular, Theme::labelFontHeight));
    graphics.drawText(stateText, getLocalBounds(), juce::Justification::centred);
}

void ToggleSwitch::mouseDown(const juce::MouseEvent&)
{
    isOn = !isOn;

    repaint();

    if (onToggle) {
        onToggle(isOn);
    }
}

}
