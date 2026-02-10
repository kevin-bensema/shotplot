# ShotPlot - Design Document

> **Version 0.10** | Last Updated: 2026-01-25

## Overview

ShotPlot is a shot group analysis application designed for rifle shooters. It allows users to analyze their shooting accuracy by photographing their targets and marking shot locations. The application calculates various statistical measures of group size and performance, providing valuable feedback for improving shooting technique.

## Technology Stack

- **Language**: C++17 or later
- **Build System**: CMake (3.16 or later)
- **GUI Framework**: Qt6
- **Graphics**: QGraphicsView/QGraphicsScene for image display and interaction
- **License**: MIT

### Platform Support
- **Target Platforms**:
  - Windows (x64)
  - macOS (Apple Silicon - aarch64)
  - Ubuntu Linux (x64)
- **Distribution Targets** (future):
  - Windows: Installer (.exe via NSIS or WiX)
  - macOS: Disk Image (.dmg with app bundle)
  - Ubuntu: Debian Package (.deb)

## Core Requirements

### Essential Features

**Image Processing:**
- Load target photographs in common formats (JPEG, PNG, BMP, TIFF)
- Convert to PNG format internally for consistency
- Display with zoom and pan controls

**Calibration:**
- User-defined scale reference (pixels to inches conversion)
- Support for various bullet diameters

**Shot Analysis:**
- Mark shot impact locations on target image
- Calculate group size statistics using smallest enclosing circle algorithm
- Compute 80% and 90% group sizes
- Real-time statistics updates
- Display results in inches, MOA, or milliradians

**Data Management:**
- Save/load sessions in .spz format (ZIP-based)
- Store all data in pixel coordinates
- Metadata support (firearm, ammunition, date, notes)
- Export annotated images with statistics

**Key Metrics:**
- Full Group Size (100%) - diameter of smallest circle containing all shots
- 80% Group Size - diameter of smallest circle containing 80% of shots
- 90% Group Size - diameter of smallest circle containing 90% of shots
- Mean Radius - average distance from centroid
- Standard Deviation - measure of dispersion

### Design Principles

1. **Pixel-Based Storage**: All shot positions and calculations stored in pixel coordinates; physical measurements calculated on-demand
2. **Separation of Concerns**: Clear distinction between data storage, calculation, and display
3. **User Control**: Flexible visualization settings and customizable export options
4. **Cross-Platform**: Support Windows, macOS, and Linux from a single codebase

## Project Roadmap

### Phase 1: Core Functionality (MVP)
**Goal**: Basic working application with essential features

- CMake build system setup
- Image loading and display
- Basic 5 states: caliber, scale, mark aim point, mark impacts, analyze
- Shot marker placement
- **Smallest enclosing circle algorithm** (critical component)
- Full group size (100%) calculation
- Basic statistics (mean radius, standard deviation)
- .spz file format (save/load)
- Basic image export

### Phase 2: Enhanced Features
**Goal**: Complete feature set for practical use

- 80% and 90% group calculations
- Multiple group circle visualization
- Metadata editor (firearm, ammunition, notes)
- Angular measurements (MOA/mils)
- Statistics plaque system with format strings
- Enhanced image export with annotations
- Visual settings persistence

### Phase 3: Distribution
**Goal**: Professional deployment for all platforms

- Cross-platform testing with friends (closed alpha)
- Windows installer (NSIS/WiX)
- macOS app bundle and DMG
- Ubuntu/Debian package

## Detailed Documentation

This design is split into specialized documents for easier navigation:

### 📊 [ALGORITHM.md](ALGORITHM.md)
**Group Size Calculation Algorithm**
- Smallest enclosing circle algorithm (the heart of ShotPlot)
- Mathematical foundations and proofs
- Pseudocode implementations
- 80%/90% group calculation methods
- Numerical stability considerations
- Test cases and validation

### 🏗️ [ARCHITECTURE.md](ARCHITECTURE.md)
**Technical Architecture & Data Model**
- Project structure and organization
- Class designs and responsibilities
- File format specification (.spz)
- JSON schema details
- CMake configuration
- Dependencies and build requirements
- Data flow diagrams

### 🎨 [UI_WORKFLOWS.md](UI_WORKFLOWS.md)
**User Interface & Workflows**
- Main window layout and design
- Calibration wizard flow
- Interaction modes
- User workflows (step-by-step)
- Statistics plaque system
- Visual design guidelines
- Export functionality

## Key Decisions

✓ **File format**: .spz (ZIP containing target.png + metadata.json)  
✓ **Coordinate system**: Pixel coordinates only, top-left origin  
✓ **Data storage**: ALL values in pixel coordinates (fractional/float)  
✓ **Display conversion**: Physical units (inches/MOA/mils) calculated on-demand  
✓ **Calibration**: Single scale factor (pixelsPerInch), user can re-calibrate  
✓ **Bullet specification**: Diameter in inches (float), not caliber string  
✓ **Bullet diameter usage**: Render shot marker glyphs and mouse cursor to scale  
✓ **Statistics persistence**: Only essential metrics (meanRadius, stdDev, 3 group circles)  
✓ **Group percentages**: Fixed at 80% and 90%  
✓ **Algorithm**: O(N³) naive approach (fast enough for typical shot counts)  
✓ **Point of Aim**: Store for future adjustment calculations  
✓ **Build system**: CMake for cross-platform builds  
✓ **Scope**: Single-target sessions only (no multi-target comparison)  

