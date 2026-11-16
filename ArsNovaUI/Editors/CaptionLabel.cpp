#include "CaptionLabel.h"

namespace ArsNovaUI
{

CaptionLabel::CaptionLabel()
{
    setJustificationType(juce::Justification::centredLeft);
    setBorderSize({});
    setMinimumHorizontalScale(1.0f);
}

void CaptionLabel::parentHierarchyChanged()
{
    lookAndFeelChanged();
}

void CaptionLabel::lookAndFeelChanged()
{
    const Theme* const theme = dynamic_cast<const Theme*>(&getLookAndFeel());

    if (theme == nullptr) {
        return;
    }

    juce::Colour textColour = theme->mutedTextColour;

    switch (tone) {
        case Tone::Strong: textColour = theme->textColour;     break;
        case Tone::Soft:   textColour = theme->softTextColour; break;
        case Tone::Muted:                                      break;
    }

    setFont(theme->font(fontStyle, fontHeight));
    setColour(juce::Label::textColourId, textColour);
}

}
