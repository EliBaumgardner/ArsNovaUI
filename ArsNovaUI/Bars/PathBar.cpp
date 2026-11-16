#include "PathBar.h"
#include "../Theme/CustomLookAndFeel.h"

namespace ArsNovaUI
{

void PathBar::paint(juce::Graphics& graphics)
{
    const Theme& theme = CustomLookAndFeel::get(*this);

    const juce::Font font = theme.font(Theme::FontStyle::Regular, pathFontHeight);

    juce::Rectangle<int> bounds = getLocalBounds().reduced(pathInset, 0);

    graphics.setColour(theme.surfaceColour);
    graphics.fillRect(getLocalBounds());

    graphics.setFont(font);

    for (int index = 0; index < segments.size(); ++index) {
        const int width = juce::GlyphArrangement::getStringWidthInt(font, segments[index]);

        if (index > 0) {
            graphics.setColour(theme.dimTextColour);
            graphics.drawText(juce::String::fromUTF8("\xe2\x80\xba"), bounds.removeFromLeft(separatorWidth), juce::Justification::centred, false);
        }

        if (index == segments.size() - 1) {
            graphics.setColour(theme.textColour);
        }
        else {
            graphics.setColour(theme.mutedTextColour);
        }

        graphics.drawText(segments[index], bounds.removeFromLeft(width), juce::Justification::centredLeft, true);
    }
}

}
