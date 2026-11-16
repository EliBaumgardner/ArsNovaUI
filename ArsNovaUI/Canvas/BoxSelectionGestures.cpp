#include "BoxSelectionGestures.h"
#include "EditCanvas.h"
#include "CanvasNode.h"

namespace ArsNovaUI
{

BoxSelectionGestures::BoxSelectionGestures(EditCanvas& canvas)
    : canvas(canvas)
{
}

void BoxSelectionGestures::begin(juce::Point<int> clickPoint)
{
    anchor = clickPoint;

    canvas.selectionBounds = {};

    canvas.nodeManager.clearAll();
}

void BoxSelectionGestures::update(const juce::MouseEvent& e)
{
    const juce::Point<int> cursor = e.getEventRelativeTo(&canvas).position.transformedBy(canvas.modelTransform).roundToInt();

    canvas.selectionBounds = juce::Rectangle<int>(anchor, cursor);

    canvas.repaint();
}

void BoxSelectionGestures::finish()
{
    const juce::Rectangle<int> selection = canvas.selectionBounds;

    for (auto& [nodeId, node] : canvas.nodeManager.all()) {
        if (node->isVisible() && selection.contains(node->getNodeCentre())) {
            node->setSelectVisual(true);
        }
    }

    canvas.selectionBounds = {};

    canvas.repaint();
}

}
