# ShotPlot - Group Size Calculation Algorithm

> Part of the [ShotPlot Design Documents](DESIGN.md)  
> **Version 0.10** | Last Updated: 2026-01-25

## Overview

This document details the mathematical algorithms at the heart of ShotPlot's group size calculations. **The smallest enclosing circle algorithm is the most critical component** of the application, as it directly determines the accuracy of all group size measurements.

## Table of Contents

- [Key Architecture Principle](#key-architecture-principle-pixel-based-storage)
- [Visual Explanation](#visual-explanation-of-key-measurements)
- [Group Size Terminology](#core-group-size-metrics)
- [Smallest Enclosing Circle Algorithm](#the-smallest-enclosing-circle-algorithm)
- [80%/90% Group Calculations](#computing-80-and-90-group-sizes)
- [Other Statistics](#other-statistical-calculations)
- [Numerical Stability](#implementation-notes-numerical-stability)

---

## Key Architecture Principle: Pixel-Based Storage

**All calculations and storage use pixel coordinates (fractional/float precision):**
- Shot positions: (x, y) in pixels
- Group circle centers: (x, y) in pixels  
- Group circle radii: pixels
- Mean radius: pixels
- Standard deviation: pixels

**Physical measurements (inches, MOA, mils) are calculated ONLY for display:**
```
Pixels → Inches:  value / pixelsPerInch
Inches → MOA:     (inches / distanceYards) * 95.5
Inches → Mils:    (inches / (distanceYards * 36)) * 1000
```

**Benefits:**
- ✓ Ground truth (pixels) never changes
- ✓ Re-calibration instantly updates all displays
- ✓ No redundant storage of derived values
- ✓ Single source of truth for all measurements

---

## Visual Explanation of Key Measurements

To clarify the different measurements that ShotPlot calculates:

```
    Shot 1 •           • Shot 2        Legend:
                                       • = Shot impact center
                                       ⊕ = Centroid of all shots
              ⊕ Centroid               ◯ = Full group circle (100%)
                                       ⊙ = 80% group circle
    
    Shot 3 •           • Shot 4        
                                       
              • Shot 5                 


    Full Group Circle (100%):         The smallest circle that encloses
    ◯─────────────────◯                ALL 5 shots. Diameter = Group Size.

    80% Group Circle:                  The smallest circle that encloses
    ⊙───────────⊙                      4 out of 5 shots (80%).

    Mean Radius: From centroid ⊕ to each shot, averaged.
    
    Note: The center of the full group circle may not exactly
    coincide with the centroid ⊕.
```

**Key Distinctions:**
1. **Group Size Definition**: The diameter of the smallest enclosing circle. This is NOT simply the distance between the two furthest shots (extreme spread), but rather the optimal circle that contains all (or N%) of the shot centers.
2. **80% Group ≠ Statistical Percentile**: The 80% group is the smallest circle containing 80% of shots. It's a geometric optimization, not a statistical distribution measure.
3. **Group Circle Center ≠ Centroid**: The center of the smallest enclosing circle may differ slightly from the centroid (geometric center of all shots). The centroid is used only for calculating mean radius.

---

## Core Group Size Metrics

### Understanding Group Size Terminology

**CRITICAL DISTINCTION**: Group size in shooting sports has a specific meaning that differs from simple statistical measures:

1. **Full Group Size (100% Group)**: The **diameter** of the smallest circle that encloses ALL shot impact centers. This is NOT the distance from the centroid to the furthest shot, but rather the diameter of the minimal enclosing circle.

2. **80% Group Size (or 90% Group)**: The **diameter** of the smallest circle that encloses 80% (or 90%) of the shots. For a 10-shot group, the 80% group size is the diameter of the smallest circle containing 8 shots.

### Data Structure for Group Circles

Each group circle (full, 80%, 90%, etc.) is represented by:

```cpp
struct GroupCircle {
    Point2D center;           // (x, y) coordinates of circle center in PIXELS
    double radiusPixels;      // radius in PIXELS (fractional/float)
    vector<int> shotIndices;  // indices of shots enclosed by this circle
};
```

**Storage**: Radius and center stored in pixel coordinates (float precision)  
**Display**: `groupSizeDiameter = (2 × radiusPixels) / pixelsPerInch` to get inches  
Then convert to MOA/mils using target distance if needed

---

## The Smallest Enclosing Circle Algorithm

This is the heart of ShotPlot's group size calculations. The algorithm finds the minimum bounding circle for a set of points.

### Mathematical Foundation

**Key Property**: The smallest circle enclosing a set of points is uniquely determined by at most 3 points on its boundary.

- If determined by **2 points**: Those points form a diameter of the circle
- If determined by **3 points**: The circle is the circumcircle of the triangle formed by those points (only if the triangle is acute or right; if obtuse, the enclosing circle is determined by the two points forming the longest side)

### Algorithm Overview

For a set of N points, the naive algorithm:

1. Check all pairs of points (N choose 2) as potential diameters
2. Check all triples of points (N choose 3) as potential circumcircles
3. For each candidate circle, verify it encloses all points
4. Return the valid circle with the smallest radius

**Complexity**: O(N³) for naive implementation. For typical shooting applications (5-20 shots), this is perfectly acceptable.

**Advanced Alternative**: Welzl's algorithm runs in O(N) expected time using randomized incremental construction, but is more complex to implement. Start with the naive approach.

### Step 1: Circle from Two Points

Given two points P₁ = (x₁, y₁) and P₂ = (x₂, y₂), they define a circle with those points as diameter endpoints:

```cpp
Circle circleFromTwoPoints(Point2D p1, Point2D p2) {
    Circle c;
    // Center is midpoint of the two points
    c.center.x = (p1.x + p2.x) / 2.0;
    c.center.y = (p1.y + p2.y) / 2.0;
    
    // Radius is half the distance between points
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    c.radius = sqrt(dx*dx + dy*dy) / 2.0;
    
    return c;
}
```

### Step 2: Circle from Three Points (Circumcircle)

Given three points P₁, P₂, P₃, we find the unique circle passing through all three (if they're not collinear).

**Method 1: Perpendicular Bisector Intersection**

The center of the circumcircle is at the intersection of the perpendicular bisectors of any two sides of the triangle.

```cpp
Circle circleFromThreePoints(Point2D p1, Point2D p2, Point2D p3) {
    Circle c;
    
    // Calculate the perpendicular bisector of segment p1-p2
    double midX_12 = (p1.x + p2.x) / 2.0;
    double midY_12 = (p1.y + p2.y) / 2.0;
    double dx_12 = p2.x - p1.x;
    double dy_12 = p2.y - p1.y;
    
    // Calculate the perpendicular bisector of segment p2-p3
    double midX_23 = (p2.x + p3.x) / 2.0;
    double midY_23 = (p2.y + p3.y) / 2.0;
    double dx_23 = p3.x - p2.x;
    double dy_23 = p3.y - p2.y;
    
    // Solve for intersection point using parametric form
    double det = dx_12 * dy_23 - dx_23 * dy_12;
    
    if (fabs(det) < 1e-10) {
        // Points are collinear - no valid circle
        c.radius = -1;  // Invalid circle indicator
        return c;
    }
    
    // Using determinant method for center calculation
    double a = midX_23 - midX_12;
    double b = midY_23 - midY_12;
    
    double t = (a * dy_23 - b * dx_23) / det;
    
    c.center.x = midX_12 - t * dy_12;
    c.center.y = midY_12 + t * dx_12;
    
    // Calculate radius as distance from center to any point
    double dx = p1.x - c.center.x;
    double dy = p1.y - c.center.y;
    c.radius = sqrt(dx*dx + dy*dy);
    
    return c;
}
```

**Method 2: Determinant Formula** (Alternative, more compact)

```cpp
Circle circleFromThreePoints_Determinant(Point2D p1, Point2D p2, Point2D p3) {
    Circle c;
    
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = p3.x, y3 = p3.y;
    
    // Calculate determinant for collinearity check
    double D = 2.0 * (x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2));
    
    if (fabs(D) < 1e-10) {
        // Points are collinear
        c.radius = -1;
        return c;
    }
    
    // Calculate center coordinates using determinant formulas
    double ux = ((x1*x1 + y1*y1)*(y2 - y3) + 
                 (x2*x2 + y2*y2)*(y3 - y1) + 
                 (x3*x3 + y3*y3)*(y1 - y2));
    double uy = ((x1*x1 + y1*y1)*(x3 - x2) + 
                 (x2*x2 + y2*y2)*(x1 - x3) + 
                 (x3*x3 + y3*y3)*(x2 - x1));
    
    c.center.x = ux / D;
    c.center.y = uy / D;
    
    // Calculate radius
    double dx = x1 - c.center.x;
    double dy = y1 - c.center.y;
    c.radius = sqrt(dx*dx + dy*dy);
    
    return c;
}
```

### Step 3: Check if Circle Encloses All Points

```cpp
bool circleEnclosesAllPoints(Circle c, vector<Point2D> points, double epsilon = 1e-6) {
    for (const auto& p : points) {
        double dx = p.x - c.center.x;
        double dy = p.y - c.center.y;
        double distSquared = dx*dx + dy*dy;
        double radiusSquared = c.radius * c.radius;
        
        // Point is outside if distance > radius (with small epsilon for numerical tolerance)
        if (distSquared > radiusSquared + epsilon) {
            return false;
        }
    }
    return true;
}
```

### Step 4: Find Smallest Enclosing Circle (Main Algorithm)

```cpp
GroupCircle findSmallestEnclosingCircle(vector<Point2D> points) {
    int n = points.size();
    GroupCircle result;
    result.radius = INFINITY;
    
    if (n == 0) {
        result.radius = 0;
        return result;
    }
    
    if (n == 1) {
        result.center = points[0];
        result.radius = 0;
        result.shotIndices = {0};
        return result;
    }
    
    // Try all pairs of points as potential diameters
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            Circle c = circleFromTwoPoints(points[i], points[j]);
            
            if (circleEnclosesAllPoints(c, points)) {
                if (c.radius < result.radius) {
                    result.center = c.center;
                    result.radius = c.radius;
                    // Populate shotIndices with all point indices
                    result.shotIndices.clear();
                    for (int k = 0; k < n; k++) {
                        result.shotIndices.push_back(k);
                    }
                }
            }
        }
    }
    
    // Try all triples of points as potential circumcircles
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                Circle c = circleFromThreePoints(points[i], points[j], points[k]);
                
                if (c.radius < 0) continue;  // Skip invalid (collinear) circles
                
                if (circleEnclosesAllPoints(c, points)) {
                    if (c.radius < result.radius) {
                        result.center = c.center;
                        result.radius = c.radius;
                        result.shotIndices.clear();
                        for (int m = 0; m < n; m++) {
                            result.shotIndices.push_back(m);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}
```

### Optimization: Check Acute/Obtuse Triangle

For three points forming a triangle, check if the triangle is obtuse. If it is, the smallest enclosing circle is determined by the longest side (the two-point case):

```cpp
bool isObtuseTriangle(Point2D p1, Point2D p2, Point2D p3) {
    // Calculate squared side lengths
    double a2 = distSquared(p2, p3);  // opposite to p1
    double b2 = distSquared(p1, p3);  // opposite to p2
    double c2 = distSquared(p1, p2);  // opposite to p3
    
    // Check if any angle is obtuse using law of cosines
    // Angle at p1 is obtuse if a² > b² + c²
    if (a2 > b2 + c2) return true;
    if (b2 > a2 + c2) return true;
    if (c2 > a2 + b2) return true;
    
    return false;
}
```

---

## Computing 80% and 90% Group Sizes

To find the 80% (or 90%) group size:

1. Determine how many shots to include:
   - For 10 shots: 80% = 8 shots, 90% = 9 shots
   - For 5 shots: 80% = 4 shots, 90% = 5 shots (rounded)

2. Generate all combinations of that many shots

3. For each combination, find the smallest enclosing circle

4. Return the combination with the smallest circle

```cpp
GroupCircle find80PercentGroup(vector<Point2D> allShots) {
    int n = allShots.size();
    int numToInclude = (int)round(n * 0.8);  // 80% of shots
    
    GroupCircle bestCircle;
    bestCircle.radius = INFINITY;
    
    // Generate all combinations of numToInclude shots from n total shots
    vector<int> combination(numToInclude);
    
    // Use recursive combination generation or iterative approach
    // For each combination:
    function<void(int, int)> generateCombinations = [&](int start, int index) {
        if (index == numToInclude) {
            // We have a complete combination
            vector<Point2D> subset;
            for (int i = 0; i < numToInclude; i++) {
                subset.push_back(allShots[combination[i]]);
            }
            
            GroupCircle circle = findSmallestEnclosingCircle(subset);
            
            if (circle.radius < bestCircle.radius) {
                bestCircle = circle;
            }
            return;
        }
        
        for (int i = start; i < n; i++) {
            combination[index] = i;
            generateCombinations(i + 1, index + 1);
        }
    };
    
    generateCombinations(0, 0);
    return bestCircle;
}
```

**Note**: For large shot counts, the number of combinations grows rapidly (C(n,k)). For typical shooting groups (5-20 shots), this is still computationally feasible.

---

## Other Statistical Calculations

### Group Center (Centroid)

The geometric center is the mean position of all shots:

```cpp
Point2D calculateCentroid(vector<Point2D> points) {
    Point2D center = {0, 0};
    for (const auto& p : points) {
        center.x += p.x;
        center.y += p.y;
    }
    center.x /= points.size();
    center.y /= points.size();
    return center;
}
```

### Mean Radius (MR)

Average distance of all shots from the centroid (in pixels):

```cpp
double calculateMeanRadiusPixels(vector<Point2D> points, Point2D centroid) {
    double sumDistances = 0;
    for (const auto& p : points) {
        double dx = p.x - centroid.x;
        double dy = p.y - centroid.y;
        sumDistances += sqrt(dx*dx + dy*dy);  // Distance in pixels
    }
    return sumDistances / points.size();  // Mean radius in pixels
}

// Convert to inches for display
double meanRadiusInches = meanRadiusPixels / pixelsPerInch;
```

### Standard Deviation

Standard deviation of shot distances from centroid (in pixels):

```cpp
double calculateStdDevPixels(vector<Point2D> points, Point2D centroid, double meanRadiusPixels) {
    double sumSquaredDiff = 0;
    for (const auto& p : points) {
        double dx = p.x - centroid.x;
        double dy = p.y - centroid.y;
        double distPixels = sqrt(dx*dx + dy*dy);  // Distance in pixels
        double diff = distPixels - meanRadiusPixels;
        sumSquaredDiff += diff * diff;
    }
    return sqrt(sumSquaredDiff / (points.size() - 1));  // Std dev in pixels
}

// Convert to inches for display
double stdDevInches = stdDevPixels / pixelsPerInch;
```

### Display Unit Conversions

All statistics are stored in pixels and converted for display:

```cpp
// 1. Convert pixels to inches
double valueInches = valuePixels / pixelsPerInch;

// 2. Convert inches to MOA
double valueToMOA(double valueInches, double distanceYards) {
    // 1 MOA = 1.047 inches at 100 yards (exact)
    // Shooters' approximation: 1 MOA = 1 inch at 100 yards
    
    // Exact calculation:
    double valueAt100Yards = (valueInches / distanceYards) * 100.0;
    return valueAt100Yards / 1.047;
    
    // Or simplified approximation (common in shooting):
    // return (valueInches / distanceYards) * 100.0;
}

// 3. Convert inches to mils (milliradians)
double valueToMils(double valueInches, double distanceYards) {
    // Convert distance to inches for consistent units
    double distanceInches = distanceYards * 36.0;
    
    // 1 mil = 1/1000 of distance
    return (valueInches / distanceInches) * 1000.0;
}

// Complete workflow example:
double radiusPixels = fullGroupCircle.radiusPixels;
double diameterPixels = 2.0 * radiusPixels;
double diameterInches = diameterPixels / pixelsPerInch;
double diameterMOA = valueToMOA(diameterInches, distanceYards);
double diameterMils = valueToMils(diameterInches, distanceYards);
```

**UI Layer Responsibility:**
- Statistics panel performs all conversions for display
- User can toggle between inches/MOA/mils views
- Underlying pixel data never changes

---

## Summary of Key Metrics

For the statistics panel, display (all converted from pixel storage):

1. **Number of shots marked** / expected
2. **Full Group Size (100%)**: Diameter in inches/MOA/mils
3. **80% Group Size**: Diameter in inches/MOA/mils 
4. **90% Group Size**: Diameter in inches/MOA/mils
5. **Mean Radius (MR)**: In inches (converted from pixels)
6. **Standard Deviation**: In inches (converted from pixels)
7. **Angular measurements**: Selectable display (inches, MOA, or mils)

**Storage vs Display:**
- **Stored**: All values in pixel coordinates (fractional/float)
- **Displayed**: Converted to physical units using `pixelsPerInch` scale factor
- **Benefit**: Re-calibration automatically updates all displayed values

**Note**: The centroid (geometric center) is calculated for mean radius and standard deviation computations, but is not displayed as a separate metric. It may be visualized on the target for reference.

---

## Implementation Notes: Numerical Stability

When implementing the smallest enclosing circle algorithm, pay attention to numerical precision:

### Floating Point Considerations

1. **Collinearity Detection**: Use epsilon tolerance when checking if three points are collinear via determinant:
   ```cpp
   const double EPSILON = 1e-10;
   if (fabs(determinant) < EPSILON) { /* collinear */ }
   ```

2. **Point-on-Circle Tests**: When verifying if a point lies on or inside a circle, use epsilon for boundary comparisons:
   ```cpp
   double distSquared = dx*dx + dy*dy;
   bool isInside = (distSquared <= radiusPixels*radiusPixels + EPSILON);
   ```

3. **Avoid Premature Sqrt**: Use squared distances when possible to avoid floating point errors from square root operations.

4. **Unit Consistency**: 
   - **Storage & Calculations**: Always use PIXELS (fractional/float)
   - **Display**: Convert to inches/MOA/mils only in the UI layer
   - This ensures if user changes scale factor, all statistics remain valid

### Recommended C++ Data Types

```cpp
// Use double precision for all geometric calculations
struct Point2D {
    double x;  // Always in pixels (float precision)
    double y;  // Always in pixels (float precision)
};

struct Circle {
    Point2D center;     // Pixels
    double radius;      // Pixels
};

struct GroupCircle {
    Point2D center;               // Pixels (float)
    double radiusPixels;          // Pixels (float)
    std::vector<int> shotIndices;
    
    // Convert to physical units for display
    double diameterInches(double pixelsPerInch) const {
        return (2.0 * radiusPixels) / pixelsPerInch;
    }
    
    double diameterMOA(double pixelsPerInch, double distanceYards) const {
        double inches = diameterInches(pixelsPerInch);
        return (inches / distanceYards) * 95.5;  // or / 1.047 for exact
    }
    
    double diameterMils(double pixelsPerInch, double distanceYards) const {
        double inches = diameterInches(pixelsPerInch);
        return (inches / (distanceYards * 36.0)) * 1000.0;
    }
};
```

### Edge Cases to Handle

1. **Zero shots**: Return empty result
2. **One shot**: Return circle with radius = 0 centered on that shot
3. **Two shots**: Always diameter case
4. **All shots at same location**: Return circle with radius = 0
5. **Collinear shots**: Handle gracefully (may have multiple valid solutions)
6. **Very tight groups**: Maintain precision even for sub-MOA groups (<0.1 inches)
7. **Very large groups**: Handle groups spanning several inches without overflow

## Test Cases

### Unit Tests Required

- **Smallest enclosing circle algorithm** (CRITICAL):
  - Two points: verify diameter calculation
  - Three points (acute triangle): verify circumcircle calculation
  - Three points (obtuse triangle): verify uses longest side as diameter
  - Four points in square configuration: verify correct circle
  - Five points with known solution: verify against reference implementation
  - Collinear points: verify graceful handling
  - Edge case: duplicate points
  - Edge case: single point (radius = 0)
- **80%/90% group calculations**: verify correct combination selection
- **Mean radius calculation**: verify average distance from centroid
- **Standard deviation**: verify calculation from centroid
- Unit conversions (pixels/inches, inches/MOA, inches/mils)

---

**See Also:**
- [Main Design Document](DESIGN.md)
- [Technical Architecture](ARCHITECTURE.md)
- [UI & Workflows](UI_WORKFLOWS.md)
