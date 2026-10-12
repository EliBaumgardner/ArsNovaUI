#include "GalleryApplication.h"

const juce::String GalleryApplication::getApplicationName()
{
    return "ArsNovaUI Gallery";
}

const juce::String GalleryApplication::getApplicationVersion()
{
    return "0.1.0";
}

void GalleryApplication::initialise(const juce::String& commandLine)
{
    const juce::String snapshotPath = commandLine.fromFirstOccurrenceOf("--snapshot", false, false).trim().unquoted();

    window = std::make_unique<GalleryWindow>(getApplicationName());

    if (snapshotPath.isEmpty()) {
        return;
    }

    juce::Timer::callAfterDelay(snapshotDelayMs, [this, snapshotPath]() {
        juce::Component* const page  = window->getContentComponent();
        const juce::Image      image = page->createComponentSnapshot(page->getLocalBounds(), true, snapshotScale);
        juce::File             file { snapshotPath };
        juce::PNGImageFormat   png;

        file.deleteFile();

        juce::FileOutputStream stream { file };

        png.writeImageToStream(image, stream);
        stream.flush();

        systemRequestedQuit();
    });
}

void GalleryApplication::shutdown()
{
    window = nullptr;
}

START_JUCE_APPLICATION(GalleryApplication)
