#include "PianoKeyboard.h"

namespace ArsNovaUI
{

PianoKeyboard::PianoKeyboard()
{
    calculateGeometry();
}

void PianoKeyboard::resized()
{
    calculateGeometry();
}

void PianoKeyboard::setRange(int newLowestNote, int newHighestNote)
{
    newLowestNote  = juce::jlimit(0, 127, newLowestNote);
    newHighestNote = juce::jlimit(newLowestNote, 127, newHighestNote);

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
    int newLowest = lowestNote + deltaOctaves * 12;

    if (newLowest < 0) {
        newLowest = 0;
    }
    if (newLowest + span > 127) {
        newLowest = 127 - span;
    }

    setRange(newLowest, newLowest + span);
}

void PianoKeyboard::setKeyPressed(int noteNumber, bool pressed)
{
    if (noteNumber >= 0 && noteNumber < 128) {
        if (pressedKeys[static_cast<size_t>(noteNumber)] != pressed) {
            pressedKeys[static_cast<size_t>(noteNumber)] = pressed;
            repaint();
        }
    }
}

bool PianoKeyboard::isKeyPressed(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < 128) {
        return pressedKeys[static_cast<size_t>(noteNumber)];
    }
    return false;
}

void PianoKeyboard::clearAllPressedKeys()
{
    pressedKeys.fill(false);
    repaint();
}

void PianoKeyboard::setSelectedKey(int noteNumber)
{
    if (selectedKey != noteNumber) {
        selectedKey = noteNumber;
        repaint();
    }
}

void PianoKeyboard::clearSelection()
{
    if (selectedKey != -1) {
        selectedKey = -1;
        repaint();
    }
}

void PianoKeyboard::setKeyMapped(int noteNumber, bool mapped, juce::Colour markerColour)
{
    if (noteNumber >= 0 && noteNumber < 128) {
        keyMappings[static_cast<size_t>(noteNumber)] = { mapped, markerColour };
        repaint();
    }
}

bool PianoKeyboard::isKeyMapped(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < 128) {
        return keyMappings[static_cast<size_t>(noteNumber)].mapped;
    }
    return false;
}

juce::Colour PianoKeyboard::getKeyMarkerColour(int noteNumber) const
{
    if (noteNumber >= 0 && noteNumber < 128) {
        return keyMappings[static_cast<size_t>(noteNumber)].markerColour;
    }
    return juce::Colour();
}

void PianoKeyboard::clearAllMappings()
{
    keyMappings.fill({});
    repaint();
}

bool PianoKeyboard::isBlackKey(int noteNumber)
{
    const int noteInOctave = (noteNumber % 12 + 12) % 12;
    return noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || noteInOctave == 8 || noteInOctave == 10;
}

juce::String PianoKeyboard::getNoteName(int noteNumber, bool includeOctave)
{
    if (noteNumber < 0 || noteNumber > 127) {
        return juce::String();
    }

    static const char* const names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    juce::String result = names[noteNumber % 12];

    if (includeOctave) {
        result += juce::String(noteNumber / 12 - 1);
    }

    return result;
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
        } else {
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
    for (const auto& key : blackKeys) {
        if (key.bounds.contains(position)) {
            return key.noteNumber;
        }
    }

    for (const auto& key : whiteKeys) {
        if (key.bounds.contains(position)) {
            return key.noteNumber;
        }
    }

    return -1;
}

