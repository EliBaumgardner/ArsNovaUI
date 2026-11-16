#include "PianoKeyboard.h"
#include "Theme/CustomLookAndFeel.h"
#include "Theme/Theme.h"

#include <algorithm>

namespace ArsNovaUI
{

PianoKeyboard::PianoKeyboard()
{
    calculateGeometry();
}

void PianoKeyboard::paint(juce::Graphics& graphics)
{
    const CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    graphics.fillAll(lookAndFeel.surfaceColour);

    for (const KeyGeometry& key : whiteKeys) {
        paintWhiteKey(graphics, key);
    }

    for (const KeyGeometry& key : blackKeys) {
        paintBlackKey(graphics, key);
    }
}

void PianoKeyboard::resized()
{
    calculateGeometry();
}

void PianoKeyboard::mouseMove(const juce::MouseEvent& event)
{
    const int note = getNoteAt(event.position);

    if (note != hoveredNote) {
        hoveredNote = note;

        if (onNoteHovered) {
            onNoteHovered(note);
        }

        repaint();
    }
}

void PianoKeyboard::mouseExit(const juce::MouseEvent&)
{
    if (hoveredNote != -1) {
        hoveredNote = -1;

        if (onNoteHovered) {
            onNoteHovered(-1);
        }

        repaint();
    }
}

void PianoKeyboard::mouseDown(const juce::MouseEvent& event)
{
    const int note = getNoteAt(event.position);

    if (note < 0) {
        return;
    }

    if (event.mods.isPopupMenu()) {
        if (onNoteRightClicked) {
            onNoteRightClicked(note);
        }

        return;
    }

    activeMouseNote = note;

    if (onNoteClicked) {
        onNoteClicked(note);
    }

    if (onNoteStateChanged) {
        onNoteStateChanged(note, true);
    }

    repaint();
}

void PianoKeyboard::mouseDrag(const juce::MouseEvent& event)
{
    const int note = getNoteAt(event.position);

    if (note != activeMouseNote && note >= 0) {
        if (activeMouseNote >= 0 && onNoteStateChanged) {
            onNoteStateChanged(activeMouseNote, false);
        }

        activeMouseNote = note;

        if (onNoteClicked) {
            onNoteClicked(note);
        }

        if (onNoteStateChanged) {
            onNoteStateChanged(note, true);
        }

        repaint();
    }
}

void PianoKeyboard::mouseUp(const juce::MouseEvent&)
{
    if (activeMouseNote >= 0) {
        if (onNoteStateChanged) {
            onNoteStateChanged(activeMouseNote, false);
        }

        activeMouseNote = -1;

        repaint();
    }
}

void PianoKeyboard::mouseDoubleClick(const juce::MouseEvent& event)
{
    const int note = getNoteAt(event.position);

    if (note >= 0 && onNoteDoubleClicked) {
        onNoteDoubleClicked(note);
    }
}

void PianoKeyboard::setRange(int newLowestNote, int newHighestNote)
{
    newLowestNote  = juce::jlimit(0, highestMidiNote, newLowestNote);
    newHighestNote = juce::jlimit(newLowestNote, highestMidiNote, newHighestNote);

    if (lowestNote != newLowestNote || highestNote != newHighestNote) {
        lowestNote  = newLowestNote;
        highestNote = newHighestNote;

        calculateGeometry();
        repaint();
    }
}

void PianoKeyboard::shiftOctave(int deltaOctaves)
{
    const int span = highestNote - lowestNote;

    int newLowest = lowestNote + deltaOctaves * notesPerOctave;

    if (newLowest < 0) {
        newLowest = 0;
    }

    if (newLowest + span > highestMidiNote) {
        newLowest = highestMidiNote - span;
    }

    setRange(newLowest, newLowest + span);
}

void PianoKeyboard::setKeyPressed(int noteNumber, bool pressed)
{
    if (noteNumber >= 0 && noteNumber < midiNoteCount) {
        if (pressedKeys[static_cast<size_t>(noteNumber)] != pressed) {
            pressedKeys[static_cast<size_t>(noteNumber)] = pressed;

            repaint();
        }
    }
}

void PianoKeyboard::setSelectedKey(int noteNumber)
{
    if (selectedKey != noteNumber) {
        selectedKey = noteNumber;

        repaint();
    }
}

void PianoKeyboard::setKeyMapped(int noteNumber, bool mapped, juce::Colour markerColour)
{
    if (noteNumber >= 0 && noteNumber < midiNoteCount) {
        keyMappings[static_cast<size_t>(noteNumber)] = { mapped, markerColour };

        repaint();
    }
}

void PianoKeyboard::clearAllPressedKeys()
{
    pressedKeys.fill(false);

    repaint();
}

void PianoKeyboard::clearSelection()
{
    if (selectedKey != -1) {
        selectedKey = -1;

        repaint();
    }
}

void PianoKeyboard::clearAllMappings()
{
    keyMappings.fill({});

    repaint();
}

bool PianoKeyboard::isKeyPressed(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < midiNoteCount) {
        return pressedKeys[static_cast<size_t>(noteNumber)];
    }

