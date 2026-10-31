#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/*
    The animation state of one CanvasArrow, held as numbers and advanced by time with nothing drawn
    here. It has two parts. The snap is a spring that moves snapProgress from 0 to 1 when an arrow
    is connected, and a ruler that fades out as it settles. The fade moves alpha toward
    alphaTarget, which is how an arrow appears and disappears.

    advance takes the time of the display frame and moves both parts, and returns whether either
    is still moving or the arrow's own animation, which it is told of through otherRunning, is.
    CanvasArrow calls it from a callback it attaches to the display frames and removes once
    advance returns false, then reads the state to build its drawing.
*/

namespace ArsNovaUI
{

class ArrowAnimation
{
public:

    bool advance(double frameSec, bool otherRunning);

    static constexpr float snapSpringRateHz    = 60.0f;
    static constexpr float snapSpringStiffness = 0.20f;
    static constexpr float snapSpringDamping   = 0.30f;
    static constexpr float snapSettledEpsilon  = 0.001f;
    static constexpr float hoverFadePerSecond  = 4.8f;
    static constexpr float hoverFadeEpsilon    = 0.001f;
    static constexpr float rulerFadePerSecond  = 8.0f;
    static constexpr float rulerFadeStartGap   = 0.02f;

    float snapProgress = 1.0f;
    float snapVelocity = 0.0f;

    float alpha       = 1.0f;
    float alphaTarget = 1.0f;

    float rulerFade = 0.0f;

    double lastFrameSec = 0.0;

private:

    bool advanceSnap (float elapsedSec);
    bool advanceHover(float elapsedSec);

    bool snapSettled();
};

}
