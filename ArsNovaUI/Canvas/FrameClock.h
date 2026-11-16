#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <vector>

/*
    The one display-frame clock of an EditCanvas. Every node or arrow that is animating subscribes
    a callback, and the clock calls each one once per display frame with the frame's time in
    seconds. The clock holds a single juce::VBlankAttachment on the canvas, made when the first
    callback subscribes and dropped when the last one leaves, so a canvas with a hundred items
    animating still has one frame source and an idle canvas has none.

    subscribe adds a callback and returns its id, and unsubscribe removes it. A callback may
    unsubscribe itself while it runs. Subscription is the handle an item keeps: start subscribes
    its callback unless it is already subscribed or has no clock, stop leaves the clock, and the
    destructor leaves it too, so an item that is deleted never receives another frame. The clock
    is weakly referenced, so a handle that outlives its clock does nothing.

    EditCanvas owns the clock, and NodeManager and ArrowManager hand its address to every
    CanvasNode and CanvasArrow they create.
*/

namespace ArsNovaUI
{

class FrameClock
{
public:

    class Subscription
    {
    public:

        Subscription() = default;
        ~Subscription();

        Subscription(const Subscription&)            = delete;
        Subscription& operator=(const Subscription&) = delete;

        void start(FrameClock* frameClock, std::function<void(double)> onFrame);
        void stop ();

        bool isActive() const;

    private:

        juce::WeakReference<FrameClock> owner;

        int subscriptionId = 0;
    };

    explicit FrameClock(juce::Component& host);

    int  subscribe  (std::function<void(double)> onFrame);
    void unsubscribe(int subscriptionId);

private:

    struct Entry
    {
        int id;

        std::function<void(double)> onFrame;
    };

    void advance(double frameSec);

    juce::Component& host;

    std::vector<Entry> entries;
    std::vector<Entry> dueEntries;

    int nextId = 1;

    bool advancing = false;

    juce::VBlankAttachment frames;

    JUCE_DECLARE_WEAK_REFERENCEABLE(FrameClock)
};

}
