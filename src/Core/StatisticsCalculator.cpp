#include "StatisticsCalculator.h"
#include "MinimumEnclosingCircle.h"

#include <cmath>
#include <algorithm>
#include <numeric>

Statistics StatisticsCalculator::calculate(const QList<ShotImpact> &impacts)
{
    Statistics stats;
    
    if (impacts.size() < 2) {
        stats.valid = false;
        return stats;
    }
    
    // Extract positions
    QList<QPointF> points;
    points.reserve(impacts.size());
    for (const auto &impact : impacts) {
        points.append(impact.position());
    }
    
    // Calculate centroid
    stats.centroid = calculateCentroid(points);
    
    // Calculate mean radius
    stats.meanRadiusPixels = calculateMeanRadius(points, stats.centroid);
    
    // Calculate standard deviation
    stats.standardDeviationPixels = calculateStandardDeviation(points, stats.centroid, stats.meanRadiusPixels);
    
    // Calculate group circles
    stats.fullGroupCircle = findFullGroupCircle(points);
    
    if (impacts.size() >= 3) {
        stats.group80Circle = find80PercentGroupCircle(points);
        stats.group90Circle = find90PercentGroupCircle(points);
    }
    
    stats.valid = true;
    return stats;
}

QPointF StatisticsCalculator::calculateCentroid(const QList<QPointF> &points)
{
    if (points.isEmpty())
    {
        return QPointF();
    }
    
    double sumX = 0.0;
    double sumY = 0.0;
    
    for (const auto &p : points)
    {
        sumX += p.x();
        sumY += p.y();
    }
    
    return QPointF(sumX / points.size(), sumY / points.size());
}

double StatisticsCalculator::calculateMeanRadius(const QList<QPointF> &points, const QPointF &centroid)
{
    if (points.isEmpty())
    {
        return 0.0;
    }
    
    double sumRadius = 0.0;
    
    for (const auto &p : points)
    {
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        sumRadius += std::sqrt(dx * dx + dy * dy);
    }
    
    return sumRadius / points.size();
}

double StatisticsCalculator::calculateStandardDeviation(const QList<QPointF> &points,
                                                        const QPointF &centroid,
                                                        double meanRadius)
{
    if (points.size() < 2)
    {
        return 0.0;
    }
    
    double sumSquaredDiff = 0.0;
    
    for (const auto &p : points)
    {
        double dx = p.x() - centroid.x();
        double dy = p.y() - centroid.y();
        double radius = std::sqrt(dx * dx + dy * dy);
        double diff = radius - meanRadius;
        sumSquaredDiff += diff * diff;
    }
    
    // Using sample standard deviation (N-1)
    return std::sqrt(sumSquaredDiff / (points.size() - 1));
}

GroupCircle StatisticsCalculator::findFullGroupCircle(const QList<QPointF> &points)
{
    if (points.isEmpty())
    {
        return GroupCircle();
    }
    
    auto circle = MinimumEnclosingCircle::findSmallestEnclosingCircle(points);
    
    // All points are included in the full group
    QList<int> indices;
    for (int i = 0; i < points.size(); ++i)
    {
        indices.append(i);
    }
    
    return GroupCircle(circle.center, circle.radius, indices);
}

GroupCircle StatisticsCalculator::find80PercentGroupCircle(const QList<QPointF> &points)
{
    return findPercentageGroupCircle(points, 0.80);
}

GroupCircle StatisticsCalculator::find90PercentGroupCircle(const QList<QPointF> &points)
{
    return findPercentageGroupCircle(points, 0.90);
}

GroupCircle StatisticsCalculator::findPercentageGroupCircle(const QList<QPointF> &points, double percentage)
{
    if (points.size() < 2)
    {
        return GroupCircle();
    }
    
    int targetCount = static_cast<int>(std::ceil(points.size() * percentage));
    targetCount = qMin(targetCount, points.size());
    
    if (targetCount == points.size())
    {
        return findFullGroupCircle(points);
    }
    
    // Brute force: try all combinations of targetCount points
    // For small N, this is acceptable (N choose K combinations)
    // For larger sets, we'd use a more sophisticated algorithm
    
    // Simple approach: find centroid, sort by distance, take closest targetCount
    QPointF centroid = calculateCentroid(points);
    
    // Create index-distance pairs
    QList<QPair<int, double>> distances;
    for (int i = 0; i < points.size(); ++i)
    {
        double dx = points[i].x() - centroid.x();
        double dy = points[i].y() - centroid.y();
        double dist = std::sqrt(dx * dx + dy * dy);
        distances.append(qMakePair(i, dist));
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
              [](const QPair<int, double> &a, const QPair<int, double> &b)
              {
                  return a.second < b.second;
              });
    
    // Take closest targetCount points
    QList<QPointF> subset;
    QList<int> indices;
    for (int i = 0; i < targetCount; ++i)
    {
        indices.append(distances[i].first);
        subset.append(points[distances[i].first]);
    }
    
    // Find minimum enclosing circle for this subset
    auto circle = MinimumEnclosingCircle::findSmallestEnclosingCircle(subset);
    
    return GroupCircle(circle.center, circle.radius, indices);
}