void PianoKeyboard::paint(juce::Graphics& g)
{
    const CustomLookAndFeel& lookAndFeel = CustomLookAndFeel::get(*this);

    g.fillAll(lookAndFeel.surfaceColour);

    // 1. Paint White Keys
    for (const auto& key : whiteKeys) {
        const int note = key.noteNumber;
        const juce::Rectangle<float> rect = key.bounds.reduced(0.5f, 0.0f);

        juce::Colour keyColour = juce::Colour::fromRGB(222, 226, 234);

        if (isKeyPressed(note)) {
            keyColour = lookAndFeel.accentColour;
        } else if (note == selectedKey) {
            keyColour = lookAndFeel.accentColour.withAlpha(0.35f);
        } else if (note == hoveredNote) {
            keyColour = juce::Colour::fromRGB(242, 245, 250);
        }

        juce::Path path;
        const float r = 2.5f;
        path.startNewSubPath(rect.getX(), rect.getY());
        path.lineTo(rect.getRight(), rect.getY());
        path.lineTo(rect.getRight(), rect.getBottom() - r);
        path.quadraticTo(rect.getRight(), rect.getBottom(), rect.getRight() - r, rect.getBottom());
        path.lineTo(rect.getX() + r, rect.getBottom());
        path.quadraticTo(rect.getX(), rect.getBottom(), rect.getX(), rect.getBottom() - r);
        path.closeSubPath();

        g.setColour(keyColour);
        g.fillPath(path);

        if (note == selectedKey) {
            g.setColour(lookAndFeel.accentColour);
            g.strokePath(path, juce::PathStrokeType(1.5f));
        } else {
            g.setColour(lookAndFeel.borderStrongColour.withAlpha(0.25f));
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }

        // C note label
        if (note % 12 == 0) {
            g.setFont(lookAndFeel.font(Theme::FontStyle::Mono, 8.5f));
            g.setColour(lookAndFeel.mutedTextColour);
            const juce::Rectangle<float> labelArea(rect.getX(), rect.getBottom() - 14.0f, rect.getWidth(), 12.0f);
            g.drawText("C" + juce::String(note / 12 - 1), labelArea, juce::Justification::centred);
        }

        // Mapped indicator
        if (isKeyMapped(note)) {
            const juce::Colour dotColour = ! keyMappings[static_cast<size_t>(note)].markerColour.isTransparent()
                                            ? keyMappings[static_cast<size_t>(note)].markerColour
                                            : lookAndFeel.accentColour;

            const float dotY = (note % 12 == 0) ? rect.getBottom() - 20.0f : rect.getBottom() - 9.0f;
            const float dotDiameter = 6.0f;
            const juce::Rectangle<float> dotRect(rect.getCentreX() - dotDiameter * 0.5f, dotY - dotDiameter * 0.5f, dotDiameter, dotDiameter);

            g.setColour(dotColour);
            g.fillEllipse(dotRect);

            g.setColour(lookAndFeel.windowColour.withAlpha(0.4f));
            g.drawEllipse(dotRect, 0.75f);
        }
    }

    // 2. Paint Black Keys
    for (const auto& key : blackKeys) {
        const int note = key.noteNumber;
        const juce::Rectangle<float> rect = key.bounds;

        juce::Colour keyColour = juce::Colour::fromRGB(24, 28, 35);

        if (isKeyPressed(note)) {
            keyColour = lookAndFeel.accentColour.darker(0.15f);
        } else if (note == selectedKey) {
            keyColour = lookAndFeel.accentColour.darker(0.4f);
        } else if (note == hoveredNote) {
            keyColour = juce::Colour::fromRGB(44, 50, 62);
        }

        juce::Path path;
        const float r = 2.0f;
        path.startNewSubPath(rect.getX(), rect.getY());
        path.lineTo(rect.getRight(), rect.getY());
        path.lineTo(rect.getRight(), rect.getBottom() - r);
        path.quadraticTo(rect.getRight(), rect.getBottom(), rect.getRight() - r, rect.getBottom());
        path.lineTo(rect.getX() + r, rect.getBottom());
        path.quadraticTo(rect.getX(), rect.getBottom(), rect.getX(), rect.getBottom() - r);
        path.closeSubPath();

        // Subtle drop shadow behind black key
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillPath(path, juce::AffineTransform::translation(0.0f, 1.0f));

        g.setColour(keyColour);
        g.fillPath(path);

        if (note == selectedKey) {
            g.setColour(lookAndFeel.accentColour);
            g.strokePath(path, juce::PathStrokeType(1.5f));
        } else {
            g.setColour(lookAndFeel.borderStrongColour.withAlpha(0.35f));
            g.strokePath(path, juce::PathStrokeType(1.0f));
        }

        // Mapped indicator
        if (isKeyMapped(note)) {
            const juce::Colour dotColour = ! keyMappings[static_cast<size_t>(note)].markerColour.isTransparent()
                                            ? keyMappings[static_cast<size_t>(note)].markerColour
                                            : lookAndFeel.accentColour;

            const float dotY = rect.getBottom() - 7.0f;
            const float dotDiameter = 5.0f;
            const juce::Rectangle<float> dotRect(rect.getCentreX() - dotDiameter * 0.5f, dotY - dotDiameter * 0.5f, dotDiameter, dotDiameter);

            g.setColour(dotColour);
            g.fillEllipse(dotRect);

            g.setColour(juce::Colours::white.withAlpha(0.4f));
            g.drawEllipse(dotRect, 0.75f);
        }
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

void PianoKeyboard::mouseUp(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);

    if (activeMouseNote >= 0) {
        if (onNoteStateChanged) {
            onNoteStateChanged(activeMouseNote, false);
        }

        activeMouseNote = -1;
        repaint();
    }
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

void PianoKeyboard::mouseExit(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);

    if (hoveredNote != -1) {
        hoveredNote = -1;
        if (onNoteHovered) {
            onNoteHovered(-1);
        }
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

}
