# ShotPlot - User Interface & Workflows

> Part of the [ShotPlot Design Documents](DESIGN.md)  
> **Version 0.10** | Last Updated: 2026-01-25

## Overview

This document details the user interface design, interaction patterns, and user workflows for ShotPlot.

## Table of Contents

- [Application Lifecycle](#application-lifecycle)
- [File Operations](#file-operations)
- [Main Window Layout](#main-window-layout)
- [Workflow States](#workflow-states)
- [Visual Design](#visual-design)
- [Statistics Plaque System](#statistics-plaque-system)
- [User Workflows](#user-workflows)
- [Configuration Settings](#configuration-settings)

---

## Application Lifecycle

### Startup

**Initial State:**
- Application opens with no document loaded
- Main window displays with empty target view
- Menu bar active, but document-dependent actions disabled:
  - File → Save (disabled)
  - File → Save As (disabled)
  - Edit menu items (disabled)
  - Export (disabled)
- Status bar shows: "No document loaded"

**Available Actions:**
- File → Import (enabled)
- File → Load (enabled)
- File → Quit (enabled)
- Drag and drop image or .spz file

### Document State

**Clean Document:**
- All changes saved
- Window title shows: `ShotPlot - [filename.spz]`
- File → Save disabled (no changes to save)
- File → Save As enabled
- Close without warning

**Dirty Document:**
- Unsaved changes exist
- Window title shows: `ShotPlot - [filename.spz]*` (asterisk indicates dirty)
- File → Save enabled
- File → Save As enabled
- Warn before:
  - Loading another document
  - Importing a new image
  - Quitting application

**Dirty State Triggers:**
- Shot marker added, moved, or deleted
- Calibration changed
- Metadata edited
- Visualization settings changed
- Any modification to ShotGroupDocument data

**ShotGroupDocument Moniker:**
- New documents (from Import): No moniker until first save
- Loaded documents (.spz): Moniker is the loaded file path
- Save operation: Uses existing moniker
- Save As operation: Prompts for new moniker

---

## File Operations

### File → Import

**Purpose**: Create new document from image file

**Behavior:**
1. If current document is dirty, warn user:
   - "You have unsaved changes. Do you want to save before importing?"
   - Options: Save, Don't Save, Cancel
2. Show file dialog:
   - Title: "Import Target Image"
   - File types: PNG (*.png), JPEG (*.jpg, *.jpeg), HEIC (*.heic) [future]
   - Initial directory: Last import directory (from QSettings)
3. User selects image file
4. Store selected directory to QSettings (lastImportDirectory)
5. Launch Calibration Wizard with selected image
6. On wizard completion:
   - New ShotGroupDocument created (no moniker)
   - ShotGroupDocument marked as dirty
   - Main window populated with target and statistics

**Enabled When:**
- Always enabled

### File → Save

**Purpose**: Save current document to existing file

**Behavior:**
1. If document has no moniker (never saved):
   - Redirect to File → Save As
2. If document has moniker:
   - Write .spz file to existing path
   - Clear dirty flag
   - Update window title (remove asterisk)
   - Status bar: "Saved to [filename.spz]"

**Enabled When:**
- Document is loaded AND dirty

### File → Save As

**Purpose**: Save current document to new/different file

**Behavior:**
1. Show save file dialog:
   - Title: "Save Session As"
   - File types: ShotPlot Session (*.spz)
   - Default extension: .spz
   - Initial directory: Last save directory (from QSettings), or current moniker directory
   - Pre-populated filename: Current moniker filename (if exists)
2. User selects/enters filename
3. Store selected directory to QSettings (lastSaveLoadDirectory)
4. Write .spz file to selected path
5. Update ShotGroupDocument moniker to new path
6. Clear dirty flag
7. Update window title with new filename
8. Status bar: "Saved to [filename.spz]"

**Enabled When:**
- Document is loaded

### File → Load

**Purpose**: Open existing .spz document

**Behavior:**
1. If current document is dirty, warn user:
   - "You have unsaved changes. Do you want to save before loading?"
   - Options: Save, Don't Save, Cancel
2. Show file dialog:
   - Title: "Open Session"
   - File types: ShotPlot Session (*.spz)
   - Initial directory: Last save/load directory (from QSettings)
3. User selects .spz file
4. Store selected directory to QSettings (lastSaveLoadDirectory)
5. Load ShotGroupDocument:
   - Extract target.png and metadata.json
   - Validate file format
   - Populate main window
6. Set ShotGroupDocument moniker to loaded file path
7. Clear dirty flag
8. Update window title: `ShotPlot - [filename.spz]`
9. Status bar: "Loaded [filename.spz]"

**Error Handling:**
- Invalid ZIP: "Error: Not a valid ShotPlot file"
- Missing files: "Error: Corrupted session file (missing data)"
- Invalid JSON: "Error: Cannot parse session data"

**Enabled When:**
- Always enabled

### File → Quit

**Purpose**: Exit application

**Behavior:**
1. If current document is dirty, warn user:
   - "You have unsaved changes. Do you want to save before quitting?"
   - Options: Save, Don't Save, Cancel
2. If user chooses Save:
   - Execute File → Save
   - If successful, quit
3. If user chooses Don't Save or no dirty document:
   - Quit immediately
4. If user chooses Cancel:
   - Return to application

**Enabled When:**
- Always enabled

### Drag and Drop

**Supported Formats:**
- Image files: PNG, JPEG, HEIC (future)
- Document files: .spz

**Behavior for Image Files:**
1. User drags image file over application window
2. Visual feedback: Drop zone highlight or cursor change
3. User drops file
4. Same behavior as File → Import:
   - Warn if current document is dirty
   - Launch Calibration Wizard with dropped image

**Behavior for .spz Files:**
1. User drags .spz file over application window
2. Visual feedback: Drop zone highlight or cursor change
3. User drops file
4. Same behavior as File → Load:
   - Warn if current document is dirty
   - Load the dropped .spz file

**Implementation Notes:**
- Accept drops anywhere in main window
- Validate file type before processing
- Reject unsupported file types with message
- Support multiple files dropped simultaneously?
  - Recommendation: Open first supported file, ignore rest

---

## Main Window Layout

```
+------------------------------------------------------------------+
| File  Edit  View  Tools  Help                                   |
+------------------------------------------------------------------+
| Workflow: [Caliber] → [Scale] → [POA] → [Mark Impacts] → [Viz]  |
+------------------------------------------------------------------+
| Per-State Toolbar: [state-specific controls]                    |
+------------------------------------------------------------------+
|                                    |                             |
|                                    |   State-Dependent           |
|                                    |   Side Panel                |
|                                    |                             |
|         Target View                |   [Content changes          |
|      (QGraphicsView)               |    per workflow state]      |
|                                    |                             |
|    [Target image with             |                             |
|     state-dependent overlays]      |                             |
|                                    |                             |
|                                    |                             |
|                                    |                             |
|                                    |                             |
|                                    |                             |
|                                    |                             |
|                                    |                             |
+------------------------------------------------------------------+
| Status: Current state message                           X: 0  Y: 0 |
+------------------------------------------------------------------+
```

### Layout Components

**Menu Bar**
- **File**: Import, Load, Save, Save As, Export Image, Quit
- **Edit**: Undo, Redo, Metadata
- **View**: Zoom In, Zoom Out, Zoom Fit, Display Units (Inches/MOA/MRAD)

**Workflow Toolbar**
- Visual representation of workflow pipeline
- Shows current state (highlighted)
- Shows completed states (checkmark or different color)
- Shows upcoming states (grayed out/disabled)
- States: Caliber → Scale Factor → Point of Aim → Mark Impacts → Visualization
- **Interactive**: Users can click on enabled states to navigate
- **State Enablement**:
  - Initially: Only "Set Caliber" enabled
  - Completing a state enables the next state
  - Users can return to any previously completed state
  - Clicking a state transitions to that state

**Per-State Toolbar**
- Changes based on current workflow state
- Contains controls and instructions specific to current task
- Examples:
  - Scale Factor state: "Select two points [1.0] inches apart"
  - Mark Impacts state: "Mark all impacts | [Clear ShotImpact List]"

**Target View (QGraphicsView)**
- Central widget displaying target image
- Renders state-dependent overlays and glyphs
- Accepts mouse input for interactive operations
- Custom cursor per state
- Zoom and pan controls (pan via middle-drag or Ctrl+drag)

**State-Dependent Side Panel**
- Content changes based on current workflow state
- Examples:
  - Caliber state: (none, uses dialog)
  - Mark Impacts state: Statistics display with checkboxes
  - Visualization state: (to be defined)

**Status Bar**
- Current state message or instruction
- Mouse cursor position in pixel coordinates

---

## Workflow States

ShotPlot uses a **state-based workflow** rather than a wizard. Users progress through states, with the UI adapting to each stage. States can be re-entered if ShotGroupDocument properties need to be changed.

### State Pipeline

1. **Set Caliber** (required, can re-enter)
2. **Specify Scale Factor** (required if not set, can re-enter)
3. **Set Point of Aim** (optional, skippable)
4. **Mark/Edit Impacts** (main working state)
5. **Edit Visualization** (to be defined later)

### State Transitions

**On Import (new document from image):**
1. Start in "Set Caliber" state
2. Only "Set Caliber" enabled in workflow toolbar

**On Load (existing .spz):**
- If document has caliber and scale factor → "Mark/Edit Impacts" state
- All completed states enabled in workflow toolbar (clickable)
- User can navigate back to any completed state

**State Navigation:**
- **Automatic**: Completing a state auto-advances to next state and enables it
- **Manual**: Click any enabled state in workflow toolbar to jump to it
- **Completion enables progression**: Each completed state enables the next state

**State Enablement Rules:**
- Set Caliber: Always enabled once document exists
- Scale Factor: Enabled after caliber is set
- Point of Aim: Enabled after scale factor is set
- Mark Impacts: Enabled after scale factor is set (POA is optional)
- Visualization: Enabled after at least one impact is marked

---

## State 1: Set Caliber

### Purpose
Define the bullet diameter for cursor and glyph rendering.

### UI Behavior

**Trigger:**
- After File → Import (automatic)
- Clicking "Set Caliber" in workflow toolbar (manual re-entry)

**Display:**
- Modal dialog appears
- Dropdown list with common calibers:
  - .224 (5.56mm)
  - .243 (6mm)
  - .264 (6.5mm)
  - .277 (270)
  - .284 (7mm)
  - .308 (7.62mm)
  - .338
  - .355 (9mm)
  - .375
  - Custom (text input)
- OK / Cancel buttons

**Interaction:**
1. User selects caliber from dropdown or enters custom value
2. User clicks OK
3. Dialog closes
4. If changing existing caliber: All ShotImpact glyphs and cursor resize to new bullet diameter

**Cancel Behavior:**
- If no caliber previously set: Returns to document without caliber (no state change)
- If caliber already set: Returns to previous state, keeps existing caliber

**Transition:**
- Auto-advances to "Specify Scale Factor" state (if no scale factor set)
- Or auto-advances to "Mark/Edit Impacts" state (if scale factor already set)
- Enables "Scale Factor" state in workflow toolbar

**Cursor:** Default arrow

**Side Panel:** None (dialog is modal)

---

## State 2: Specify Scale Factor

### Purpose
Interactive calibration to convert pixels to physical measurements.

### UI Behavior

**Per-State Toolbar:**
- Instruction text: "Select two points"
- Line edit with distance value (default: "1.0")
- Unit label: "inches apart"
- User can edit distance if using different reference (e.g., 3" paster)

**Interaction Sequence:**

**Step 1: First Click**
1. User clicks on first reference point
2. Circle-plus glyph placed at click location
3. Cursor remains as circle-plus

**Step 2: Drag/Move**
1. Bright green line renders from first point to cursor
2. Line updates dynamically as cursor moves
3. Distance in pixels shown (optional)

**Step 3: Second Click**
1. User clicks second reference point
2. Calculate scale factor: `pixelsPerInch = distance_pixels / distance_inches`
3. Clear first-point glyph and line
4. Reset cursor

**Transition:**
- If document has no Point of Aim → "Set Point of Aim" state
- If document has Point of Aim → "Mark/Edit Impacts" state

**Cursor:** Circle-plus (+ symbol inside circle, or just + crosshair)

**Side Panel:** 
- Instructions: "Click two points on a known distance"
- Shows calculated scale factor after completion (optional)

**Visual Feedback:**
- First click glyph: Circle with crosshair, bright color
- Reference line: Bright green (#00FF00), thick (3-5 pixels)
- Line drawn with QPainter overlay

**Notes:**
- Must validate that two click locations are different (non-zero distance)
- Consider allowing Esc to cancel and restart

---

## State 3: Set Point of Aim (Optional)

### Purpose
Mark the intended point of impact for future accuracy calculations.

### UI Behavior

**Per-State Toolbar:**
- Instruction text: "Click to mark your intended point of impact"
- [Skip] button (implementation deferred, likely added later)

**Interaction:**
1. User clicks on target at intended point of aim
2. POA glyph placed at click location
3. POA saved to ShotGroupDocument
4. Automatic transition to "Mark/Edit Impacts"

**Skipping:**
- For now: User can click "Mark Impacts" in workflow toolbar to skip POA
- Future: Dedicated [Skip] button in per-state toolbar

**Transition:**
- After click → Auto-advance to "Mark/Edit Impacts" state
- Enables "Mark Impacts" state in workflow toolbar

**Cursor:** X-shaped hit marker (rotated 45°, like Call of Duty style)
- Color: Bright, contrasting (e.g., magenta)
- Size: ~30-40 pixels

**Side Panel:**
- Instructions: "Point of Aim helps track accuracy vs precision"
- Note: "Optional - you can skip by clicking Mark Impacts"

**Point of Aim Glyph:**
- X symbol rotated 45 degrees
- Bright color (magenta #FF00FF)
- Different from impact markers
- Persistent on canvas

**Repositioning:**
- Clicking again before advancing replaces POA location
- Or: Click "POA" in workflow toolbar to re-enter state and reposition

---

## State 4: Mark/Edit Impacts

### Purpose
Main working state for marking shot locations and viewing statistics.

### UI Behavior

**Per-State Toolbar:**
- Instruction text: "Mark all impacts"
- [Clear ShotImpact List] button

**Interaction:**

**Adding Impacts:**
1. User left-clicks on shot hole
2. Verify click (mouseDown to mouseUp delta is small - not a pan)
3. Create ShotImpact in ShotGroupDocument
4. Place segmented circle glyph at location with shot number
5. Recalculate all statistics
6. Update side panel statistics

**Removing Impacts:**
- Right-click on ShotImpact glyph → Delete that ShotImpact
- Or: [Clear ShotImpact List] button → Remove all ShotImpacts immediately (no confirmation)

**Undo/Redo:**
- Full undo/redo support using QUndoStack
- Undoable operations:
  - Add single ShotImpact (Ctrl+Z to undo)
  - Remove single ShotImpact (Ctrl+Z to undo)
  - Clear ShotImpact list (Ctrl+Z to restore entire list as single operation)
- Redo: Ctrl+Shift+Z or Ctrl+Y
- **Implementation approach**: Complete ShotImpact list replacement on undo/redo

**Panning:**
- Middle-drag or Ctrl+drag to pan
- Distinguish from impact clicks by checking drag delta

**Transition:**
- Manual transition to "Edit Visualization" state (TBD: button or workflow toolbar)

**Cursor:** Segmented circle scaled to bullet diameter
- Color: To be defined (Color A)
- Size: Bullet diameter converted to pixels using scale factor
- Segmented: Circle with short gaps/interruptions at North, South, East, and West (cardinal directions)
  - Creates crosshair effect while maintaining circular boundary
  - Allows user to see through cursor to underlying target

**Side Panel - Statistics Display:**

Content varies based on application-wide display units setting (View → Display Units).

**Example with Inches selected:**
```
Shots: 5 / ?

Group Statistics:
-------------------
Full Group: 1.23"
80% Group: 0.96"
90% Group: 1.12"
Mean Radius: 0.45"
Std Dev: 0.38"

Display Options:
[x] Full Group Circle
[ ] Point of Aim
[ ] 80% Circle (future)
[ ] 90% Circle (future)

[Edit Metadata...]
```

**Example with MOA selected:**
```
Shots: 5 / ?

Group Statistics:
-------------------
Full Group: 1.17 MOA
80% Group: 0.92 MOA
90% Group: 1.07 MOA
Mean Radius: 0.43 MOA
Std Dev: 0.36 MOA

Display Options:
[x] Full Group Circle
[ ] Point of Aim
[ ] 80% Circle (future)
[ ] 90% Circle (future)

[Edit Metadata...]
```

**Example with MRAD selected:**
```
Shots: 5 / ?

Group Statistics:
-------------------
Full Group: 0.34 mrad
80% Group: 0.27 mrad
90% Group: 0.31 mrad
Mean Radius: 0.12 mrad
Std Dev: 0.10 mrad

Display Options:
[x] Full Group Circle
[ ] Point of Aim
[ ] 80% Circle (future)
[ ] 90% Circle (future)

[Edit Metadata...]
```

**Notes:**
- Display units setting is **application-wide** (stored in QSettings)
- Changes via View → Display Units menu
- All statistics panels immediately update when setting changes
- Affects all documents (not per-document setting)

**ShotImpact Glyphs:**
- Segmented circle outline with gaps at N, S, E, W cardinal directions
- Shot number rendered in center
- Color: Contrasting with background (same as cursor Color A)
- Size: Scaled to bullet diameter using scale factor
- Sequential numbering: 1, 2, 3, ...
- Same visual style as cursor (provides consistency)

**Real-Time Statistics:**
- Recalculate on every ShotImpact added/removed
- Update side panel immediately
- Enable/disable group circle rendering via checkboxes

**Notes:**
- Consider visual feedback when hovering over existing impacts
- Highlight selected/hovered ShotImpact
- Allow dragging impacts to reposition (like original design)?

---

## State 5: Edit Visualization

### Purpose
Configure visual overlays and statistics plaque for export.

### UI Behavior
**To be defined in future iteration**

Expected features:
- Toggle all group circles (80%, 90%)
- Configure statistics plaque
- Preview export appearance

---

## Workflow State Visual Summary

| State | Cursor | Per-State Toolbar | Side Panel | Overlays | Enabled When |
|-------|--------|-------------------|------------|----------|--------------|
| Caliber | Default | N/A (modal dialog) | None | None | Always (after import) |
| Scale Factor | Circle-plus (+) | Distance input field | Instructions | Green line, first-point glyph | After caliber set |
| Point of Aim | X (45° rotated) | (Skip deferred) | Instructions | POA X glyph | After scale factor set |
| Mark Impacts | Segmented circle (gaps at N/S/E/W) | Clear ShotImpact List button | Statistics + checkboxes | ShotImpact glyphs (segmented circles with numbers), group circles | After scale factor set |
| Visualization | TBD | TBD | TBD | TBD | After >= 1 impact marked |

**Cursor Details:**
- **Scale Factor**: Circle with + symbol inside (or just + crosshair)
- **Point of Aim**: X rotated 45°, magenta color
- **Mark Impacts**: Segmented circle (4 arcs with gaps at cardinals), scaled to bullet diameter

---

## Common Interactions (All States)

### Zoom and Pan

**Available in ALL states except Set Caliber (which is a modal dialog):**

**Mouse Wheel Zoom:**
- Scroll up: Zoom in
- Scroll down: Zoom out
- Zoom centered on cursor position
- Works in Scale Factor, POA, Mark Impacts, and Visualization states

**Panning:**
- **Left mouse drag**: Pan the view (when drag delta exceeds threshold)
- **Middle mouse drag**: Pan the view (always)
- **Ctrl + Left mouse drag**: Pan the view (always)
- Works in all states with QGraphicsView active

**QGraphicsView Configuration:**
- Scroll bars: Always disabled
- Pan: Handled by mouse drag detection
- Zoom: Handled by mouse wheel events
- Smooth scrolling/zooming for better UX

**Keyboard:**
- Esc: Cancel current operation / reset state
- Ctrl+Z: Undo (in Mark Impacts state)
- Ctrl+Shift+Z or Ctrl+Y: Redo (in Mark Impacts state)

### Click vs Drag Detection

**Critical for State Interactions:**

States need to distinguish between clicks (state action) and drags (panning):

1. **Track mouse press position**: Store `mousePressPos` on mouse down
2. **Calculate drag delta**: On mouse move, compute distance from press position
3. **Threshold check**: If delta < threshold (e.g., 5 pixels) → Treat as click
4. **State behavior**:
   - **Small delta** (< 5px): Execute state action (add ShotImpact, mark POA, etc.)
   - **Large delta** (>= 5px): Pan the view instead

**Implementation per State:**
- **Scale Factor**: First click sets point, drag before second click pans
- **Point of Aim**: Click sets POA, drag pans
- **Mark Impacts**: Click adds ShotImpact, drag pans

**Middle mouse** or **Ctrl+drag** always pans, regardless of delta

---

## Visual Design

### Color Scheme

**Shot Markers:**
- Primary color: Bright red or yellow for visibility
- Numbered labels (optional): White text on semi-transparent background
- Selected marker: Blue highlight border
- Hover state: Slight enlargement and border highlight

**Group Circles:**
- Full group (100%): Semi-transparent blue (#0066CC, 40% opacity)
- 80% circle: Semi-transparent green (#00CC66, 40% opacity)
- 90% circle: Semi-transparent orange (#FF9933, 40% opacity)
- Circle centers: Contrasting color (e.g., cyan crosshair)

**Other Elements:**
- Point of Aim marker: Magenta crosshair (#CC00CC)
- Reference line: Yellow (#FFCC00) with measurement label
- Background: Image-dependent, ensure overlays are visible

### Shot Markers (ShotImpact Glyphs)

**Design:**
- Segmented circle with gaps at cardinal directions (N, S, E, W)
- Creates crosshair-like effect while maintaining circular boundary
- Diameter: Scaled to bullet diameter (e.g., .308" converted to pixels using calibration)
- Shot number rendered in center of circle
- Line width: 2-3 pixels for visibility

**Visual Structure:**
- Four arc segments (NE, SE, SW, NW quadrants)
- Short gaps (~10-15° each) at N, S, E, W positions
- Center: Shot number (white text with optional dark outline for contrast)

**States:**
- **Normal**: Primary color (red or yellow) segmented circle outline
- **Hover**: Slightly enlarged, highlighted
- **Selected**: Blue outline, ready for delete
- **Excluded** (from 80%/90%): Dimmed or grayed out (optional, future)

**Numbering:**
- Always visible in center of glyph
- Sequential: 1, 2, 3, ... (order of placement)
- Font: Bold, sans-serif
- Color: White with subtle dark outline/shadow for contrast against any background

### Information Display

**Statistics Panel:**
- Clean, readable typography (sans-serif font)
- Hierarchical organization (session info → statistics → options)
- Units clearly indicated (", MOA, mil)
- Update animations: Subtle fade when values change
- Color coding: Green for good groups, red for large groups (optional)

**On-Target Overlays:**
- Semi-transparent backgrounds for readability
- Minimize visual clutter
- Use consistent styling across all overlays
- Ensure visibility against various target backgrounds

---

## Statistics Plaque System

### Format String System

**Purpose**: Allow users to customize the statistics overlay on exported images.

**Syntax**: Use curly braces `{placeholder}` for dynamic values.

**Available Placeholders:**

| Placeholder | Description | Example |
|-------------|-------------|---------|
| `{distance}` | Target distance | `100` |
| `{distanceUnit}` | Distance unit | `yard` |
| `{bulletDiameter}` | Bullet diameter | `0.308` |
| `{shots}` | Number of shots | `5` |
| `{date}` | Session date | `2026-01-25` |
| `{fullGroup}` | Full group size | `1.23` |
| `{group80}` | 80% group | `0.96` |
| `{group90}` | 90% group | `1.12` |
| `{meanRadius}` | Mean radius | `0.45` |
| `{stdDev}` | Std deviation | `0.38` |
| `{unit}` | Current unit | `"`, `MOA`, `mil` |
| `{fullGroupMOA}` | Full group in MOA | `1.17` |
| `{fullGroupMil}` | Full group in mils | `0.34` |
| `{firearm}` | Firearm description | User's text |
| `{ammunition}` | Ammo description | User's text |
| `{notes}` | User notes | User's text |

**Formatting:**
- Use `\n` for line breaks
- Text outside placeholders rendered as-is
- Unknown placeholders: Ignored or left as-is

### Example Format Strings

**Minimal:**
```
{fullGroup}" @ {distance}{distanceUnit}
{fullGroupMOA} MOA
```

**Detailed:**
```
Shot Group Analysis
Distance: {distance} {distanceUnit}
Bullet: {bulletDiameter}"

Full Group: {fullGroup} {unit}
80% Group: {group80} {unit}
Mean Radius: {meanRadius} {unit}
Std Dev: {stdDev} {unit}

{shots} shots on {date}
```

**Competition Style:**
```
{firearm}
{ammunition}

Group: {fullGroup}" @ {distance}yd
MOA: {fullGroupMOA}
```

### Plaque Configuration UI

**Dialog Window:**
- Enable/disable checkbox
- Position controls:
  - X, Y coordinates (or drag on target preview)
  - Width, Height sliders
- Title text input
- Format string editor:
  - Multi-line text area
  - Placeholder suggestions dropdown
  - Preview pane showing rendered output
- Style options:
  - Background color picker
  - Opacity slider (0-100%)
  - Font selector
  - Border style dropdown
- Template presets dropdown
- OK / Cancel buttons

**Preview:**
- Live preview of plaque on target image
- Updates as user types
- Shows how it will appear in export

---

## User Workflows

### Workflow 1: Analyze a New Target

**Import and Setup:**
1. Launch ShotPlot (no document loaded)
2. File → Import (or drag-and-drop image file)
3. Select target photograph (PNG, JPEG, or HEIC)
4. Image loads in main window

**State 1: Set Caliber**
5. **Caliber dialog appears automatically**
6. Select bullet diameter: .308" from dropdown
7. Click OK
8. Transitions to "Specify Scale Factor" state

**State 2: Specify Scale Factor**
9. **Workflow toolbar highlights "Scale Factor" step**
10. Per-state toolbar shows: "Select two points [1.0] inches apart"
11. Cursor changes to circle-plus (+)
12. Click first point on 1" reference mark
13. Green line follows cursor from first point
14. Click second point to complete reference
15. Scale factor calculated automatically
16. Transitions to "Set Point of Aim" state

**State 3: Set Point of Aim (Optional)**
17. **Cursor changes to X-shaped hit marker**
18. Per-state toolbar shows: "Click to mark intended point of impact | [Skip]"
19. Click on target center (or click Skip)
20. POA glyph placed if clicked
21. Transitions to "Mark/Edit Impacts" state

**State 4: Mark Impacts**
22. **Cursor changes to segmented circle (bullet diameter)**
23. Per-state toolbar shows: "Mark all impacts | [Clear ShotImpact List]"
24. Side panel shows statistics (initially empty)
25. Click on each shot hole (5 times)
26. ShotImpact glyphs appear with shot numbers (1, 2, 3, 4, 5)
27. **Statistics recalculate after each click:**
    - Full Group: 1.23"
    - 80% Group: 0.96"
    - 90% Group: 1.12"
    - Mean Radius: 0.45"
    - Std Dev: 0.38"
    - MOA: 1.17
28. Toggle display options in side panel:
    - [x] Full Group Circle (blue overlay appears)
    - [ ] Point of Aim
29. If mistake: Right-click ShotImpact to delete, or use Undo (Ctrl+Z)

**Save and Export:**
30. File → Save As
31. Enter filename: "2026-01-25_RemingtonPrecision.spz"
32. ShotGroupDocument saved (dirty flag cleared)
33. Optional: Edit → Metadata
    - Firearm: "Remington 700 .308 Win"
    - Ammunition: "Federal GMM 168gr"
    - Notes: "First group with new load"
34. Optional: File → Export Image
    - Select format (PNG)
    - Export annotated image
35. Done!

### Workflow 2: Load and Review Previous Session

1. Launch ShotPlot (no document loaded)
2. File → Load (or drag-and-drop .spz file)
3. Browse to .spz file: "2026-01-25_RemingtonPrecision.spz"
4. **ShotGroupDocument loads:**
   - Target image displayed in QGraphicsView
   - All ShotImpact glyphs restored with shot numbers
   - Statistics calculated and shown in side panel
   - Workflow toolbar shows "Mark Impacts" state (all previous states completed)
5. **Automatically in "Mark/Edit Impacts" state**
6. Review statistics in side panel:
   - Full Group, 80%, 90% displayed
   - Toggle display mode (inches/MOA/mils) if desired
7. Optional: Add more impacts
   - Click additional shot holes
   - Statistics recalculate automatically
8. Optional: Remove impacts
   - Right-click ShotImpact glyph to delete
   - Or use Undo (Ctrl+Z)
9. Optional: Re-calibrate scale factor
   - Navigate back to "Scale Factor" state (TBD: menu action or workflow toolbar)
   - Set new reference line
   - All statistics update with new scale (pixel data unchanged)
10. Optional: Change caliber
   - Navigate back to "Set Caliber" state (TBD: menu action)
   - Cursor and glyphs resize to new bullet diameter
11. If changes made: File → Save (ShotGroupDocument marked dirty)
12. Or: File → Save As to create new version

### Workflow 3: Configure Statistics Display

1. Open session
2. In Statistics Panel, click display mode toggle:
   - Inches → MOA → Mils → Inches
3. Check/uncheck group circle visibility:
   - [x] Full Group Circle
   - [x] 80% Circle
   - [ ] 90% Circle
4. Visibility changes reflected immediately on target view
5. Settings automatically saved with session

### Workflow 4: Export Annotated Image

1. Open session
2. Configure what to show:
   - Toggle group circles
   - Configure statistics plaque (if desired)
3. Click File → Export Image
4. **Export dialog opens**
5. Preview pane shows final output
6. Select options:
   - Format: PNG (lossless), JPEG (smaller), PDF (document)
   - Quality: (for JPEG)
   - Resolution: (for scaling)
   - Include elements:
     - [x] Shot markers
     - [x] Group circles
     - [x] Statistics plaque
     - [ ] Point of Aim
7. Choose save location and filename
8. Click Export
9. Success message displayed

---

## Configuration Settings

### Application Settings (Global)

Accessed via Edit → Preferences (or File → Settings)

**General:**
- Default units: inches / cm
- Default distance unit: yards / meters
- Default display mode: inches / MOA / mils
- Recent files list size: 5-20

**Marker Appearance:**
- Color: Red / Yellow / Custom
- Size: Small / Medium / Large
- Numbered: Yes / No (default)
- Numbering style: Sequential / Custom

**Statistics Display:**
- Metrics to show: Checkboxes for each metric
- Decimal places: 1-4
- MOA calculation: Exact (1.047) / Approximate (1.0)

**Export Defaults:**
- Default format: PNG / JPEG / PDF
- Default quality: (for JPEG)
- Default resolution: 1x / 2x / 4x
- Always include: Checkboxes for overlay elements

**Calibration:**
- Default reference distance: 1.0 inches
- Bullet diameter presets: Editable list

### Per-Session Settings (Saved in .spz)

**Visualization Settings:**
- Show full group circle: Yes / No
- Show 80% group circle: Yes / No
- Show 90% group circle: Yes / No
- Show center points: Yes / No
- Show Point of Aim marker: Yes / No

**Marker Display:**
- Marker numbering: Yes / No
- Marker size: (inherited from app settings, can override)

**Statistics Plaque:**
- Enabled: Yes / No
- Position: (x, y) in pixels
- Size: (width, height) in pixels
- Title: Text string
- Format string: Multi-line template
- Background color: RGB
- Opacity: 0-100%
- Font: Font family and size
- Border: Style and color

**Visual Overlays:**
- Group circle opacity: 0-100%
- Marker opacity: 0-100%
- Overlay line width: 1-5 pixels

---

## Accessibility Considerations

**Keyboard Navigation:**
- All functions accessible via keyboard
- Tab navigation through UI elements
- Keyboard shortcuts for common actions (Ctrl+O, Ctrl+S, etc.)
- Esc to cancel operations

**Visual Clarity:**
- High contrast mode option
- Configurable marker colors for color blindness
- Adjustable font sizes
- Clear visual feedback for all interactions

**Error Messages:**
- Clear, actionable error messages
- Specific guidance on how to fix issues
- No cryptic error codes

---

**See Also:**
- [Main Design Document](DESIGN.md)
- [Algorithm Details](ALGORITHM.md)
- [Technical Architecture](ARCHITECTURE.md)
