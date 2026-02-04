#pragma once

#include <QMainWindow>
#include <QUndoStack>
#include <memory>

class TargetView;
class TargetScene;
class WorkflowToolbar;
class PerStateToolbar;
class StatisticsPanel;
class ShotGroupDocument;
class WorkflowState;

/// @brief Main application window for ShotPlot
/// 
/// Manages the application's UI, document lifecycle, workflow state transitions,
/// and user interactions. Coordinates between the document model, graphics scene/view,
/// toolbars, and workflow states. Handles file operations (import, load, save, export),
/// menu actions, zoom controls, and drag-and-drop file import.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /// Handles window close events, prompting to save unsaved changes if needed
    void closeEvent(QCloseEvent *event) override;
    
    /// Handles drag enter events for file import (accepts image files and .spz session files)
    void dragEnterEvent(QDragEnterEvent *event) override;
    
    /// Handles drop events to import images or load session files
    void dropEvent(QDropEvent *event) override;

private slots:
    // File menu actions
    /// Opens file dialog to import a target image (PNG, JPG, JPEG)
    void onImport();
    
    /// Opens file dialog to load a saved session file (.spz)
    void onLoad();
    
    /// Saves the current document to its existing file path, or prompts for Save As if no path set
    void onSave();
    
    /// Prompts user for file path and saves the current document as a new session file
    void onSaveAs();
    
    /// Exports the current target view as an image file (not yet implemented)
    void onExportImage();
    
    // Edit menu actions
    /// Opens the metadata editor dialog for editing session metadata
    void onEditMetadata();
    
    // View menu actions
    /// Increases the zoom level of the target view
    void onZoomIn();
    
    /// Decreases the zoom level of the target view
    void onZoomOut();
    
    /// Fits the entire target image to the view
    void onZoomFit();
    
    /// Updates display units preference (inches, MOA, or MRAD) and refreshes statistics panel
    void onDisplayUnitsChanged(QAction* action);

private:
    /// Initializes undo/redo actions
    void setupUi();
    
    /// Creates and configures the menu bar (File, Edit, View menus)
    void setupMenus();
    
    /// Creates and configures toolbars (workflow toolbar and per-state toolbar)
    void setupToolbars();
    
    /// Creates the central graphics view and scene for displaying the target
    void setupCentralWidget();
    
    /// Creates dock widgets (statistics panel)
    void setupDockWidgets();
    
    /// Connects signals between UI components and slots
    void setupConnections();
    
    /// Updates the window title to reflect the current document name and dirty state
    void updateWindowTitle();
    
    /// Enables/disables menu actions based on document state
    void updateMenuState();
    
    /// Prompts user to save unsaved changes if document is dirty. Returns true if operation can proceed.
    bool maybeSave();
    
    /// Transitions to the specified workflow state, calling onExit/onEnter handlers
    void setCurrentState(int stateIndex);
    
    /// Creates a new document from an imported image and initializes workflow states
    void createNewDocument(const QImage &image);
    
private:
    // UI Components
    TargetView* m_pTargetView = nullptr;              ///< Graphics view displaying the target scene
    TargetScene* m_pTargetScene = nullptr;            ///< Graphics scene containing target image and impact markers
    WorkflowToolbar* m_pWorkflowToolbar = nullptr;    ///< Toolbar showing workflow pipeline and state selection
    PerStateToolbar* m_pPerStateToolbar = nullptr;    ///< Toolbar with state-specific controls
    StatisticsPanel* m_pStatisticsPanel = nullptr;    ///< Dock widget displaying shot group statistics
    
    // Document
    std::unique_ptr<ShotGroupDocument> m_document;   ///< Current document model (null if no document loaded)
    QUndoStack* m_pUndoStack = nullptr;               ///< Undo/redo command stack for document operations
    
    // Workflow states
    std::vector<std::unique_ptr<WorkflowState>> m_states;  ///< All workflow states (caliber, scale, POA, impacts, visualization)
    int m_currentStateIndex = -1;                    ///< Index of the currently active workflow state (-1 if none)
    
    // Menu actions (for enable/disable management)
    QAction* m_pSaveAction = nullptr;                 ///< Save menu action (enabled when document is dirty)
    QAction* m_pSaveAsAction = nullptr;               ///< Save As menu action (enabled when document exists)
    QAction* m_pExportAction = nullptr;               ///< Export Image menu action (enabled when document exists)
    QAction* m_pUndoAction = nullptr;                 ///< Undo action from undo stack
    QAction* m_pRedoAction = nullptr;                 ///< Redo action from undo stack
    QAction* m_pMetadataAction = nullptr;             ///< Edit Metadata menu action (enabled when document exists)
    QAction* m_pZoomInAction = nullptr;               ///< Zoom In menu action (enabled when document exists)
    QAction* m_pZoomOutAction = nullptr;              ///< Zoom Out menu action (enabled when document exists)
    QAction* m_pZoomFitAction = nullptr;              ///< Zoom Fit menu action (enabled when document exists)
};
