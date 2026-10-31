#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*
    The listener that turns each change to a canvas's document tree into an entry in EditCanvas's
    queue of AsyncUpdates, which says what the canvas must redraw and for which node. A node added
    to or removed from the document becomes NodeAdded or NodeRemoved; an id added to or removed
    from a node's list of children becomes ArrowAdded or ArrowRemoved; a changed XPosition or
    YPosition becomes NodeMoved; and a change to a node's dangling arrows or their tips becomes
    DanglingArrowsChanged. A change to any other property is handed to
    EditCanvas::treePropertyChanged, which the program that uses the canvas turns into the updates
    it needs.

    It only appends to the queue and touches no component, so the changes are applied later in
    one place, EditCanvas::handleAsyncUpdate, whatever caused them. That includes undo, redo and
    paste. The canvas's owner attaches it to the document's tree.
*/

namespace ArsNovaUI
{

class EditCanvas;

class CanvasTreeListener : public juce::ValueTree::Listener
{
public:

    explicit CanvasTreeListener(EditCanvas& canvas);

    void valueTreeChildAdded     (juce::ValueTree& parent, juce::ValueTree& child) override;
    void valueTreeChildRemoved   (juce::ValueTree& parent, juce::ValueTree& child, int childIndex) override;
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& propertyIdentifier) override;

private:

    void enqueueDanglingArrowsChanged(const juce::ValueTree& nodeTree) const;

    EditCanvas& canvas;
};

}
