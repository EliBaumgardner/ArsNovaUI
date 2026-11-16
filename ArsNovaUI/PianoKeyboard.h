#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <functional>
#include <vector>

/*
    A piano keyboard component styled to the ArsNovaUI theme: draws white and black keys with
    clean geometry, rounded bottom corners, hover/pressed/selected states, and mapped key markers.
    It fits across any bar or panel, automatically dividing its width by the number of white keys
    in its note range.

    The range is lowestNote to highestNote, set through setRange and moved in 12-semitone steps by
    shiftOctave. selectedKey is the key focused for mapping, set through setSelectedKey. Keys can
    be pressed (active playback), selected, or marked as mapped (with an optional custom color
    dot). Hit-testing checks black keys before white keys for exact mouse interaction. The key
    colours come from the Theme (whiteKeyColour, blackKeyColour and their hover colours).

    Callbacks report clicks, state transitions (down/up), right clicks (context / unmap), double
    clicks, and hover changes.
*/

namespace ArsNovaUI
{

class PianoKeyboard : public juce::Component
{
public:

    PianoKeyboard();

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    void mouseMove       (const juce::MouseEvent& event) override;
    void mouseExit       (const juce::MouseEvent& event) override;
    void mouseDown       (const juce::MouseEvent& event) override;
    void mouseDrag       (const juce::MouseEvent& event) override;
    void mouseUp         (const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

    void setRange   (int newLowestNote, int newHighestNote);
    void shiftOctave(int deltaOctaves);

    void setKeyPressed (int noteNumber, bool pressed);
    void setSelectedKey(int noteNumber);
    void setKeyMapped  (int noteNumber, bool mapped, juce::Colour markerColour = juce::Colour());

    void clearAllPressedKeys ();
    void clearSelection ();
    void clearAllMappings ();

    bool isKeyPressed(int noteNumber) const;
    bool isKeyMapped (int noteNumber) const;

    juce::Colour getKeyMarkerColour(int noteNumber) const;

    static bool         isBlackKey (int noteNumber);
    static juce::String getNoteName(int noteNumber, bool includeOctave = true);

    static constexpr int defaultLowestNote  = 36;
    static constexpr int defaultHighestNote = 84;
    static constexpr int highestMidiNote    = 127;
    static constexpr int midiNoteCount      = 128;
    static constexpr int notesPerOctave     = 12;
    static constexpr int lowestOctaveNumber = -1;

    static constexpr float blackKeyWidthRatio  = 0.62f;
    static constexpr float blackKeyHeightRatio = 0.62f;

    std::function<void(int noteNumber)>              onNoteClicked;
    std::function<void(int noteNumber, bool isDown)> onNoteStateChanged;
    std::function<void(int noteNumber)>              onNoteRightClicked;
    std::function<void(int noteNumber)>              onNoteDoubleClicked;
    std::function<void(int noteNumber)>              onNoteHovered;

    int lowestNote  = defaultLowestNote;
    int highestNote = defaultHighestNote;

    int selectedKey = -1;

private:

    struct KeyGeometry
    {
        int noteNumber = 0;

        bool isBlack = false;

        juce::Rectangle<float> bounds;
    };

    struct KeyMapping
    {
        bool mapped = false;

        juce::Colour markerColour;
    };

    void paintWhiteKey(juce::Graphics& graphics, const KeyGeometry& key) const;
    void paintBlackKey(juce::Graphics& graphics, const KeyGeometry& key) const;
    void paintMarker  (juce::Graphics& graphics, int noteNumber, juce::Point<float> centre, float diameter, juce::Colour outlineColour) const;
    void calculateGeometry ();

    int getNoteAt(juce::Point<float> position) const;

    juce::Colour markerColourFor(int noteNumber) const;

    static juce::Path outlinePath(const juce::Rectangle<float>& rect, float cornerRadius);

    static constexpr std::array<int, 5> blackNotesInOctave { 1, 3, 6, 8, 10 };

    static constexpr int smallGroupFirstBlackNote = 1;
    static constexpr int smallGroupFirstWhiteKey  = 0;
    static constexpr int smallGroupWhiteCount     = 3;
    static constexpr int smallGroupBlackCount     = 2;
    static constexpr int largeGroupFirstBlackNote = 6;
    static constexpr int largeGroupFirstWhiteKey  = 3;
    static constexpr int largeGroupWhiteCount     = 4;
    static constexpr int largeGroupBlackCount     = 3;

    static constexpr float whiteKeyInset          = 0.5f;
    static constexpr float whiteKeyCornerRadius   = 2.5f;
    static constexpr float blackKeyCornerRadius   = 2.0f;
    static constexpr float selectedStrokeWidth    = 1.5f;
    static constexpr float outlineStrokeWidth     = 1.0f;
    static constexpr float whiteOutlineAlpha      = 0.25f;
    static constexpr float blackOutlineAlpha      = 0.35f;
    static constexpr float selectedFillAlpha      = 0.35f;
    static constexpr float pressedBlackDarkening  = 0.15f;
    static constexpr float selectedBlackDarkening = 0.4f;
    static constexpr float shadowAlpha            = 0.35f;
    static constexpr float shadowOffset           = 1.0f;
    static constexpr float labelFontHeight        = 8.5f;
    static constexpr float labelBottomOffset      = 14.0f;
    static constexpr float labelHeight            = 12.0f;
    static constexpr float whiteMarkerOffset      = 9.0f;
    static constexpr float labelledMarkerOffset   = 20.0f;
    static constexpr float whiteMarkerDiameter    = 6.0f;
    static constexpr float blackMarkerOffset      = 7.0f;
    static constexpr float blackMarkerDiameter    = 5.0f;
    static constexpr float markerStrokeWidth      = 0.75f;
    static constexpr float markerOutlineAlpha     = 0.4f;

    int hoveredNote     = -1;
    int activeMouseNote = -1;

    std::array<bool, midiNoteCount>       pressedKeys {};
    std::array<KeyMapping, midiNoteCount> keyMappings {};

    std::vector<KeyGeometry> whiteKeys;
    std::vector<KeyGeometry> blackKeys;
};

}
