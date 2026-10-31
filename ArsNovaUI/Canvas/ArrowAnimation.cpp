#include "ArrowAnimation.h"

namespace ArsNovaUI
{

bool ArrowAnimation::advance(double frameSec, bool otherRunning)
{
    float elapsedSec = 0.0f;

    if (lastFrameSec > 0.0) {
        elapsedSec = static_cast<float>(frameSec - lastFrameSec);
    }

    lastFrameSec = frameSec;

    const bool snapRunning  = advanceSnap(elapsedSec);
    const bool hoverRunning = advanceHover(elapsedSec);

    const bool stillRunning = snapRunning || otherRunning || hoverRunning;

    if (! stillRunning) {
        lastFrameSec = 0.0;
    }

    return stillRunning;
}

bool ArrowAnimation::advanceSnap(float elapsedSec)
{
    if (1.0f - snapProgress < rulerFadeStartGap) {
        rulerFade = juce::jmax(0.0f, rulerFade - elapsedSec * rulerFadePerSecond);
    }

    if (snapSettled()) {
        return rulerFade > 0.0f;
    }

    const float springTicks = elapsedSec * snapSpringRateHz;

    snapVelocity += (1.0f - snapProgress) * snapSpringStiffness * springTicks;
    snapVelocity *= std::pow(snapSpringDamping, springTicks);
    snapProgress += snapVelocity * springTicks;

    if (snapSettled()) {
        snapProgress = 1.0f;
        snapVelocity = 0.0f;
        return rulerFade > 0.0f;
    }

    return true;
}

bool ArrowAnimation::advanceHover(float elapsedSec)
{
    if (alphaTarget < alpha && ! snapSettled()) {
        return true;
    }

    if (std::abs(alpha - alphaTarget) < hoverFadeEpsilon) {
        alpha = alphaTarget;
        return false;
    }

    float step = -hoverFadePerSecond * elapsedSec;

    if (alpha < alphaTarget) {
        step = hoverFadePerSecond * elapsedSec;
    }

    alpha = juce::jlimit(0.0f, 1.0f, alpha + step);
    return true;
}

bool ArrowAnimation::snapSettled()
{
    return std::abs(snapProgress - 1.0f) < snapSettledEpsilon
        && std::abs(snapVelocity) < snapSettledEpsilon;
}

}
