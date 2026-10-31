#include "CanvasHitTester.h"
#include "EditCanvas.h"
#include "CanvasNode.h"
#include "CanvasArrow.h"

#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <utility>

namespace ArsNovaUI
{

namespace
{
template <std::ranges::input_range Range,
          std::indirectly_unary_invocable<std::ranges::iterator_t<const Range>> Project,
          std::indirect_unary_predicate<std::projected<std::ranges::iterator_t<const Range>, Project>> Keep,
          std::indirectly_unary_invocable<std::projected<std::ranges::iterator_t<const Range>, Project>> Distance>
auto nearest(const Range& range, Project project, Keep keep, Distance distanceTo, float radius)
    -> std::indirect_result_t<Project&, std::ranges::iterator_t<const Range>>
{
    using Candidate = std::indirect_result_t<Project&, std::ranges::iterator_t<const Range>>;

    Candidate best            = nullptr;
    float     closestDistance = radius;

    for (const auto& element : range) {
        Candidate const candidate = project(element);

        if (candidate == nullptr || ! keep(candidate)) {
            continue;
        }

        const float distance = distanceTo(candidate);

        if (distance < closestDistance) {
            closestDistance = distance;
            best            = candidate;
        }
    }

    return best;
}

CanvasArrow* identity(CanvasArrow* arrow)
{
    return arrow;
}

CanvasNode* nodeOf(const std::pair<const int, std::unique_ptr<CanvasNode>>& entry)
{
    return entry.second.get();
}
}

CanvasArrow* CanvasHitTester::arrowNear(juce::Point<float> point, float radius) const
{
    return nearest(canvas.arrowManager.all(), identity, [](CanvasArrow* arrow) { return arrow->startNode != nullptr && arrow->isVisible(); }, [point](CanvasArrow* arrow) { return distanceToSegment(point, arrow->startNode->getNodeCentre().toFloat(), arrow->getTip().toFloat()); }, radius);
}

CanvasArrow* CanvasHitTester::arrowHeadNear(juce::Point<float> point, float radius) const
{
    return nearest(canvas.arrowManager.all(), identity, [](CanvasArrow* arrow) { return !arrow->isDangling() && arrow->startNode != nullptr && arrow->isVisible(); }, [point](CanvasArrow* arrow) { return point.getDistanceFrom(arrow->getHeadAnchor()); }, radius);
}

CanvasArrow* CanvasHitTester::danglingHeadNear(juce::Point<float> point, float radius) const
{
    return nearest(canvas.arrowManager.all(), identity, [](CanvasArrow* arrow) { return arrow->isDangling() && arrow->startNode != nullptr && arrow->isVisible(); }, [point](CanvasArrow* arrow) { return point.getDistanceFrom(arrow->getTip().toFloat()); }, radius);
}

CanvasArrow* CanvasHitTester::arrowLabelNear(juce::Point<float> point, float radius) const
{
    return nearest(canvas.arrowManager.all(), identity, [](CanvasArrow* arrow) { return arrow->startNode != nullptr && arrow->isVisible() && arrow->showsDurationLabel(); }, [point](CanvasArrow* arrow) {
            const ArrowLabel label = arrow->getLabel(arrow->getGeometry(1.0f), CanvasArrow::arrowHeadLength);

            return point.getDistanceFrom(label.centre); }, radius);
}

CanvasNode* CanvasHitTester::nodeNear(juce::Point<float> point, float radius, int excludeId) const
{
    return nearest(canvas.nodeManager.all(), nodeOf, [excludeId](CanvasNode* node) { return node->nodeId != excludeId && node->isVisible(); }, [point](CanvasNode* node) { return point.getDistanceFrom(node->getNodeCentre().toFloat()); }, radius);
}

CanvasNode* CanvasHitTester::rootNear(juce::Point<float> point, float radius, int excludeId) const
{
    return nearest(canvas.nodeManager.all(), nodeOf, [excludeId](CanvasNode* node) { return node->nodeId != excludeId && node->isVisible() && node->isRoot; }, [point](CanvasNode* node) { return point.getDistanceFrom(node->getNodeCentre().toFloat()); }, radius);
}

CanvasNode* CanvasHitTester::nodeContaining(juce::Point<float> point, int excludeId) const
{
    return nearest(canvas.nodeManager.all(), nodeOf, [point, excludeId](CanvasNode* node) {
            return node->nodeId != excludeId
                && node->isVisible()
                && node->takesArrows
                && point.getDistanceFrom(node->getNodeCentre().toFloat()) <= node->getVisualRadius(); }, [point](CanvasNode* node) { return point.getDistanceFrom(node->getNodeCentre().toFloat()); }, std::numeric_limits<float>::max());
}

float CanvasHitTester::distanceToSegment(juce::Point<float> point, juce::Point<float> segmentStart, juce::Point<float> segmentEnd)
{
    const juce::Point<float> segment       = segmentEnd - segmentStart;
    const float              lengthSquared = segment.x * segment.x + segment.y * segment.y;

    if (lengthSquared < 1.0e-6f) {
        return point.getDistanceFrom(segmentStart);
    }

    const float alongSegment = juce::jlimit(0.0f, 1.0f, ((point.x - segmentStart.x) * segment.x + (point.y - segmentStart.y) * segment.y) / lengthSquared);

    return point.getDistanceFrom(segmentStart + segment * alongSegment);
}

}
