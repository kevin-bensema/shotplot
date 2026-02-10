# UI_WORKFLOWS.md Review - Findings

**Review Date:** 2026-02-02  
**Document Version:** 0.10  
**Document Path:** `/home/kevin/Projects/ShotPlot/Documents/UI_WORKFLOWS.md`

---

## 1. REMAINING DESIGN QUESTIONS

### Major Unanswered Questions

1. **State 5: Edit Visualization - Completely Undefined**
   - Entire state marked as "To be defined in future iteration"
   - Missing: UI layout, controls, interaction patterns, cursor, side panel content
   - Missing: How users enter/exit this state
   - Missing: What operations are available in this state
   - Impact: Critical workflow gap - users cannot configure export visualizations

2. **Color A Definition**
   - Referenced multiple times (lines 522, 604) but never defined
   - Used for: Impact glyphs and cursor in Mark Impacts state
   - Question: What is the actual color value/name for "Color A"?
   - Related: Visual Design section mentions "red or yellow" but doesn't specify which is Color A

3. **Export Dialog Details**
   - Workflow 4 mentions export dialog but lacks specification:
     - Dialog layout and controls
     - Preview pane implementation details
     - Format selection UI (dropdown? radio buttons?)
     - Quality/resolution controls UI
     - Include/exclude checkboxes layout
     - Error handling for export failures

4. **Metadata Editing Dialog**
   - Referenced in workflows (line 915) but not specified:
     - Dialog layout and fields
     - Field validation rules
     - Character limits
     - Date picker vs text input
     - Save/cancel behavior

5. **Preferences/Settings Dialog**
   - Configuration Settings section lists settings but doesn't specify:
     - Dialog structure and organization (tabs? sections?)
     - UI controls for each setting type
     - Default values for all settings
     - Apply/Cancel/OK button behavior
     - Settings validation

6. **Calibration Wizard Behavior**
   - Referenced but not detailed:
     - Wizard step-by-step flow
     - What happens if user cancels wizard?
     - Can user go back/forward in wizard?
     - Error handling for invalid calibration attempts

### Minor Unanswered Questions

7. **Skip Button for POA State**
   - Implementation deferred (line 444)
   - Current workaround documented but not ideal
   - Question: When will this be implemented? What will it look like?

8. **80%/90% Circle Display**
   - Marked as "(future)" in multiple places (lines 547, 568, 589)
   - Question: Are these calculations implemented but display deferred, or both deferred?
   - Question: When will these be available?

9. **HEIC Format Support**
   - Marked as "[future]" (lines 89, 193)
   - Question: Timeline for implementation?

10. **ShotImpact Repositioning**
    - Line 617: "Allow dragging ShotImpacts to reposition (like original design)?"
    - Question: Is this feature planned? What's the interaction pattern?

11. **Excluded ShotImpact Visualization**
    - Line 743: "Excluded (from 80%/90%): Dimmed or grayed out (optional, future)"
    - Question: Is this feature planned? How will users understand which ShotImpacts are excluded?

12. **Navigation to Previous States**
    - Lines 945, 949: "TBD: menu action or workflow toolbar"
    - Question: How exactly do users navigate back to Scale Factor or Caliber states?
    - Current: Workflow toolbar click mentioned, but details unclear

13. **Statistics Display When No POA Set**
    - Question: What statistics are shown/calculated when POA is not set?
    - Accuracy vs precision distinction mentioned but not detailed

14. **Minimum Shot Requirements**
    - Question: What happens when < 2 ShotImpacts are marked?
    - Some statistics (e.g., std dev) require multiple ShotImpacts
    - Error handling not specified

15. **Multi-File Drag and Drop**
    - Line 216: "Support multiple files dropped simultaneously?"
    - Recommendation given but question mark suggests uncertainty
    - Question: Final decision on behavior?

---

## 2. COVERAGE GAPS

### Missing Workflows

1. **Error Recovery Workflows**
   - What happens when file operations fail?
   - How to recover from corrupted .spz files?
   - What if image file is corrupted or unreadable?
   - What if calibration produces invalid results (e.g., negative scale factor)?