    return false;
}

bool PianoKeyboard::isKeyMapped(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < midiNoteCount) {
        return keyMappings[static_cast<size_t>(noteNumber)].mapped;
    }

    return false;
}

juce::Colour PianoKeyboard::getKeyMarkerColour(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < midiNoteCount) {
        return keyMappings[static_cast<size_t>(noteNumber)].markerColour;
    }

    return juce::Colour();
}

bool PianoKeyboard::isBlackKey(int noteNumber)
{
    const int noteInOctave = (noteNumber % notesPerOctave + notesPerOctave) % notesPerOctave;

    return std::ranges::find(blackNotesInOctave, noteInOctave) != blackNotesInOctave.end();
}

juce::String PianoKeyboard::getNoteName(int noteNumber, bool includeOctave)
{
    static const char* const names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    if (noteNumber < 0 || noteNumber > highestMidiNote) {
        return juce::String();
    }

    juce::String result = names[noteNumber % notesPerOctave];

    if (includeOctave) {
        result += juce::String(noteNumber / notesPerOctave + lowestOctaveNumber);
    }

    return result;
}

void PianoKeyboard::paintWhiteKey(juce::Graphics& graphics, const KeyGeometry& key) const
{
    const CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const int  note     = key.noteNumber;
    const bool isCNote  = note % notesPerOctave == 0;
    const bool selected = note == selectedKey;

    const juce::Rectangle<float> rect = key.bounds.reduced(whiteKeyInset, 0.0f);

    const juce::Path path = outlinePath(rect, whiteKeyCornerRadius);

    juce::Colour keyColour     = lookAndFeel.whiteKeyColour;
    juce::Colour outlineColour = lookAndFeel.borderStrongColour.withAlpha(whiteOutlineAlpha);

    float outlineWidth = outlineStrokeWidth;
    float markerOffset = whiteMarkerOffset;

    if (isKeyPressed(note)) {
        keyColour = lookAndFeel.accentColour;
    }
    else if (selected) {
        keyColour = lookAndFeel.accentColour.withAlpha(selectedFillAlpha);
    }
    else if (note == hoveredNote) {
        keyColour = lookAndFeel.whiteKeyHoverColour;
    }

    if (selected) {
        outlineColour = lookAndFeel.accentColour;
        outlineWidth  = selectedStrokeWidth;
    }

    if (isCNote) {
        markerOffset = labelledMarkerOffset;
    }

    graphics.setColour(keyColour);
    graphics.fillPath(path);

    graphics.setColour(outlineColour);
    graphics.strokePath(path, juce::PathStrokeType(outlineWidth));

    if (isCNote) {
        const juce::Rectangle<float> labelArea(rect.getX(), rect.getBottom() - labelBottomOffset, rect.getWidth(), labelHeight);

        graphics.setFont(lookAndFeel.font(Theme::FontStyle::Mono, labelFontHeight));
        graphics.setColour(lookAndFeel.mutedTextColour);
        graphics.drawText(getNoteName(note), labelArea, juce::Justification::centred);
    }

    if (isKeyMapped(note)) {
        const juce::Point<float> markerCentre(rect.getCentreX(), rect.getBottom() - markerOffset);

        paintMarker(graphics, note, markerCentre, whiteMarkerDiameter, lookAndFeel.windowColour.withAlpha(markerOutlineAlpha));
    }
}

void PianoKeyboard::paintBlackKey(juce::Graphics& graphics, const KeyGeometry& key) const
{
    const CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    const int  note     = key.noteNumber;
    const bool selected = note == selectedKey;

    const juce::Rectangle<float> rect = key.bounds;

    const juce::Path path = outlinePath(rect, blackKeyCornerRadius);

    const juce::AffineTransform shadowShift = juce::AffineTransform::translation(0.0f, shadowOffset);

    juce::Colour keyColour     = lookAndFeel.blackKeyColour;
    juce::Colour outlineColour = lookAndFeel.borderStrongColour.withAlpha(blackOutlineAlpha);

    float outlineWidth = outlineStrokeWidth;

    if (isKeyPressed(note)) {
        keyColour = lookAndFeel.accentColour.darker(pressedBlackDarkening);
    }
    else if (selected) {
        keyColour = lookAndFeel.accentColour.darker(selectedBlackDarkening);
    }
    else if (note == hoveredNote) {
        keyColour = lookAndFeel.blackKeyHoverColour;
    }

    if (selected) {
        outlineColour = lookAndFeel.accentColour;
        outlineWidth  = selectedStrokeWidth;
    }

    graphics.setColour(lookAndFeel.dropShadowColour.withAlpha(shadowAlpha));
    graphics.fillPath(path, shadowShift);

    graphics.setColour(keyColour);
    graphics.fillPath(path);

    graphics.setColour(outlineColour);
    graphics.strokePath(path, juce::PathStrokeType(outlineWidth));

    if (isKeyMapped(note)) {
        const juce::Point<float> markerCentre(rect.getCentreX(), rect.getBottom() - blackMarkerOffset);

        paintMarker(graphics, note, markerCentre, blackMarkerDiameter, juce::Colours::white.withAlpha(markerOutlineAlpha));
    }
}