## Open Questions

### UI/UX
1. **Undo/Redo**: Full undo stack for marker placement, or just manual adjustment?
2. **Statistics plaque**: Font size, colors, auto-sizing vs fixed dimensions?
3. **Format string validation**: Warn about unknown placeholders?
4. **Default templates**: Provide preset format strings for common use cases?
5. **Cursor rendering**: Exact visual design for the bullet-diameter-sized cursor in marking mode

## References

### Algorithm & Mathematics
- Smallest Enclosing Circle problem (Wikipedia)
- Welzl's Algorithm for linear-time computation
- Circumcircle calculation methods

### Shooting Metrics
- Mean Radius as precision metric
- MOA and Mil definitions
- Group size methodologies in shooting sports

### Similar Tools
- Existing shot analysis software
- Target analysis methodologies
- Industry best practices

### Qt6 Resources
- QGraphicsView framework
- JSON serialization
- Cross-platform deployment

---

**License**: MIT  
**Status**: Design specification - ready for implementation

## Document History

**v0.10** (2026-01-25)
- **Display units as application-wide setting**:
  - Added QSettings key: `display/units` (inches/moa/mrad)
  - Setting affects all documents, not per-document
  - Statistics panels update immediately on change
- **Simplified menu structure**:
  - Removed Tools and Help menus for now
  - File menu: Complete (Import, Load, Save, Save As, Export, Quit)
  - Edit menu: Undo, Redo, Metadata
  - View menu: Zoom In, Zoom Out, Zoom Fit, Display Units submenu
- **StatisticsCalculator service architecture**:
  - Separated calculation logic from ShotGroupDocument class
  - Stateless service with static methods
  - ShotGroupDocument delegates to StatisticsCalculator, caches results
  - Better separation of concerns and testability

**v0.9** (2026-01-25)
- **Complete ShotGroupDocument class design**:
  - Comprehensive data member specification (image, caliber, scale, POA, impacts, metadata)
  - Full interface with getters/setters, state queries, serialization
  - Statistics caching with dirty flag
  - Qt signals for UI updates
- **QGraphicsView configuration**:
  - Scroll bars always disabled
  - Pan/zoom available in all states (except Caliber dialog)
  - Detailed implementation of wheel zoom and drag pan
  - Click vs drag detection (5-pixel threshold)
  - State delegation for mouse events

**v0.8.1** (2026-01-25)
- **Clarified workflow state behavior**:
  - Workflow toolbar is clickable - users can navigate to any enabled state
  - States become enabled as prerequisites are met
  - Clicking "Set Caliber" relaunches dialog, resizes glyphs if changed
  - Loading documents with caliber+scale goes to Mark Impacts state
- **Segmented circle design**: Circle with gaps at N, S, E, W cardinal directions
- **Undo/redo details**: Clear ShotImpact List is single undoable operation, no confirmation
- **POA skip**: Deferred, user can click Mark Impacts state to skip for now

**v0.8** (2026-01-25)
- **Major UI redesign**: Replaced calibration wizard with state-based workflow
- Five workflow states: Set Caliber → Scale Factor → Point of Aim → Mark Impacts → Visualization
- New UI layout: Workflow toolbar + per-state toolbar + state-dependent side panel
- Detailed cursor designs per state (circle-plus, X-marker, segmented circle)
- Interactive scale calibration with visual feedback (green line, glyphs)
- Undo/redo support for impact marking
- Click vs drag detection for pan vs mark disambiguation

**v0.7** (2026-01-25)
- Added comprehensive application lifecycle documentation
- Documented all file operations (Import, Save, Save As, Load, Quit)
- Added drag-and-drop support specification
- Added QSettings for directory persistence
- Detailed document state management (clean/dirty, moniker)

**v0.6** (2026-01-25)
- Removed advanced features (Phase 3) - out of scope
- Clarified design decisions:
  - Bullet diameter used for cursor and glyph rendering
  - Point of Aim for future adjustment calculations
  - Fixed 80%/90% group percentages
  - O(N³) naive algorithm (sufficient for use case)
  - Single-target sessions only
  - No testing framework or CI/CD in initial scope
- Simplified open questions to focus on UI/UX details

**v0.5** (2026-01-25)
- Split into multiple documents for better organization
- Added CMake build system specification
- Added platform support (Windows/macOS/Linux)
- Added distribution targets

**v0.4** (2026-01-25)
- Added visualization settings
- Statistics plaque system with format strings

**v0.3** (2026-01-25)
- Pixel-based storage architecture
- On-demand unit conversions

**v0.2** (2026-01-25)
- .spz ZIP-based file format
- Simplified metadata structure
- Point of Aim feature

**v0.1** (2026-01-25)
- Initial design document
