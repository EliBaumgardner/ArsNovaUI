#include "GalleryWindow.h"

GalleryWindow::GalleryWindow(const juce::String& title)
    : DocumentWindow(title, juce::Colours::black, juce::DocumentWindow::allButtons)
{
    setLookAndFeel(&lookAndFeel);
    setUsingNativeTitleBar(true);
    setResizable(true, false);
    setContentNonOwned(&page, false);
    centreWithSize(GalleryPage::defaultWidth, GalleryPage::defaultHeight);
    setVisible(true);
}

GalleryWindow::~GalleryWindow()
{
    clearContentComponent();
    setLookAndFeel(nullptr);
}

void GalleryWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