2. **Undo/Redo Workflows (Beyond Mark Impacts)**
   - Undo/redo specified only for Mark Impacts state
   - Missing: Can users undo calibration changes? POA placement? Metadata edits?
   - Missing: Undo/redo for visualization settings

3. **Re-calibration Workflow**
   - Workflow 2 mentions re-calibrating (line 944) but lacks detail:
     - What happens to existing ShotImpacts when scale factor changes?
     - Are ShotImpact positions recalculated?
     - Are statistics automatically updated?
     - Warning dialogs?

4. **Changing Caliber Workflow**
   - Workflow 2 mentions changing caliber (line 948) but lacks detail:
     - What happens to existing ShotImpacts?
     - Are glyphs automatically resized?
     - Any data loss concerns?

5. **Metadata Editing Workflow**
   - Referenced but no detailed workflow provided
   - Missing: Step-by-step interaction pattern

6. **Preferences Configuration Workflow**
   - Missing: How users access and modify preferences
   - Missing: When preferences take effect (immediate vs restart)

7. **Export Workflow Details**
   - Workflow 4 is high-level but missing:
     - File dialog behavior
     - Filename suggestions
     - Overwrite warnings
     - Export progress indication
     - Success/failure feedback

8. **Session Management**
   - Missing: Can users have multiple sessions open?
   - Missing: Window management if multiple sessions
   - Missing: Recent files menu behavior

9. **Statistics Calculation Edge Cases**
   - Missing: Behavior with 0 ShotImpacts
   - Missing: Behavior with 1 ShotImpact
   - Missing: Behavior with 2 ShotImpacts (some stats may be undefined)
   - Missing: Behavior when all ShotImpacts are collinear

10. **Visualization State Entry/Exit**
    - Missing: How users enter Visualization state
    - Missing: How users exit Visualization state
    - Missing: Can users edit ShotImpacts while in Visualization state?

### Missing Error Handling Specifications

11. **File Operation Errors**
    - Invalid file formats
    - Permission denied errors
    - Disk full errors
    - Network drive errors (if applicable)

12. **Calibration Errors**
    - Zero-length calibration line
    - Invalid distance input (negative, zero, non-numeric)
    - Calibration produces unrealistic scale factors

13. **ShotImpact Placement Errors**
    - Clicking outside image bounds
    - Overlapping ShotImpacts (allowed? prevented?)
    - Maximum number of ShotImpacts (if any limit)

14. **Statistics Calculation Errors**
    - Division by zero scenarios
    - Numerical precision issues
    - Invalid data states

### Missing UI Component Specifications

15. **Recent Files Menu**
    - Configuration Settings mentions "Recent files list size: 5-20"
    - Missing: Menu structure, how files are displayed, click behavior

16. **Keyboard Shortcuts Comprehensive List**
    - Some shortcuts mentioned (Ctrl+Z, Ctrl+Y, Esc)
    - Missing: Complete list of all keyboard shortcuts
    - Missing: Shortcuts for menu items (Ctrl+O, Ctrl+S, etc.)

17. **Status Bar Details**
    - Basic description provided but missing:
      - What messages appear in different states?
      - Message format and length limits
      - How mouse coordinates are formatted

18. **Workflow Toolbar Visual Design**
    - Functionality described but missing:
      - Visual appearance (buttons? tabs? breadcrumbs?)
      - How completed states are indicated
      - How current state is highlighted
      - Disabled state appearance

19. **Per-State Toolbar Layout**
    - Examples given but missing:
      - Standard layout pattern
      - Button sizes and spacing
      - Text field sizing
      - Responsive behavior

20. **Side Panel Layout**
    - Content described but missing:
      - Panel width and resizability
      - Scroll behavior for long content
      - Collapsible sections?
      - Panel visibility toggle?

### Missing Interaction Details

21. **Click vs Drag Threshold**
    - Threshold mentioned (5 pixels, line 690) but:
     - Is this configurable?
     - Same threshold for all states?
     - Platform-specific considerations?

22. **Zoom Limits**
    - Zoom functionality described but missing:
     - Minimum zoom level
     - Maximum zoom level
     - Zoom step size
     - Zoom fit behavior details

23. **Pan Boundaries**
    - Pan functionality described but missing:
     - Can users pan beyond image bounds?
     - Pan limits/constraints
     - Pan speed/acceleration

