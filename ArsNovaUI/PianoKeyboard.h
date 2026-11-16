#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Theme/CustomLookAndFeel.h"
#include "Theme/Theme.h"

#include <array>
#include <functional>
#include <vector>

namespace ArsNovaUI
{

/*
    A piano keyboard component styled to the ArsNovaUI theme: draws white and black keys with
    clean geometry, rounded bottom corners, hover/pressed/selected states, and mapped key markers.
    It fits across any bar or panel, automatically dividing its width by the number of white keys
    in its note range.

    The range is configurable (lowestNote to highestNote), and shiftOctave moves it in 12-semitone
    steps. Keys can be pressed (active playback), selected (focused for mapping), or marked as mapped
    (with an optional custom color dot). Hit-testing checks black keys before white keys for exact
    mouse interaction.

    Callbacks report clicks, state transitions (down/up), right clicks (context / unmap), double
    clicks, and hover changes.
*/

class PianoKeyboard : public juce::Component
{
public:
    PianoKeyboard();
    ~PianoKeyboard() override = default;

    void paint   (juce::Graphics& g) override;
    void resized () override;

    void mouseMove        (const juce::MouseEvent& event) override;
    void mouseExit        (const juce::MouseEvent& event) override;
    void mouseDown        (const juce::MouseEvent& event) override;
    void mouseDrag        (const juce::MouseEvent& event) override;
    void mouseUp          (const juce::MouseEvent& event) override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;

    void setRange (int newLowestNote, int newHighestNote);
    int  getLowestNote  () const { return lowestNote; }
    int  getHighestNote () const { return highestNote; }
    void shiftOctave    (int deltaOctaves);

    void setKeyPressed       (int noteNumber, bool pressed);
    bool isKeyPressed        (int noteNumber) const;
    void clearAllPressedKeys ();

    void setSelectedKey (int noteNumber);
    int  getSelectedKey () const { return selectedKey; }
    void clearSelection ();

    void setKeyMapped       (int noteNumber, bool mapped, juce::Colour markerColour = juce::Colour());
    bool isKeyMapped        (int noteNumber) const;
    juce::Colour getKeyMarkerColour (int noteNumber) const;
    void clearAllMappings   ();

    static bool isBlackKey  (int noteNumber);
    static juce::String getNoteName (int noteNumber, bool includeOctave = true);

    std::function<void(int noteNumber)> onNoteClicked;
    std::function<void(int noteNumber, bool isDown)> onNoteStateChanged;
    std::function<void(int noteNumber)> onNoteRightClicked;
    std::function<void(int noteNumber)> onNoteDoubleClicked;
    std::function<void(int noteNumber)> onNoteHovered;

    static constexpr int defaultLowestNote  = 36; // C2
    static constexpr int defaultHighestNote = 84; // C6
    static constexpr float blackKeyWidthRatio  = 0.62f;
    static constexpr float blackKeyHeightRatio = 0.62f;

private:
    struct KeyGeometry
    {
        int noteNumber = 0;
        bool isBlack   = false;
        juce::Rectangle<float> bounds;
    };

    struct KeyMapping
    {
        bool mapped = false;
        juce::Colour markerColour;
    };

    void calculateGeometry ();
    int  getNoteAt (juce::Point<float> position) const;

    int lowestNote  = defaultLowestNote;
    int highestNote = defaultHighestNote;

    int selectedKey     = -1;
    int hoveredNote     = -1;
    int activeMouseNote = -1;

    std::array<bool, 128>       pressedKeys {};
    std::array<KeyMapping, 128> keyMappings {};

    std::vector<KeyGeometry> whiteKeys;
    std::vector<KeyGeometry> blackKeys;

    static constexpr int notesPerOctave           = 12;
    static constexpr int smallGroupFirstBlackNote = 1;
    static constexpr int smallGroupFirstWhiteKey  = 0;
    static constexpr int smallGroupWhiteCount     = 3;
    static constexpr int smallGroupBlackCount     = 2;
    static constexpr int largeGroupFirstBlackNote = 6;
    static constexpr int largeGroupFirstWhiteKey  = 3;
    static constexpr int largeGroupWhiteCount     = 4;
    static constexpr int largeGroupBlackCount     = 3;
};

}
