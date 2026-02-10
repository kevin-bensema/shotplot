## List of known bugs to work on.

### Double import crash
- Open ShotPlot
- Import an image with Ctrl+I and pick a caliber.
- Import another image with Ctrl+I, select discard when asked to save
- Crash.

### Drag and drop import offset.
- Open ShotPlot.
- Import an image via drag and drop.
- The image will not be centered in the view.

Probably due to the eventFilter() drag-and-drop method. I think the QGraphicsView's mouse cursor position is not being updated during the drag event, and then when the cursor materializes in the middle of the view without an event, it has an inconsistent state.