#include "ContextMenu.h"
#include "../Theme/CustomLookAndFeel.h"

namespace ArsNovaUI
{

void ContextMenu::addItem(juce::String label, ItemKind kind, std::function<void()> action,
                          bool isEnabled, bool isOn)
{
    items.push_back({ std::move(label), kind, std::move(action), isEnabled, isOn });
}

void ContextMenu::addComponent(juce::Component& component, int idealWidth, int idealHeight)
{
    items.push_back({ {}, ItemKind::Component, nullptr, true, false, &component, idealWidth, idealHeight });
}

void ContextMenu::addSeparator()
{
    items.push_back({ {}, ItemKind::Separator, nullptr, false, false });
}

void ContextMenu::show(juce::Component& target)
{
    juce::PopupMenu                               menu;
    juce::Component::SafePointer<juce::Component> safeTarget(&target);
    int                                           itemId = 1;

    const juce::Colour toggleTextColour = CustomLookAndFeel::get(target).captionColour;

    menu.setLookAndFeel(&target.getLookAndFeel());

    for (const Item& item : items) {
        switch (item.kind) {
            case ItemKind::Action: {
                menu.addItem(itemId, item.label, item.isEnabled, false);
                break;
            }
            case ItemKind::Toggle: {
                juce::PopupMenu::Item toggleItem(item.label);

                toggleItem.setID(itemId).setEnabled(item.isEnabled).setTicked(item.isOn).setColour(toggleTextColour);

                menu.addItem(toggleItem);
                break;
            }
            case ItemKind::Component: {
                menu.addCustomItem(itemId, *item.component, item.idealWidth, item.idealHeight, false);
                break;
            }
            case ItemKind::Separator: {
                menu.addSeparator();
                break;
            }
        }

        ++itemId;
    }

    juce::PopupMenu::Options options = juce::PopupMenu::Options().withTargetComponent(target).withMousePosition();

    if (placement == Placement::BelowAnchorRightAligned) {
        const juce::Rectangle<int> screenArea = screenAreaBelowAnchor(target, options);

        options = options.withTargetScreenArea(screenArea);
    }

    if (hosting == Hosting::InsideWindow) {
        options = options.withParentComponent(insideHost(target));
    }

    menu.showMenuAsync(options, [safeTarget, chosenItems = items, dismissed = onDismissed](int result) {
        if (safeTarget == nullptr) {
            return;
        }

        if (dismissed) {
            dismissed();
        }

        if (result <= 0 || result > static_cast<int>(chosenItems.size())) {
            return;
        }

        const Item& chosen = chosenItems[static_cast<size_t>(result - 1)];

        if (chosen.action) {
            chosen.action();
        }
    });
}

juce::Component* ContextMenu::insideHost(juce::Component& target)
{
    juce::Component* host = target.getTopLevelComponent();

    for (juce::Component* ancestor = target.getParentComponent(); ancestor != nullptr; ancestor = ancestor->getParentComponent()) {
        if (ancestor->isTransformed()) {
            host = ancestor;
        }
    }

    return host;
}

juce::Rectangle<int> ContextMenu::screenAreaBelowAnchor(juce::Component& target, const juce::PopupMenu::Options& options) const
{
    juce::LookAndFeel& lookAndFeel = target.getLookAndFeel();

    const juce::Rectangle<int> anchorOnScreen = target.localAreaToGlobal(anchor);

    const float scale = juce::Component::getApproximateScaleFactorForComponent(&target);

    int widestItem = 0;

    for (const Item& item : items) {
        const bool isSeparator = item.kind == ItemKind::Separator;

        int itemWidth  = item.idealWidth;
        int itemHeight = item.idealHeight;

        if (item.kind != ItemKind::Component) {
            lookAndFeel.getIdealPopupMenuItemSizeWithOptions(item.label, isSeparator, options.getStandardItemHeight(), itemWidth, itemHeight, options);
        }

        widestItem = std::max(widestItem, itemWidth);
    }

    const int border      = lookAndFeel.getPopupMenuBorderSizeWithOptions(options);
    const int menuWidth   = widestItem + border * 2;
    const int screenWidth = juce::roundToInt(menuWidth * scale);
    const int shadowWidth = juce::roundToInt(Theme::popupMenuShadowRadius * scale);
    const int screenLeft  = anchorOnScreen.getRight() + shadowWidth - screenWidth;

    const juce::Rectangle<int> alignedArea = anchorOnScreen.withLeft(screenLeft);

    return alignedArea.translated(0, -shadowWidth);
}

}
