#include "ValueFormat.h"

namespace ArsNovaUI
{

const juce::String TextFormat::labelCharacters {
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _-."
};

TextFormat::TextFormat(int longestText, const juce::String& characters)
    : maxLength(longestText), allowedCharacters(characters)
{
}

InputRestrictions TextFormat::restrictions() const
{
    InputRestrictions limits;

    limits.maxLength         = maxLength;
    limits.allowedCharacters = allowedCharacters;

    return limits;
}

juce::String TextFormat::text(const ValueBinding& binding, TextPurpose purpose) const
{
    const juce::var stored = binding.primary.getValue();

    if (! stored.isString()) {
        return {};
    }

    return stored.toString();
}

ParsedValue TextFormat::parse(const ValueBinding& binding, const juce::String& enteredText) const
{
    juce::String entered = enteredText;

    if (trimsWhitespace) {
        entered = enteredText.trim();
    }

    if (rejectsEmptyText && entered.isEmpty()) {
        return { binding.primary.getValue(), {} };
    }

    return { entered, {} };
}

}
