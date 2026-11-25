#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include <vector>

namespace ArsNovaUI
{

class ContextMenu
{
public:

    enum class ItemKind;

    void addItem     (juce::String label, ItemKind kind, std::function<void()> action, bool isEnabled = true, bool isOn = false);
    void addComponent(juce::Component& component, int idealWidth, int idealHeight);
    void addSeparator ();

    void show(juce::Component& target);

    static juce::Component* insideHost(juce::Component& target);

    enum class ItemKind {
        Action,
        Toggle,
        Component,
        Separator
    };

    enum class Placement {
        AtMouse,
        BelowAnchorRightAligned
    };

    enum class Hosting {
        Desktop,
        InsideWindow
    };

    std::function<void()> onDismissed;

    Placement placement = Placement::AtMouse;

    Hosting hosting = Hosting::InsideWindow;

    juce::Rectangle<int> anchor;

private:

    juce::Rectangle<int> screenAreaBelowAnchor(juce::Component& target, const juce::PopupMenu::Options& options) const;

    struct Item
    {
        juce::String          label;
        ItemKind              kind;
        std::function<void()> action;
        bool                  isEnabled;
        bool                  isOn;
        juce::Component*      component   = nullptr;
        int                   idealWidth  = 0;
        int                   idealHeight = 0;
    };

    std::vector<Item> items;
};

}
