# ShotPlot - Technical Architecture

> Part of the [ShotPlot Design Documents](DESIGN.md)  
> **Version 0.10** | Last Updated: 2026-01-25

## Overview

This document details the technical architecture, class structure, file format, and build configuration for ShotPlot.

## Table of Contents

- [Project Structure](#project-structure)
- [Key Classes](#key-classes)
- [Application State Management](#application-state-management)
- [Data Flow](#data-flow)
- [File Format (.spz)](#file-format-spz)
- [Build System (CMake)](#build-system-cmake)
- [Dependencies](#dependencies)

---

## Project Structure

```
ShotPlot/
├── CMakeLists.txt              # Root CMake configuration
├── src/
│   ├── main.cpp                # Application entry point
│   ├── MainWindow.cpp/.h       # Main application window with state management
│   ├── Widgets/
│   │   ├── WorkflowToolbar     # Pipeline visualization toolbar
│   │   ├── PerStateToolbar     # State-specific controls toolbar
│   │   ├── TargetView          # QGraphicsView-based target display
│   │   ├── StatisticsPanel     # Statistics display widget (state-dependent)
│   │   ├── MetadataEditor      # Metadata input form
│   │   └── CaliberDialog       # Modal dialog for caliber selection
│   ├── Graphics/
│   │   ├── TargetScene         # QGraphicsScene for target
│   │   ├── ImpactGlyphItem     # Segmented circle glyph for impacts
│   │   ├── POAGlyphItem        # X-shaped point of aim marker
│   │   ├── GroupCircleItem     # Visual group boundary circle
│   │   ├── ScaleLineItem       # Green reference line for calibration
│   │   └── CustomCursors       # Custom cursor rendering (circle, +, X)
│   ├── States/
│   │   ├── WorkflowState       # Base class for workflow states
│   │   ├── SetCaliberState     # State 1: Caliber selection
│   │   ├── ScaleFactorState    # State 2: Interactive scale calibration
│   │   ├── POAState            # State 3: Point of aim marking
│   │   ├── MarkImpactsState    # State 4: Shot marking and editing
│   │   └── VisualizationState  # State 5: (future) Export config
│   ├── Core/
│   │   ├── ShotGroupDocument   # Main data model (all data in pixels)
│   │   ├── Shot                # Individual shot data (pixel coordinates)
│   │   ├── GroupCircle         # Circle data structure (center + radius in pixels)
│   │   ├── Statistics          # Statistical calculations engine (works in pixels)
│   │   ├── MinimumEnclosingCircle # Smallest circle algorithm implementation
│   │   ├── Calibration         # Stores pixelsPerInch scale factor
│   │   ├── UnitConverter       # Converts pixels to inches/MOA/mils for display
│   │   └── UndoCommands        # QUndoCommand subclasses for undo/redo
│   ├── IO/
│   │   ├── DocumentSerializer  # Save/load .spz files (ZIP format)
│   │   ├── ImageExporter       # Export annotated images with plaque
│   │   ├── PlaqueRenderer      # Render statistics plaque with format string
│   │   └── FormatStringParser  # Parse and substitute placeholders
│   └── Utils/
│       └── Units               # Unit conversion utilities
├── tests/                      # Unit tests (CMake test target)
│   ├── CMakeLists.txt
│   └── test_*.cpp
├── resources/                  # Qt resources (.qrc)
│   ├── icons/
│   └── resources.qrc
├── docs/                       # Documentation
│   ├── DESIGN.md
│   ├── ALGORITHM.md
│   ├── ARCHITECTURE.md
│   └── UI_WORKFLOWS.md
├── packaging/                  # Platform-specific packaging scripts
│   ├── windows/                # NSIS/WiX installer scripts
│   ├── macos/                  # DMG creation scripts
│   └── linux/                  # .deb packaging files
├── LICENSE
└── README.md
```

---

## Key Classes

### Core Data Model

#### ShotGroupDocument Class

The `ShotGroupDocument` class is the central data model for a ShotPlot session. It stores all data in pixel coordinates and provides the interface for state management and calculations.

**Class Definition:**

```cpp
class ShotGroupDocument : public QObject {
    Q_OBJECT

public:
    // Construction
    ShotGroupDocument(QObject* parent = nullptr);
    ~ShotGroupDocument();

    // ===== Core Data (Persisted to .spz) =====
    
    // Image data
    QImage targetImage() const;
    void setTargetImage(const QImage& image);
    
    // Workflow state data
    double bulletDiameter() const;              // inches
    void setBulletDiameter(double diameter);
    bool hasCaliberSet() const;
    
    double pixelsPerInch() const;
    void setPixelsPerInch(double ppi);
    bool hasScaleFactorSet() const;
    
    QPointF pointOfAim() const;                 // pixel coordinates (optional)
    void setPointOfAim(const QPointF& poa);
    void clearPointOfAim();
    bool hasPointOfAimSet() const;
    
    // Session parameters
    double targetDistance() const;               // numeric value
    void setTargetDistance(double distance);
    
    enum class DistanceUnit { Yards, Meters };
    DistanceUnit distanceUnit() const;
    void setDistanceUnit(DistanceUnit unit);
    
    QDate sessionDate() const;
    void setSessionDate(const QDate& date);
    
    // Impacts (shots)
    int impactCount() const;
    QList<ShotImpact> impacts() const;
    ShotImpact impactAt(int index) const;
    void addImpact(const ShotImpact& impact);
    void removeImpact(int index);
    void clearImpacts();
    void replaceImpacts(const QList<ShotImpact>& impacts);  // For undo/redo
    
    // Metadata
    QString firearm() const;
    void setFirearm(const QString& firearm);
    
    QString ammunition() const;
    void setAmmunition(const QString& ammo);
    
    QString notes() const;
    void setNotes(const QString& notes);
    
    // Visualization settings
    bool showFullGroupCircle() const;
    void setShowFullGroupCircle(bool show);
    
    bool show80PercentCircle() const;
    void setShow80PercentCircle(bool show);
    
    bool show90PercentCircle() const;
    void setShow90PercentCircle(bool show);
    
    bool showPointOfAim() const;
    void setShowPointOfAim(bool show);
    
    // Statistics plaque configuration
    struct PlaqueConfig {
        bool enabled = false;
        int x = 50;
        int y = 50;
        int width = 300;
        int height = 200;
        QString title = "Shot Group Statistics";
        QString formatString;
    };
    
    PlaqueConfig plaqueConfig() const;
    void setPlaqueConfig(const PlaqueConfig& config);
    
    // ===== Statistics (Not Persisted) =====
    
    // Statistics (calculated by StatisticsCalculator, stored in ShotGroupDocument)
    const Statistics& statistics() const;  // Returns current statistics
    
    // ===== File Management =====
    
    QString filePath() const;  // Document moniker
    void setFilePath(const QString& path);
    bool hasFilePath() const;
    
    bool isDirty() const;
    void setDirty(bool dirty);
    
    // ===== Workflow State Queries =====
    
    bool canEnableScaleFactorState() const;   // has caliber
    bool canEnablePointOfAimState() const;    // has scale factor
    bool canEnableMarkImpactsState() const;   // has scale factor
    bool canEnableVisualizationState() const; // has >= 1 impact
    
    // ===== Serialization =====
    
    bool saveToFile(const QString& filePath, QString* errorMsg = nullptr);
    bool loadFromFile(const QString& filePath, QString* errorMsg = nullptr);
    
    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& json, QString* errorMsg = nullptr);

signals:
    // Emitted when data changes (for UI updates)
    void dataChanged();
    void dirtyChanged(bool dirty);
    void impactsChanged();
    void statisticsChanged();
    void visualizationSettingsChanged();

private:
    // ===== Private Data Members =====
    
    // Image
    QImage m_targetImage;
    
    // Workflow state data
    double m_bulletDiameter = 0.0;         // 0 = not set
    double m_pixelsPerInch = 0.0;          // 0 = not set
    QPointF m_pointOfAim;                  // invalid QPointF = not set
    bool m_hasPointOfAim = false;
    
    // Session parameters
    double m_targetDistance = 100.0;       // default 100 yards
    DistanceUnit m_distanceUnit = DistanceUnit::Yards;
    QDate m_sessionDate;                   // current date by default
    
    // Impacts
    QList<ShotImpact> m_impacts;
    int m_nextImpactId = 1;
    
    // Metadata
    QString m_firearm;
    QString m_ammunition;
    QString m_notes;
    
    // Visualization settings
    bool m_showFullGroupCircle = true;
    bool m_show80PercentCircle = false;
    bool m_show90PercentCircle = false;
    bool m_showPointOfAim = true;
    PlaqueConfig m_plaqueConfig;
    
    // Statistics
    Statistics m_statistics;
    
    // File management
    QString m_filePath;
    bool m_isDirty = false;
};
```

#### ShotImpact Structure

Represents a single shot impact:

```cpp
struct ShotImpact {
    int id;                // Unique identifier
    double x;              // Pixel coordinate (fractional/float)
    double y;              // Pixel coordinate (fractional/float)
    QString notes;         // Optional per-shot notes (rarely used)
    
    ShotImpact(int id, double x, double y, const QString& notes = QString())
        : id(id), x(x), y(y), notes(notes) {}
    
    QPointF position() const { return QPointF(x, y); }
    
    // Convenience method for display in physical units
    QPointF physicalPosition(double pixelsPerInch) const {
        return QPointF(x / pixelsPerInch, y / pixelsPerInch);
    }
};
```

**Design Rationale:**

1. **All coordinates in pixels**: Ground truth, never changes
2. **Clear state queries**: `hasCaliberSet()`, `canEnableMarkImpactsState()`, etc.
3. **Eager statistics updates**: Recalculated immediately when impacts or scale factor change
4. **Dirty flag**: Tracks unsaved changes for file operations
5. **Qt signals**: Notify UI components when data changes
6. **Separation of concerns**: ShotGroupDocument stores data, StatisticsCalculator service performs calculations

**statistics() Implementation Pattern:**
```cpp
const Statistics& ShotGroupDocument::statistics() const {
    return m_statistics;
}

void ShotGroupDocument::updateStatistics() {
    m_statistics = StatisticsCalculator::calculate(m_impacts);
    emit statisticsChanged();
}
```

Statistics are updated immediately when data changes (impacts, scale factor), not lazily on access. This simplifies the design and matches the access pattern - UI always needs updated statistics after any change. ShotGroupDocument doesn't know *how* to calculate statistics, it just knows *when* to update them.

#### GroupCircle
Each group circle (full, 80%, 90%, etc.) is represented by:

```cpp
struct GroupCircle {
    Point2D center;           // (x, y) coordinates of circle center in PIXELS
    double radiusPixels;      // radius in PIXELS (fractional/float)
    vector<int> shotIndices;  // indices of shots enclosed by this circle
};
```

See [ALGORITHM.md](ALGORITHM.md) for detailed calculations.

### Calculation Engine

#### StatisticsCalculator Service

**Stateless service class** that calculates group statistics from shot data. Operates entirely in pixel space.

```cpp
class StatisticsCalculator {
public:
    // Main calculation entry point
    static Statistics calculate(const QList<ShotImpact>& impacts);
    
    // Individual calculations
    static QPointF calculateCentroid(const QList<QPointF>& points);
    static double calculateMeanRadius(const QList<QPointF>& points, const QPointF& centroid);
    static double calculateStandardDeviation(const QList<QPointF>& points, 
                                              const QPointF& centroid,
                                              double meanRadius);
    
    static GroupCircle findFullGroupCircle(const QList<QPointF>& points);
    static GroupCircle find80PercentGroupCircle(const QList<QPointF>& points);
    static GroupCircle find90PercentGroupCircle(const QList<QPointF>& points);
    
private:
    // Not instantiable - static methods only
    StatisticsCalculator() = delete;
};
```

**Statistics Structure (returned by calculator):**
```cpp
struct Statistics {
    double meanRadiusPixels = 0.0;
    double standardDeviationPixels = 0.0;
    GroupCircle fullGroupCircle;
    GroupCircle group80Circle;
    GroupCircle group90Circle;
    bool valid = false;  // true if calculated with >= 2 impacts
};
```

**Usage Pattern:**
```cpp
// In ShotGroupDocument class
const Statistics& ShotGroupDocument::statistics() const {
    return m_statistics;
}

void ShotGroupDocument::updateStatistics() {
    m_statistics = StatisticsCalculator::calculate(m_impacts);
    emit statisticsChanged();
}

// Called when impacts or scale factor change:
void ShotGroupDocument::addImpact(const ShotImpact &impact) {
    m_impacts.append(impact);
    updateStatistics();  // Immediate recalculation
    setDirty(true);
    emit impactsChanged();
}
```

**Benefits:**
- **Single Responsibility**: ShotGroupDocument stores data, StatisticsCalculator computes
- **Testability**: Can test calculator independently with known inputs
- **Stateless**: No state management, just pure functions
- **Reusability**: Could calculate statistics for any list of impacts

#### MinimumEnclosingCircle

**Helper class** used by StatisticsCalculator. Implements the smallest enclosing circle algorithm.

```cpp
class MinimumEnclosingCircle {
public:
    // Find smallest circle containing all points
    static Circle findSmallestEnclosingCircle(const QList<QPointF>& points);
    
    // Helper functions
    static Circle circleFromTwoPoints(const QPointF& p1, const QPointF& p2);
    static Circle circleFromThreePoints(const QPointF& p1, const QPointF& p2, const QPointF& p3);
    static bool circleEnclosesAllPoints(const Circle& c, const QList<QPointF>& points);
    
private:
    MinimumEnclosingCircle() = delete;
};
```

See [ALGORITHM.md](ALGORITHM.md) for detailed implementation with pseudocode.

#### UnitConverter
Utility class for display conversions:
- Pixels → Inches (using pixelsPerInch)
- Inches → MOA (using distance)
- Inches → Mils (using distance)
- Handles both exact and approximate MOA calculations

#### UndoCommands
QUndoCommand subclasses for undo/redo functionality:

**AddImpactCommand:**
- Records: ShotImpact to add (position, ID)
- Undo: Removes the impact from document
- Redo: Re-adds the impact

**RemoveImpactCommand:**
- Records: ShotImpact to remove (full data)
- Undo: Restores the impact
- Redo: Removes it again

**ClearImpactsCommand:**
- Records: Entire impact list before clear
- Undo: Restores complete impact list (single operation)
- Redo: Clears list again
- No confirmation dialog needed (undoable)

**Implementation Note:**
- Consider storing complete impact list snapshots for undo/redo
- Each command triggers statistics recalculation on execute/undo
- QUndoStack manages command history
- Commands integrated with Edit menu (Undo/Redo actions)

### UI Components

#### MainWindow
Main application window:
- Central widget: TargetView (QGraphicsView)
- Right dock: StatisticsPanel
- Menu bar: File, Edit, View
- Toolbar: Quick access buttons
- Status bar: Messages and cursor position
- Document state management
- Drag and drop support:
  - Image files (PNG, JPEG, HEIC) → Trigger import
  - .spz files → Trigger load
  - Accepts drops on entire window
  - Visual feedback during drag

**Menu Structure:**

**File Menu:**
- Import... (Ctrl+I): Import image to create new document (always enabled)
- Load... (Ctrl+O): Open existing .spz file (always enabled)
- Save (Ctrl+S): Save to existing moniker, or Save As if none (enabled when dirty)
- Save As... (Ctrl+Shift+S): Save to new/different file (enabled when document loaded)
- ---
- Export Image...: Export annotated image (enabled when document loaded)
- ---
- Quit (Ctrl+Q): Exit application (warn if dirty, always enabled)

**Edit Menu:**
- Undo (Ctrl+Z): Undo last action (enabled when undo stack has items)
- Redo (Ctrl+Shift+Z or Ctrl+Y): Redo action (enabled when redo stack has items)
- ---
- Metadata...: Edit firearm, ammunition, notes (enabled when document loaded)

**View Menu:**
- Zoom In (Ctrl++): Zoom in on target view (enabled when document loaded)
- Zoom Out (Ctrl+-): Zoom out on target view (enabled when document loaded)
- Zoom Fit (Ctrl+0): Fit entire target in view (enabled when document loaded)
- ---
- Display Units submenu:
  - • Inches (application-wide setting)
  - • MOA (application-wide setting)
  - • MRAD (application-wide setting)

**State Management:**
- Enable/disable menu items based on document state
- Track dirty flag for unsaved changes
- Update window title with filename and dirty indicator
- Warn before destructive operations when dirty
- Display units preference persisted to QSettings

#### TargetView (QGraphicsView)

Custom QGraphicsView subclass for target image display and interaction.

**Configuration:**
```cpp
class TargetView : public QGraphicsView {
    Q_OBJECT
    
public:
    TargetView(QWidget* parent = nullptr);
    
    void setDocument(ShotGroupDocument* doc);
    void setWorkflowState(WorkflowState* state);
    
protected:
    // Event handling
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    
    // Custom painting for state overlays (e.g., green line)
    void paintEvent(QPaintEvent* event) override;
    
private:
    ShotGroupDocument* m_document = nullptr;
    WorkflowState* m_currentState = nullptr;
    
    // Pan/zoom state
    QPointF m_lastMousePos;
    QPointF m_mousePressPos;
    bool m_isPanning = false;
    double m_zoomFactor = 1.0;
};
```

**Initialization:**
```cpp
TargetView::TargetView(QWidget* parent) 
    : QGraphicsView(parent) 
{
    // Disable scroll bars - pan/zoom handled manually
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Smooth rendering
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Viewport settings
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    // Enable mouse tracking for cursor updates
    setMouseTracking(true);
    
    // Background
    setBackgroundBrush(QBrush(Qt::darkGray));
}
```

**Zoom Implementation:**
```cpp
void TargetView::wheelEvent(QWheelEvent* event) {
    // Available in all states (except Set Caliber, which is modal)
    const double scaleFactor = 1.15;
    
    if (event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
        m_zoomFactor *= scaleFactor;
    } else {
        // Zoom out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        m_zoomFactor /= scaleFactor;
    }
    
    event->accept();
}
```

**Pan Implementation:**
```cpp
void TargetView::mousePressEvent(QMouseEvent* event) {
    m_mousePressPos = event->pos();
    m_lastMousePos = event->pos();
    
    // Always pan with middle button or Ctrl+left
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier)) {
        m_isPanning = true;
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    
    // Left button - delegate to current state
    if (event->button() == Qt::LeftButton && m_currentState) {
        QPointF scenePos = mapToScene(event->pos());
        m_currentState->handleMousePress(scenePos);
        event->accept();
    }
}

void TargetView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        // Pan the view
        QPointF delta = mapToScene(event->pos()) - mapToScene(m_lastMousePos.toPoint());
        translate(delta.x(), delta.y());
        m_lastMousePos = event->pos();
        event->accept();
        return;
    }
    
    // Delegate to current state (e.g., for drawing green line)
    if (m_currentState) {
        QPointF scenePos = mapToScene(event->pos());
        m_currentState->handleMouseMove(scenePos);
    }
    
    event->accept();
}

void TargetView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isPanning) {
        m_isPanning = false;
        // Restore state-specific cursor
        if (m_currentState) {
            m_currentState->updateCursor();
        }
        event->accept();
        return;
    }
    
    // Check for click vs drag
    QPointF delta = event->pos() - m_mousePressPos;
    double dragDistance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    
    if (dragDistance < 5.0 && event->button() == Qt::LeftButton) {
        // Click - execute state action
        if (m_currentState) {
            QPointF scenePos = mapToScene(event->pos());
            m_currentState->handleMouseClick(scenePos);
        }
    } else if (dragDistance >= 5.0) {
        // Was a drag - already handled as pan
    }
    
    event->accept();
}
```

**Key Features:**
- **No scroll bars**: Pan via drag, zoom via wheel
- **Smooth zooming**: Centered on cursor position
- **Click vs drag**: 5-pixel threshold distinguishes actions
- **State delegation**: Mouse events forwarded to current WorkflowState
- **Always available**: Pan/zoom work in Scale Factor, POA, Mark Impacts, Visualization states

#### TargetScene (QGraphicsScene)

- Manages all QGraphicsItem objects (impacts, circles, POA, etc.)
- Contains target image as background item
- Coordinate system: Pixel coordinates, origin at top-left
- Updated by WorkflowState and Document changes

#### Workflow State Components

**WorkflowToolbar:**
- Visual pipeline showing: Caliber → Scale → POA → Mark → Viz
- Highlights current state
- Shows completed states (checkmark/different color)
- Shows enabled but not current states (clickable)
- Shows disabled/upcoming states (grayed out, not clickable)
- **Interactive**: Click enabled states to navigate
- **State enablement**:
  - Caliber: Always enabled (after document created)
  - Scale: Enabled after caliber set
  - POA: Enabled after scale set
  - Mark: Enabled after scale set (POA optional)
  - Viz: Enabled after >= 1 impact marked

**PerStateToolbar:**
- Changes content based on current workflow state
- Scale Factor state: Distance input field, instruction text
- Mark Impacts state: Clear button, instruction text
- POA state: Skip button, instruction text
- Dynamically rebuilt on state transitions

**WorkflowState (Base Class):**
- Abstract interface for each workflow state
- Methods:
  - `onEnter()`: Setup state (cursor, toolbars, side panel)
  - `onExit()`: Cleanup state
  - `handleMouseClick(pos)`: Process click events
  - `handleMouseMove(pos)`: Process move events (for line drawing, etc.)
  - `isComplete()`: Check if state has been completed (for enabling next state)
  - `nextState()`: Determine next state for auto-advancement
  - `paint(QPainter*)`: Optional custom painting (e.g., green line in Scale state)

**State Implementations:**
- SetCaliberState: Shows modal dialog, transitions based on document
- ScaleFactorState: Interactive two-point line drawing
- POAState: Single-click to mark point of aim
- MarkImpactsState: Click to add impacts, undo/redo support
- VisualizationState: (future) Configuration for export

#### CaliberDialog
Modal dialog for bullet diameter selection:
- Dropdown with common calibers
- Custom input option
- OK/Cancel buttons
- Triggered automatically on import or manually

#### StatisticsPanel
Real-time display widget (state-dependent content):
- Mark Impacts state: Shows group statistics, checkboxes
- Other states: May be hidden or show different content
- Converts from pixels to display units
- Updates live as markers are added/moved
- Toggle display mode (inches/MOA/mils)

### Graphics Items

**ImpactGlyphItem (QGraphicsItem):**
- Renders segmented circle with gaps at N, S, E, W cardinal directions
- Four arc segments creating crosshair effect
- Scaled to bullet diameter using calibration
- Center text: Shot number (white with outline)
- Line width: 2-3 pixels
- Color: Configurable (default: red or yellow)
- States: Normal, hover, selected

**POAGlyphItem (QGraphicsItem):**
- X-shaped marker rotated 45 degrees
- Color: Magenta (#FF00FF)
- Size: ~30-40 pixels (fixed, not scaled)
- Distinct from impact markers

**GroupCircleItem (QGraphicsItem):**
- Semi-transparent circle overlay
- Three types: Full (100%), 80%, 90%
- Different colors per type
- Toggle visibility via checkboxes
- Optional center point marker

**ScaleLineItem (QGraphicsItem):**
- Bright green line (#00FF00)
- Drawn from first click point to cursor during calibration
- Thick (3-5 pixels) for visibility
- Rendered with QPainter
- Removed after second click

**CustomCursors:**
- **Segmented Circle**: Four arcs with gaps at cardinals, scaled to bullet diameter
- **Circle-Plus**: Circle with + symbol (or just +)
- **X-Marker**: X rotated 45°, magenta
- Rendered as QPixmap cursors
- Set via `QWidget::setCursor()`

### Application State Management

#### Document Manager
Manages document lifecycle:
- Current document state (loaded/empty)
- Dirty flag tracking
- ShotGroupDocument moniker (file path)
- Warning dialogs for unsaved changes

**Document States:**
- **No ShotGroupDocument**: Application just opened, no ShotGroupDocument loaded
- **Clean ShotGroupDocument**: ShotGroupDocument loaded, no unsaved changes
- **Dirty ShotGroupDocument**: ShotGroupDocument loaded with unsaved modifications

**Operations:**
- Import image → Create new ShotGroupDocument (no moniker)
- Load .spz → Load existing ShotGroupDocument (with moniker)
- Save → Write to moniker (clear dirty flag)
- Save As → Write to new moniker (update moniker, clear dirty flag)
- Modify → Set dirty flag

#### Settings Manager (QSettings)
Manages application preferences using Qt's QSettings:

**Settings Keys:**

**Directories:**
- `directories/lastImport` - Last directory used for File → Import
- `directories/lastSaveLoad` - Last directory used for Save/Load operations

**Display Preferences:**
- `display/units` - Statistics display unit: "inches", "moa", or "mrad" (default: "inches")

**Usage:**
```cpp
QSettings settings("YourOrganization", "ShotPlot");

// Save directory after import
settings.setValue("directories/lastImport", selectedDirectory);

// Restore directory for import dialog
QString lastDir = settings.value("directories/lastImport", 
                                  QStandardPaths::PicturesLocation).toString();

// Save directory after save/load
settings.setValue("directories/lastSaveLoad", selectedDirectory);

// Restore directory for save/load dialog
QString lastSaveDir = settings.value("directories/lastSaveLoad",
                                      QStandardPaths::DocumentsLocation).toString();

// Display units
enum class DisplayUnit { Inches, MOA, MRAD };

DisplayUnit getDisplayUnit() {
    QString unitStr = settings.value("display/units", "inches").toString();
    if (unitStr == "moa") return DisplayUnit::MOA;
    if (unitStr == "mrad") return DisplayUnit::MRAD;
    return DisplayUnit::Inches;
}

void setDisplayUnit(DisplayUnit unit) {
    QString unitStr = "inches";
    if (unit == DisplayUnit::MOA) unitStr = "moa";
    if (unit == DisplayUnit::MRAD) unitStr = "mrad";
    settings.setValue("display/units", unitStr);
}
```

**Storage Location:**
- Windows: Registry (HKEY_CURRENT_USER\Software\YourOrganization\ShotPlot)
- macOS: ~/Library/Preferences/com.yourorganization.ShotPlot.plist
- Linux: ~/.config/YourOrganization/ShotPlot.conf

**Future Settings (Phase 2+):**
- Default bullet diameter presets
- Marker appearance preferences
- Export format defaults
- Recent files list

### I/O Components

#### DocumentSerializer
- Creates and reads .spz files (ZIP format)
- Uses QuaZip library
- Serializes to/from JSON
- Handles versioning

#### ImageExporter
- Renders annotated images
- Overlays enabled group circles
- Draws shot markers
- Renders statistics plaque
- Exports to PNG/JPEG/PDF

#### PlaqueRenderer
- Draws semi-transparent statistics box
- Renders formatted text
- Handles multi-line layout
- Configurable styling

#### FormatStringParser
- Parses format string with placeholders
- Substitutes values from ShotGroupDocument
- Handles unit conversions
- Error handling for unknown placeholders

---

## Data Flow

1. **Startup** → MainWindow created
2. **New Session** → CalibrationWizard launched
3. **Image Loaded** → Displayed in TargetScene via TargetView
4. **Reference Set** → Calibration object calculates pixels/inch
5. **Parameters Entered** → ShotGroupDocument object initialized
6. **Shot Marking** → User clicks → ShotMarkerItem added → ShotImpact object created
7. **Statistics Update** → Statistics recalculated → GroupCircle objects created → StatisticsPanel updated
8. **Visualization Config** → User toggles circle visibility, configures plaque → Saved to visualizationSettings
9. **Save** → DocumentSerializer creates .spz (ZIP with target.png + metadata.json including visualizationSettings)
10. **Export** → ImageExporter renders annotated image:
    - Draw enabled group circles
    - Draw shot markers
    - FormatStringParser substitutes placeholders
    - PlaqueRenderer draws statistics box with formatted text

---

## File Format (.spz)

ShotPlot uses a **ZIP-based container format** with the `.spz` extension (ShotPlot Zip). This provides compression, bundling of multiple files, and easy extensibility.

### ZIP Container Structure

```
example_session.spz  (ZIP file)
├── target.png                # Target image in PNG format (always this name)
└── metadata.json             # All session data and statistics
```

**Design Rationale:**
- **Single PNG format**: Simplifies loading/saving - always work with PNG (lossless)
- **No original image preservation**: Reduces file size and complexity
- **ZIP compression**: Automatic compression of PNG and JSON
- **Extensibility**: Easy to add more files in future (annotations, exports, etc.)

### File: target.png

- Always named `target.png`
- PNG format (lossless compression)
- Contains the target photograph
- Same image used for display and analysis
- Coordinate origin: Top-left corner (0, 0), standard image coordinates

### File: metadata.json

Complete session data in JSON format:

```json
{
  "version": "1.0",
  "created": "2026-01-25T10:30:00Z",
  "modified": "2026-01-25T10:45:00Z",
  
  "image": {
    "width": 3024,
    "height": 4032
  },
  
  "calibration": {
    "pixelsPerInch": 245.5
  },
  
  "pointOfAim": {
    "x": 1500,
    "y": 2000,
    "notes": "Center of bullseye"
  },
  
  "session": {
    "distance": 100,
    "distanceUnit": "yard",
    "bulletDiameter": 0.308,
    "numberOfShots": 5
  },
  
  "shots": [
    {
      "id": 1,
      "x": 1512,
      "y": 2016,
      "notes": ""
    },
    {
      "id": 2,
      "x": 1520,
      "y": 2010,
      "notes": ""
    }
  ],
  
  "statistics": {
    "meanRadiusPixels": 110.475,
    "standardDeviationPixels": 93.29,
    "fullGroupCircle": {
      "centerX": 1518.3,
      "centerY": 2012.1,
      "radiusPixels": 151.02,
      "shotIndices": [0, 1, 2, 3, 4]
    },
    "group80Circle": {
      "centerX": 1519.2,
      "centerY": 2008.5,
      "radiusPixels": 117.84,
      "shotIndices": [0, 1, 2, 4]
    },
    "group90Circle": {
      "centerX": 1518.1,
      "centerY": 2011.3,
      "radiusPixels": 137.48,
      "shotIndices": [0, 1, 2, 3, 4]
    }
  },
  
  "visualizationSettings": {
    "showFullGroupCircle": true,
    "show80PercentCircle": true,
    "show90PercentCircle": false,
    "statisticsPlaque": {
      "enabled": true,
      "x": 50,
      "y": 50,
      "width": 300,
      "height": 200,
      "title": "Shot Group Statistics",
      "formatString": "Distance: {distance} {distanceUnit}\nBullet: {bulletDiameter}\"\n\nFull Group: {fullGroup} {unit}\n80% Group: {group80} {unit}\nMean Radius: {meanRadius} {unit}\nStd Dev: {stdDev} {unit}\n\nMOA: {fullGroupMOA}"
    }
  },
  
  "metadata": {
    "firearm": "Remington 700 .308 Win with Vortex Viper PST 4-16x50",
    "ammunition": "Federal Gold Medal Match 168gr HPBT, Lot ABC123",
    "date": "2026-01-25",
    "notes": "First group with new load. Partly cloudy, 68°F, light wind from 3 o'clock."
  },
  
  "notes": "First group with new load"
}
```

### JSON Schema Details

#### Coordinate System
- **Origin**: Top-left corner of image (0, 0)
- **X-axis**: Increases to the right
- **Y-axis**: Increases downward
- **Units**: All coordinates stored in **pixels**
- **Conversion**: Physical measurements calculated on-the-fly using `pixelsPerInch`

#### Key Fields

**calibration.pixelsPerInch**
- Single float value: scale factor for the image
- Calculate from user's reference line during calibration
- User can re-calibrate at any time (fast operation)

**pointOfAim**
- Optional field (may be null/absent)
- User-specified intended point of impact
- Allows calculating accuracy vs. precision separately
- Stored in pixel coordinates

**session.bulletDiameter**
- Float value in inches (e.g., 0.308, 0.224, 0.264)
- Used for edge-to-edge measurements if needed
- Eliminates confusion between cartridge designations
- More precise than caliber strings

**shots**
- Array of shot impact locations
- ID for reference and sequencing
- Pixel coordinates only (x, y)
- Optional notes per shot

**statistics**
- Cached calculations from most recent analysis
- All circle centers in pixel coordinates
- Radii stored in pixels
- Can be recomputed on load if needed
- Only essential statistics persisted:
  - meanRadius (pixels)
  - standardDeviation (pixels)
  - fullGroupCircle (100% group)
  - group80Circle
  - group90Circle

**statistics.*.shotIndices**
- Zero-based indices into the shots array
- Indicates which shots are included in this circle
- For 80%/90% circles, shows which shots were selected
- Allows visualization of included/excluded shots

**metadata Fields**
- `firearm`: Single text string describing the firearm
- `ammunition`: Single text string describing the ammunition
- `date`: Date string (YYYY-MM-DD format)
- `notes`: Free-form text for any additional information

**visualizationSettings**
- Controls which group circles are displayed (full, 80%, 90%)
- Statistics plaque configuration for image export
  - Position (x, y) and size (width, height) in pixels on the target image
  - Title text for the plaque
  - Format string with placeholders for dynamic content
- These settings are preserved per-session and used for export

**Display Conversions (calculated at runtime):**
- **Inches**: `valuePixels / pixelsPerInch`
- **MOA**: `(valueInches / distanceYards) * 95.5` (or exact: `/ 1.047`)
- **Mils**: `(valueInches / (distanceYards * 36)) * 1000`
- Conversions happen in the UI layer, not in data storage

### File Operations

#### Saving a Session

1. Prepare `metadata.json` with current session data
2. Ensure `target.png` is available
3. Create ZIP archive:
   ```cpp
   // Qt example pseudocode
   QuaZip zip("session.spz");
   zip.open(QuaZip::mdCreate);
   
   // Add PNG
   QuaZipFile pngFile(&zip);
   pngFile.open(QIODevice::WriteOnly, QuaZipNewInfo("target.png"));
   targetImage.save(&pngFile, "PNG");
   pngFile.close();
   
   // Add JSON
   QuaZipFile jsonFile(&zip);
   jsonFile.open(QIODevice::WriteOnly, QuaZipNewInfo("metadata.json"));
   jsonFile.write(jsonData.toUtf8());
   jsonFile.close();
   
   zip.close();
   ```

#### Loading a Session

1. Open ZIP archive
2. Extract `target.png` → Load into QImage/QPixmap
3. Extract `metadata.json` → Parse with QJsonDocument
4. Reconstruct ShotGroupDocument object from parsed data
5. Recompute any non-persisted statistics if needed
6. Display in UI

#### File Extension Association

- Primary: `.spz` (ShotPlot Zip)
- MIME type: `application/x-shotplot` or `application/x-shotplot+zip`
- Icon: Target with shot group visualization

### Data Validation

When loading a `.spz` file, validate:
- ZIP contains both required files (`target.png`, `metadata.json`)
- JSON has required version field
- `pixelsPerInch > 0`
- `numberOfShots` matches length of `shots` array
- Image dimensions are reasonable (not corrupted)
- Statistics `shotIndices` reference valid shots
- Bullet diameter is positive

### Version Migration

Future versions may add fields to metadata.json:
- Unknown fields should be preserved (forward compatibility)
- Version 1.0 is baseline
- Breaking changes require new major version

---

## Build System (CMake)

### Requirements

- **CMake** 3.16 or later
- **C++17** compatible compiler:
  - Windows: MSVC 2019+, MinGW-w64, or Clang
  - macOS: Xcode 12+ (Clang)
  - Linux: GCC 8+, Clang 7+

### CMake Configuration Example

Basic `CMakeLists.txt` structure:

```cmake
cmake_minimum_required(VERSION 3.16)
project(ShotPlot VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(QuaZip-Qt6 REQUIRED)

# Source files
add_executable(ShotPlot
    src/main.cpp
    src/MainWindow.cpp
    # ... other sources
)

target_link_libraries(ShotPlot
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    QuaZip::QuaZip
)

# Platform-specific settings
if(WIN32)
    # Windows-specific configuration
    set_target_properties(ShotPlot PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
    # macOS-specific configuration
    set_target_properties(ShotPlot PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist
    )
elseif(UNIX)
    # Linux-specific configuration
    install(TARGETS ShotPlot DESTINATION bin)
endif()

# Testing
enable_testing()
add_subdirectory(tests)
```

### Building from Source

**General build process:**
```bash
# Clone repository
git clone https://github.com/username/ShotPlot.git
cd ShotPlot

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run tests
ctest

# Run application
./ShotPlot  # Linux/macOS
# or
.\ShotPlot.exe  # Windows
```

**Platform-specific notes:**
- Windows: May need to specify Qt6 path: `cmake -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2019_64 ..`
- macOS: Use Xcode generator: `cmake -G Xcode ..` or default Makefile
- Linux: Standard build process should work with installed packages

---

## Dependencies

### Required Libraries

- **Qt6** (LGPL/Commercial)
  - Qt6::Core - Core functionality
  - Qt6::Widgets - UI widgets
  - Qt6::Gui - Graphics and images
  - Qt6::Graphics - QGraphicsView/Scene framework
  - Minimum version: Qt 6.2 or later
- **QuaZip** - ZIP archive handling (LGPL)
  - Alternative: KArchive (KDE Frameworks)
  - Alternative: libzip with custom Qt integration
  - **Recommended**: QuaZip for Qt integration
  - Can be built from source or installed via package managers
- Qt6 ImageFormats plugin - Additional image format support for import

### Optional Libraries

- Qt6::Svg - For vector graphics export
- Qt6::PrintSupport - For printing functionality

### Platform-Specific Dependencies

**Windows:**
- Qt6 can be installed via official installer or vcpkg
- QuaZip available via vcpkg or manual build
- NSIS or WiX Toolset for installer creation (Phase 4)

**macOS:**
- Qt6 via Homebrew or official installer
- QuaZip via Homebrew: `brew install quazip`
- Xcode command line tools

**Ubuntu Linux:**
```bash
sudo apt install qt6-base-dev libqt6svg6-dev cmake build-essential
sudo apt install libquazip-qt6-dev  # or build from source
```

### License Compatibility

MIT License - permissive open source license allowing:
- Commercial use
- Modification
- Distribution
- Private use

**Dependencies License Compatibility:**
- Qt6: LGPL v3 (compatible - dynamic linking allowed)
- QuaZip: LGPL (compatible)
- All dependencies are compatible with MIT license for the application

### Attribution

Consider acknowledging:
- Qt framework
- QuaZip library
- Any statistical algorithms or references used
- Icon sets if using external resources

---

**See Also:**
- [Main Design Document](DESIGN.md)
- [Algorithm Details](ALGORITHM.md)
- [UI & Workflows](UI_WORKFLOWS.md)