24. **ShotImpact Selection**
    - Right-click to delete mentioned but missing:
     - How are ShotImpacts selected?
     - Visual feedback for selection
     - Can multiple ShotImpacts be selected?
     - Keyboard selection (arrow keys)?

25. **Hover Feedback**
    - Mentioned (lines 615-616) but not fully specified:
     - Hover delay/timing
     - Visual feedback details
     - Tooltips?

### Missing Data Validation Rules

26. **Input Validation**
    - Missing: Validation rules for:
      - Caliber input (range? format?)
      - Distance input in scale factor (range? format?)
      - Metadata fields (length limits? character restrictions?)
      - Filename validation for save operations

27. **Data Consistency Checks**
    - Missing: What happens if:
      - ShotGroupDocument has ShotImpacts but no scale factor?
      - ShotGroupDocument has ShotImpacts but no caliber?
      - Scale factor is invalid (zero, negative, extremely large/small)?

---

## 3. AMBIGUITIES AND INCOMPLETE SPECIFICATIONS

### Color Inconsistencies

1. **Reference Line Color Conflict**
   - Line 404: "Bright green line" (#00FF00)
   - Line 426: "Bright green (#00FF00)"
   - Line 722: "Reference line: Yellow (#FFCC00) with measurement label"
   - **Ambiguity**: Is the reference line green or yellow? Or are there two different elements?

2. **ShotImpact Marker Color Ambiguity**
   - Line 522: "Color: To be defined (Color A)"
   - Line 604: "Color: Contrasting with background (same as cursor Color A)"
   - Line 709: "Primary color: Bright red or yellow for visibility"
   - Line 740: "Primary color (red or yellow) segmented circle outline"
   - **Ambiguity**: Which color is actually used? Is Color A red or yellow? Or user-configurable?

3. **Group Circle Center Color**
   - Line 718: "Circle centers: Contrasting color (e.g., cyan crosshair)"
   - **Ambiguity**: "e.g." suggests example, not specification. What's the actual color?

### State Transition Ambiguities

4. **State Transition After Caliber Change**
   - Line 374: "Or auto-advances to 'Mark/Edit Impacts' state (if scale factor already set)"
   - **Ambiguity**: What if both caliber and scale factor are already set? Does changing caliber stay in current state or force transition?

5. **POA State Skipping**
   - Line 453: "For now: User can click 'Mark Impacts' in workflow toolbar to skip POA"
   - **Ambiguity**: Is this the intended long-term behavior or temporary workaround?

6. **Visualization State Entry**
   - Line 519: "Manual transition to 'Edit Visualization' state (TBD: button or workflow toolbar)"
   - **Ambiguity**: How exactly do users enter this state? Button location? Workflow toolbar click?

### Statistics Display Ambiguities

7. **Statistics When POA Not Set**
   - POA is optional, but accuracy calculations presumably require it
   - **Ambiguity**: What statistics are shown/hidden when POA is not set?
   - **Ambiguity**: Are accuracy vs precision metrics clearly distinguished?

8. **"Shots: 5 / ?" Format**
   - Line 534: "Shots: 5 / ?"
   - **Ambiguity**: What does the "?" represent? Total expected? Maximum? Placeholder for future feature?

9. **Statistics Update Timing**
   - Real-time updates mentioned, but:
   - **Ambiguity**: Are statistics calculated synchronously (blocking UI) or asynchronously?
   - **Ambiguity**: What happens during calculation? Loading indicator?

### Visual Design Ambiguities

10. **Cursor Size Specifications**
    - Various cursor sizes mentioned but inconsistent:
      - POA cursor: "~30-40 pixels" (line 462)
      - ShotImpact cursor: "Scaled to bullet diameter" (line 523)
    - **Ambiguity**: Are these fixed sizes or relative? How is "scaled to bullet diameter" calculated exactly?

11. **Segmented Circle Gap Size**
    - Line 736: "Short gaps (~10-15° each)"
    - **Ambiguity**: Exact gap size? Configurable? Consistent across all sizes?

12. **Font Specifications**
    - Fonts mentioned but not fully specified:
      - "Bold, sans-serif" (line 748)
      - "Clean, readable typography (sans-serif font)" (line 754)
    - **Ambiguity**: Specific font family? Size? Platform-specific fonts?

### File Operation Ambiguities

13. **Save As Default Filename**
    - Line 128: "Pre-populated filename: Current moniker filename (if exists)"
    - **Ambiguity**: What if ShotGroupDocument has no moniker? Empty? Suggested name based on image filename?

14. **Drag and Drop File Validation**
    - Line 214: "Validate file type before processing"
    - **Ambiguity**: What happens if invalid file type dropped? Error message? Silent rejection?

15. **Multiple File Drag and Drop**
    - Line 216-217: Recommendation given but marked with "?"
    - **Ambiguity**: Is this the final decision or still under consideration?

### Configuration Ambiguities

16. **Display Units Scope**
    - Line 596: "application-wide (stored in QSettings)"
    - **Ambiguity**: If user changes units, do all open documents update immediately?
    - **Ambiguity**: What if user has multiple documents open?

17. **Settings Persistence**
    - Some settings are application-wide (QSettings), some are per-document (.spz)
    - **Ambiguity**: Clear distinction not always obvious. Which settings are which?

18. **Default Settings Values**
    - Configuration Settings section lists options but not defaults
    - **Ambiguity**: What are the default values for all settings?

### Workflow Ambiguities

19. **Cancel Behavior in Calibration Wizard**
    - Calibration wizard mentioned but cancel behavior not specified
    - **Ambiguity**: What happens if user cancels? Return to empty ShotGroupDocument? Previous state?

20. **Undo Behavior Scope**
    - Undo specified for Mark Impacts but scope unclear
    - **Ambiguity**: Single operation undo or multi-step undo stack?
    - **Ambiguity**: Can users undo across state transitions?

---

## 4. MISSING WORKFLOWS OR INTERACTION SCENARIOS

### Critical Missing Workflows

1. **First-Time User Onboarding**
   - No workflow for new users
   - Missing: Welcome screen? Tutorial? Help system access?

2. **Error Recovery Workflow**
   - What to do when things go wrong
   - Missing: Step-by-step error recovery procedures

3. **Batch Processing Workflow**
   - Can users process multiple targets?
   - Missing: Workflow for analyzing multiple sessions

4. **Export with Custom Plaque Workflow**
   - Statistics plaque system described but workflow missing
   - Missing: Step-by-step plaque configuration and export

5. **Re-calibration After Mistakes**
   - What if user realizes calibration is wrong after marking ShotImpacts?
   - Missing: Detailed re-calibration workflow with ShotImpact preservation

6. **Changing Units Mid-Session**
   - Display units can be changed, but workflow not detailed
   - Missing: How this affects existing displays and calculations

### Missing Edge Case Scenarios

7. **Empty Document State**
   - What can users do with empty ShotGroupDocument?
   - Missing: Valid operations in empty state

8. **Single ShotImpact Scenario**
   - What statistics are shown with only 1 ShotImpact?
   - Missing: Behavior specification

9. **Two ShotImpact Scenario**
   - Some statistics may be undefined or trivial
   - Missing: What's displayed?

10. **All ShotImpacts Collinear**
    - Edge case for group circle calculation
    - Missing: Handling specification

11. **Extremely Large/Small Scale Factors**
    - What if calibration produces unrealistic values?
    - Missing: Validation and error handling

12. **Very Large Number of ShotImpacts**
   - Performance considerations?
   - Missing: Limits, performance, UI behavior with many ShotImpacts

13. **Overlapping Impact Glyphs**
    - Can ShotImpacts be placed very close together?
    - Missing: Visual handling, selection behavior

14. **ShotImpact Outside Image Bounds**
    - Can users mark ShotImpacts outside the target image?
    - Missing: Boundary handling

### Missing Advanced Scenarios

15. **Keyboard-Only Navigation**
    - Accessibility mentioned but workflow missing
    - Missing: Complete keyboard navigation workflow

16. **High Contrast Mode Workflow**
    - Accessibility feature mentioned but not detailed
    - Missing: How to enable, what changes

17. **Custom Caliber Workflow**
    - Custom caliber input mentioned but workflow not detailed
    - Missing: Input validation, format, range

18. **Statistics Plaque Template Management**
    - Templates mentioned but not detailed
    - Missing: How to save/load/manage templates

19. **Export Format Comparison**
    - Multiple formats mentioned but comparison missing
    - Missing: When to use which format, trade-offs

20. **Session Comparison Workflow**
    - Can users compare multiple sessions?
    - Missing: Multi-session analysis workflow (if supported)

---

## 5. INCONSISTENCIES AND CONTRADICTIONS

1. **Reference Line Color**: Green vs Yellow (see Ambiguities #1)

2. **ShotImpact Marker Color**: Color A vs Red/Yellow (see Ambiguities #2)

3. **Statistics Plaque Position**
   - Line 838: "X, Y coordinates (or drag on target preview)"
   - **Inconsistency**: Two different interaction methods mentioned - which is primary?

4. **Pan Method**
   - Line 666: "Left mouse drag: Pan the view (when drag delta exceeds threshold)"
   - Line 515: "Middle-drag or Ctrl+drag to pan"
   - **Inconsistency**: Does left-drag also pan, or only middle-drag/Ctrl+drag?

5. **Workflow Toolbar Interactivity**
   - Line 265: "Interactive: Users can click on enabled states to navigate"
   - Line 324: "Manual: Click any enabled state in workflow toolbar to jump to it"
   - **Consistency**: This is consistent, but could be clearer about disabled state behavior

---

## 6. RECOMMENDATIONS FOR COMPLETION

### High Priority (Blocking Implementation)

1. **Define State 5: Edit Visualization**
   - Complete UI specification
   - Interaction patterns
   - Entry/exit behavior

2. **Resolve Color Ambiguities**
   - Define Color A explicitly
   - Resolve reference line color conflict
   - Specify all color values definitively

3. **Specify Export Dialog**
   - Complete UI layout
   - All controls and options
   - Error handling

4. **Define Metadata Dialog**
   - Complete specification
   - Field definitions
   - Validation rules

5. **Clarify State Transitions**
   - All transition paths documented
   - Edge cases handled
   - Cancel behaviors specified

### Medium Priority (Important for UX)

6. **Add Error Handling Specifications**
   - All error scenarios
   - User-facing messages
   - Recovery procedures

7. **Complete Missing Workflows**
   - Error recovery
   - Re-calibration
   - Metadata editing
   - Preferences configuration

8. **Specify Edge Cases**
   - Zero/one/two ShotImpact scenarios
   - Invalid data states
   - Boundary conditions

9. **Define UI Component Details**
   - Workflow toolbar visual design
   - Side panel layout
   - Status bar message formats

10. **Resolve Interaction Ambiguities**
    - Click vs drag thresholds
    - Pan methods
    - Selection behaviors

### Low Priority (Polish and Completeness)

11. **Add Advanced Scenarios**
    - Batch processing (if supported)
    - Session comparison (if supported)
    - Template management

12. **Complete Accessibility Details**
    - Keyboard navigation workflow
    - High contrast mode details
    - Screen reader support (if applicable)

13. **Add Help System Specification**
    - Help menu structure
    - Context-sensitive help
    - User guide integration

14. **Specify Performance Considerations**
    - Large number of ShotImpacts
    - Calculation performance
    - UI responsiveness

---

## SUMMARY

The UI_WORKFLOWS.md document provides a solid foundation for the ShotPlot application's user interface design, covering most core workflows and interaction patterns. However, several critical areas need completion:

- **1 major gap**: State 5 (Edit Visualization) is completely undefined
- **15+ design questions** remain unanswered
- **20+ coverage gaps** identified in workflows, error handling, and edge cases
- **20+ ambiguities** need clarification
- **5+ inconsistencies** need resolution

The document would benefit from:
1. Completing the Visualization state specification
2. Resolving all color ambiguities
3. Adding comprehensive error handling specifications
4. Defining all missing workflows
5. Clarifying edge cases and boundary conditions
6. Specifying UI component details more completely

Most gaps are in advanced features, error handling, and edge cases rather than core functionality, suggesting the document is ready for initial implementation but will need iteration as development progresses.
