#include "MinimumEnclosingCircle.h"

#include <cmath>
#include <limits>

namespace
{
    constexpr double kEpsilon = 1e-9;
}

bool Circle::contains(const QPointF &p, double epsilon) const
{
    double dx = p.x() - center.x();
    double dy = p.y() - center.y();
    return std::sqrt(dx * dx + dy * dy) <= radius + epsilon;
}

Circle MinimumEnclosingCircle::findSmallestEnclosingCircle(const QList<QPointF> &points)
{
    if (points.isEmpty())
    {
        return Circle();
    }
    
    if (points.size() == 1)
    {
        return Circle(points[0], 0.0);
    }
    
    if (points.size() == 2)
    {
        return circleFromTwoPoints(points[0], points[1]);
    }
    
    // Naive O(N³) algorithm:
    // Try all pairs as diameter, then all triples for circumcircle
    // Keep the smallest valid circle
    
    Circle bestCircle;
    bestCircle.radius = std::numeric_limits<double>::max();
    
    int n = points.size();
    
    // Try all pairs (circle with points as diameter)
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            Circle c = circleFromTwoPoints(points[i], points[j]);
            if (c.radius < bestCircle.radius && circleEnclosesAllPoints(c, points))
            {
                bestCircle = c;
            }
        }
    }
    
    // Try all triples (circumcircle)
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            for (int k = j + 1; k < n; ++k)
            {
                Circle c = circleFromThreePoints(points[i], points[j], points[k]);
                if (c.radius > 0 && c.radius < bestCircle.radius && 
                    circleEnclosesAllPoints(c, points))
                {
                    bestCircle = c;
                }
            }
        }
    }
    
    return bestCircle;
}

Circle MinimumEnclosingCircle::circleFromTwoPoints(const QPointF &p1, const QPointF &p2)
{
    // Circle with p1-p2 as diameter
    QPointF center((p1.x() + p2.x()) / 2.0, (p1.y() + p2.y()) / 2.0);
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();
    double radius = std::sqrt(dx * dx + dy * dy) / 2.0;
    return Circle(center, radius);
}

Circle MinimumEnclosingCircle::circleFromThreePoints(const QPointF &p1, const QPointF &p2, const QPointF &p3)
{
    // Calculate circumcircle of triangle p1, p2, p3
    // Using the formula for circumcenter
    
    double ax = p1.x(), ay = p1.y();
    double bx = p2.x(), by = p2.y();
    double cx = p3.x(), cy = p3.y();
    
    double d = 2.0 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
    
    if (std::abs(d) < kEpsilon)
    {
        // Points are collinear, no valid circumcircle
        // Return the two-point circle of the farthest pair
        double d12 = (p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y());
        double d23 = (p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y());
        double d13 = (p3.x() - p1.x()) * (p3.x() - p1.x()) + (p3.y() - p1.y()) * (p3.y() - p1.y());
        
        if (d12 >= d23 && d12 >= d13)
        {
            return circleFromTwoPoints(p1, p2);
        }
        else if (d23 >= d13)
        {
            return circleFromTwoPoints(p2, p3);
        }
        else
        {
            return circleFromTwoPoints(p1, p3);
        }
    }
    
    double a2 = ax * ax + ay * ay;
    double b2 = bx * bx + by * by;
    double c2 = cx * cx + cy * cy;
    
    double ux = (a2 * (by - cy) + b2 * (cy - ay) + c2 * (ay - by)) / d;
    double uy = (a2 * (cx - bx) + b2 * (ax - cx) + c2 * (bx - ax)) / d;
    
    QPointF center(ux, uy);
    double radius = std::sqrt((ax - ux) * (ax - ux) + (ay - uy) * (ay - uy));
    
    return Circle(center, radius);
}

bool MinimumEnclosingCircle::circleEnclosesAllPoints(const Circle &c, const QList<QPointF> &points)
{
    for (const auto& p : points)
    {
        if (!c.contains(p, kEpsilon))
        {
            return false;
        }
    }
    return true;
}