void PianoKeyboard::paintMarker(juce::Graphics& graphics, int noteNumber, juce::Point<float> centre, float diameter, juce::Colour outlineColour) const
{
    const juce::Rectangle<float> markerBounds = juce::Rectangle<float>(diameter, diameter).withCentre(centre);

    graphics.setColour(markerColourFor(noteNumber));
    graphics.fillEllipse(markerBounds);

    graphics.setColour(outlineColour);
    graphics.drawEllipse(markerBounds, markerStrokeWidth);
}

void PianoKeyboard::calculateGeometry()
{
    whiteKeys.clear();
    blackKeys.clear();

    const juce::Rectangle<float> bounds = getLocalBounds().toFloat();

    if (bounds.isEmpty()) {
        return;
    }

    int whiteCount = 0;
    int whiteIndex = 0;

    for (int note = lowestNote; note <= highestNote; ++note) {
        if (! isBlackKey(note)) {
            ++whiteCount;
        }
    }

    if (whiteCount == 0) {
        return;
    }

    const float whiteWidth  = bounds.getWidth() / static_cast<float>(whiteCount);
    const float whiteHeight = bounds.getHeight();
    const float blackWidth  = whiteWidth * blackKeyWidthRatio;
    const float blackHeight = whiteHeight * blackKeyHeightRatio;

    for (int note = lowestNote; note <= highestNote; ++note) {
        const int   noteInOctave = note % notesPerOctave;
        const float whiteX       = bounds.getX() + whiteIndex * whiteWidth;

        int groupStartWhite = 0;
        int groupWhiteCount = 0;
        int groupBlackCount = 0;
        int firstBlackNote  = 0;

        if (! isBlackKey(note)) {
            whiteKeys.push_back({ note, false, juce::Rectangle<float>(whiteX, bounds.getY(), whiteWidth, whiteHeight) });
            ++whiteIndex;

            continue;
        }

        if (noteInOctave < largeGroupFirstBlackNote) {
            groupStartWhite = smallGroupFirstWhiteKey;
            groupWhiteCount = smallGroupWhiteCount;
            groupBlackCount = smallGroupBlackCount;
            firstBlackNote  = smallGroupFirstBlackNote;
        }
        else {
            groupStartWhite = largeGroupFirstWhiteKey;
            groupWhiteCount = largeGroupWhiteCount;
            groupBlackCount = largeGroupBlackCount;
            firstBlackNote  = largeGroupFirstBlackNote;
        }

        const int semitonesBetweenBlackKeys = 2;
        const int placeInGroup              = (noteInOctave - firstBlackNote) / semitonesBetweenBlackKeys;
        const int whitesBelowInOctave       = groupStartWhite + placeInGroup + 1;
        const int octaveFirstWhite          = whiteIndex - whitesBelowInOctave;

        const float stripWidth = (groupWhiteCount * whiteWidth - groupBlackCount * blackWidth) / groupWhiteCount;
        const float octaveLeft = bounds.getX() + octaveFirstWhite * whiteWidth;
        const float blackX     = octaveLeft + groupStartWhite * whiteWidth + (placeInGroup + 1) * stripWidth + placeInGroup * blackWidth;

        blackKeys.push_back({ note, true, juce::Rectangle<float>(blackX, bounds.getY(), blackWidth, blackHeight) });
    }
}

int PianoKeyboard::getNoteAt(juce::Point<float> position) const
{
    for (const KeyGeometry& key : blackKeys) {
        if (key.bounds.contains(position)) {
            return key.noteNumber;
        }
    }

    for (const KeyGeometry& key : whiteKeys) {
        if (key.bounds.contains(position)) {
            return key.noteNumber;
        }
    }

    return -1;
}

juce::Colour PianoKeyboard::markerColourFor(int noteNumber) const
{
    const juce::Colour customColour = keyMappings[static_cast<size_t>(noteNumber)].markerColour;

    if (customColour.isTransparent()) {
        return CustomLookAndFeel::get(*this).accentColour;
    }

    return customColour;
}

juce::Path PianoKeyboard::outlinePath(const juce::Rectangle<float>& rect, float cornerRadius)
{
    const float left   = rect.getX();
    const float top    = rect.getY();
    const float right  = rect.getRight();
    const float bottom = rect.getBottom();

    const float rightCornerStart = bottom - cornerRadius;
    const float rightCornerEnd   = right - cornerRadius;
    const float leftCornerStart  = left + cornerRadius;

    juce::Path path;

    path.startNewSubPath(left, top);
    path.lineTo(right, top);
    path.lineTo(right, rightCornerStart);
    path.quadraticTo(right, bottom, rightCornerEnd, bottom);
    path.lineTo(leftCornerStart, bottom);
    path.quadraticTo(left, bottom, left, rightCornerStart);
    path.closeSubPath();

    return path;
}

}
