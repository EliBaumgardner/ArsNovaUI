#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

#include "ArrowAnimation.h"
#include "../Editors/ValueEditor.h"
#include "../Theme/Visuals.h"

/*
    The component that stands for one arrow on an EditCanvas, from startNode to endNode or, when
    dangling, from startNode to a loose tip at tipOffset from it. arrowTree is the tree of the
    connection or of the dangling arrow. The arrow's length is its meaning: its shaft runs from
    the start node to the tip, stopping at the edge of the end node's body, straight or in a
    curve, and getGeometry, getLabel and buildShaftPath work that out for the drawing.

    It also holds how the arrow looks right now: hovered, selected, being dragged, a ghost, and the
    ArrowAnimation of its snap and fade, which advanceAnimation moves on each display frame while
    anything is moving. durationEditor is the editor for the number shown on the shaft.

    The program that uses the canvas derives from it, draws it, and says through the pure
    virtuals what the label shows, whether it shows one and how the label is edited.
    advanceExtraAnimation lets the subclass animate more on the same frames. ArrowManager creates
    every arrow through EditCanvas::makeArrow or makeDanglingArrow, and CanvasHitTester and
    ArrowGestures find and drag it.
*/

namespace ArsNovaUI
{

class CanvasNode;

class CanvasArrow : public juce::Component
{
public:

    CanvasArrow(CanvasNode* startNode, CanvasNode* endNode, juce::UndoManager& undoManager);
    CanvasArrow(CanvasNode* startNode, juce::Point<int> tipOffset, juce::UndoManager& undoManager);

    void resized() override;

    void setArrowBounds ();
    void setTipOffset(juce::Point<int> offset);

    void endDrag ();
    void triggerSnapAnimation ();
    void advanceAnimation(double frameSec);

    void setHoverFade  (bool shouldBeVisible);
    void initHoverState(bool visibleNow);

    virtual void beginDurationEdit() = 0;

    virtual bool advanceExtraAnimation();

    virtual bool showsDurationLabel() const = 0;

    virtual juce::String getDurationLabel() const = 0;

    bool isDangling() const
    {
        return endNode == nullptr;
    }

    ArrowGeometry getGeometry(float animationProgress) const;

    ArrowLabel getLabel(const ArrowGeometry& geometry, float headLength) const;

    juce::Path buildShaftPath(const ArrowGeometry& geometry, float headLength, juce::Point<float> origin) const;

    juce::Point<int> getTip() const;

    juce::Point<float> getHeadAnchor() const;

    static constexpr float curvePerpScale         = 0.8f;
    static constexpr float curveOffsetFactor      = 0.15f;
    static constexpr float headVisibleThreshold   = 0.3f;
    static constexpr float labelVisibleThreshold  = 0.8f;
    static constexpr float headAnchorInset        = 8.0f;
    static constexpr float arrowHeadLength        = 9.0f;
    static constexpr float arrowHeadLengthHover   = 11.0f;
    static constexpr float verticalLabelThreshold = 0.2f;
    static constexpr int   arrowBoundsPadding     = 40;
    static constexpr int   valueEditorWidth       = 30;
    static constexpr int   valueEditorHeight      = 12;

    CanvasNode* const startNode = nullptr;
    CanvasNode* const endNode   = nullptr;

    juce::Point<int> tipOffset;

    int danglingIndex = -1;

    std::unique_ptr<ValueEditor> durationEditor;

    juce::ValueTree arrowTree;

    ArrowAnimation         animation;
    juce::VBlankAttachment animationFrames;

    bool editingDuration = false;
    bool isGhost         = false;
    bool dragging        = false;
    bool hovered         = false;
    bool selected        = false;

private:

    void buildDurationEditor(juce::UndoManager& undoManager);
};

}
