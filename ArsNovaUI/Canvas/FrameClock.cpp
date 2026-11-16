#include "FrameClock.h"

#include <algorithm>

namespace ArsNovaUI
{

FrameClock::Subscription::~Subscription()
{
    stop();
}

void FrameClock::Subscription::start(FrameClock* frameClock, std::function<void(double)> onFrame)
{
    if (frameClock == nullptr || isActive()) {
        return;
    }

    owner          = frameClock;
    subscriptionId = frameClock->subscribe(std::move(onFrame));
}

void FrameClock::Subscription::stop()
{
    if (subscriptionId != 0 && owner != nullptr) {
        owner->unsubscribe(subscriptionId);
    }

    subscriptionId = 0;
    owner          = nullptr;
}

bool FrameClock::Subscription::isActive() const
{
    return subscriptionId != 0;
}

FrameClock::FrameClock(juce::Component& host)
    : host(host)
{
}

int FrameClock::subscribe(std::function<void(double)> onFrame)
{
    const int subscriptionId = nextId++;

    entries.push_back({ subscriptionId, std::move(onFrame) });

    if (frames.isEmpty()) {
        frames = juce::VBlankAttachment(&host, [this](double frameSec) { advance(frameSec); });
    }

    return subscriptionId;
}

void FrameClock::unsubscribe(int subscriptionId)
{
    std::erase_if(entries, [subscriptionId](const Entry& entry) { return entry.id == subscriptionId; });

    if (entries.empty() && ! advancing) {
        frames = {};
    }
}

void FrameClock::advance(double frameSec)
{
    dueEntries = entries;
    advancing  = true;

    for (const Entry& due : dueEntries) {
        const bool stillSubscribed = std::ranges::any_of(entries, [&due](const Entry& entry) { return entry.id == due.id; });

        if (stillSubscribed) {
            due.onFrame(frameSec);
        }
    }

    advancing = false;

    if (entries.empty()) {
        frames = {};
    }
}

}
