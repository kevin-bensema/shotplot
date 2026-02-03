#include "MainWindow.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QActionGroup>
#include <QStandardPaths>

#include "Widgets/TargetView.h"
#include "Widgets/WorkflowToolbar.h"
#include "Widgets/PerStateToolbar.h"
#include "Widgets/StatisticsPanel.h"
#include "Graphics/TargetScene.h"
#include "Core/ShotGroupDocument.h"

#include "States/WorkflowState.h"
#include "States/SetCaliberState.h"
#include "States/ScaleFactorState.h"
#include "States/POAState.h"
#include "States/MarkImpactsState.h"
#include "States/VisualizationState.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_undoStack(new QUndoStack(this))
{
    setupUi();
    setupMenus();
    setupToolbars();
    setupCentralWidget();
    setupDockWidgets();
    setupConnections();
    
    // Enable drag and drop
    setAcceptDrops(true);
    
    // Initial state
    updateWindowTitle();
    updateMenuState();
    
    // Set reasonable default size
    resize(1200, 800);
    
    statusBar()->showMessage(tr("No document loaded"));
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    // Create undo stack actions
    m_undoAction = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_undoAction->setShortcut(QKeySequence::Undo);
    
    m_redoAction = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_redoAction->setShortcut(QKeySequence::Redo);
}

void MainWindow::setupMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction *importAction = fileMenu->addAction(tr("&Import..."), this, &MainWindow::onImport);
    importAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    
    QAction *loadAction = fileMenu->addAction(tr("&Load..."), this, &MainWindow::onLoad);
    loadAction->setShortcut(QKeySequence::Open);
    
    m_saveAction = fileMenu->addAction(tr("&Save"), this, &MainWindow::onSave);
    m_saveAction->setShortcut(QKeySequence::Save);
    
    m_saveAsAction = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onSaveAs);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    
    fileMenu->addSeparator();
    
    m_exportAction = fileMenu->addAction(tr("&Export Image..."), this, &MainWindow::onExportImage);
    
    fileMenu->addSeparator();
    
    QAction *quitAction = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAction->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    
    editMenu->addSeparator();
    
    m_metadataAction = editMenu->addAction(tr("&Metadata..."), this, &MainWindow::onEditMetadata);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    
    m_zoomInAction = viewMenu->addAction(tr("Zoom &In"), this, &MainWindow::onZoomIn);
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    
    m_zoomOutAction = viewMenu->addAction(tr("Zoom &Out"), this, &MainWindow::onZoomOut);
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    
    m_zoomFitAction = viewMenu->addAction(tr("Zoom &Fit"), this, &MainWindow::onZoomFit);
    m_zoomFitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    
    viewMenu->addSeparator();
    
    // Display units submenu
    QMenu *unitsMenu = viewMenu->addMenu(tr("Display &Units"));
    QActionGroup *unitsGroup = new QActionGroup(this);
    unitsGroup->setExclusive(true);
    
    QAction *inchesAction = unitsMenu->addAction(tr("Inches"));
    inchesAction->setCheckable(true);
    inchesAction->setData("inches");
    unitsGroup->addAction(inchesAction);
    
    QAction *moaAction = unitsMenu->addAction(tr("MOA"));
    moaAction->setCheckable(true);
    moaAction->setData("moa");
    unitsGroup->addAction(moaAction);
    
    QAction *mradAction = unitsMenu->addAction(tr("MRAD"));
    mradAction->setCheckable(true);
    mradAction->setData("mrad");
    unitsGroup->addAction(mradAction);
    
    // Load saved preference
    QSettings settings;
    QString savedUnit = settings.value("display/units", "inches").toString();
    if (savedUnit == "moa")
    {
        moaAction->setChecked(true);
    }
    else if (savedUnit == "mrad")
    {
        mradAction->setChecked(true);
    }
    else
    {
        inchesAction->setChecked(true);
    }
    
    connect(unitsGroup, &QActionGroup::triggered, this, &MainWindow::onDisplayUnitsChanged);
}

void MainWindow::setupToolbars()
{
    // Workflow toolbar (pipeline visualization)
    m_workflowToolbar = new WorkflowToolbar(this);
    addToolBar(Qt::TopToolBarArea, m_workflowToolbar);
    
    // Force per-state toolbar onto a new row
    addToolBarBreak(Qt::TopToolBarArea);
    
    // Per-state toolbar (state-specific controls)
    m_perStateToolbar = new PerStateToolbar(this);
    addToolBar(Qt::TopToolBarArea, m_perStateToolbar);
}

void MainWindow::setupCentralWidget()
{
    // Create the graphics scene and view
    m_targetScene = new TargetScene(this);
    m_targetView = new TargetView(this);
    m_targetView->setScene(m_targetScene);
    
    setCentralWidget(m_targetView);
}

void MainWindow::setupDockWidgets()
{
    // Statistics panel (right dock)
    QDockWidget *statsDock = new QDockWidget(tr("Statistics"), this);
    statsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    
    m_statisticsPanel = new StatisticsPanel(this);
    statsDock->setWidget(m_statisticsPanel);
    
    addDockWidget(Qt::RightDockWidgetArea, statsDock);
}

void MainWindow::setupConnections()
{
    // Workflow toolbar state selection
    connect(m_workflowToolbar, &WorkflowToolbar::stateSelected,
            this, &MainWindow::onWorkflowStateChanged);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        const QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QString path = urls.first().toLocalFile().toLower();
            if (path.endsWith(".png") || path.endsWith(".jpg") || 
                path.endsWith(".jpeg") || path.endsWith(".spz"))
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    
    QString path = urls.first().toLocalFile();
    
    if (path.toLower().endsWith(".spz"))
    {
        // Load session file
        if (maybeSave())
        {
            // TODO: Load document from path
            statusBar()->showMessage(tr("Loaded: %1").arg(path));
        }
    }
    else
    {
        // Import image file
        if (maybeSave())
        {
            QImage image(path);
            if (!image.isNull())
            {
                createNewDocument(image);
                statusBar()->showMessage(tr("Imported: %1").arg(path));
            }
            else
            {
                QMessageBox::warning(this, tr("Import Error"),
                    tr("Could not load image: %1").arg(path));
            }
        }
    }
}

void MainWindow::onImport()
{
    if (!maybeSave()) return;
    
    QSettings settings;
    QString lastDir = settings.value("directories/lastImport",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    
    QString path = QFileDialog::getOpenFileName(this, tr("Import Target Image"),
        lastDir, tr("Images (*.png *.jpg *.jpeg);;All Files (*)"));
    
    if (path.isEmpty()) return;
    
    // Save directory preference
    settings.setValue("directories/lastImport", QFileInfo(path).absolutePath());
    
    QImage image(path);
    if (image.isNull())
    {
        QMessageBox::warning(this, tr("Import Error"),
            tr("Could not load image: %1").arg(path));
        return;
    }
    
    createNewDocument(image);
    statusBar()->showMessage(tr("Imported: %1").arg(path));
}

void MainWindow::onLoad()
{
    if (!maybeSave()) return;
    
    QSettings settings;
    QString lastDir = settings.value("directories/lastSaveLoad",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    
    QString path = QFileDialog::getOpenFileName(this, tr("Open Session"),
        lastDir, tr("ShotPlot Sessions (*.spz);;All Files (*)"));
    
    if (path.isEmpty()) return;
    
    // Save directory preference
    settings.setValue("directories/lastSaveLoad", QFileInfo(path).absolutePath());
    
    // TODO: Load document from path using DocumentSerializer
    statusBar()->showMessage(tr("Loaded: %1").arg(path));
}

void MainWindow::onSave()
{
    if (!m_document) return;
    
    if (!m_document->hasFilePath())
    {
        onSaveAs();
        return;
    }
    
    QString errorMsg;
    if (!m_document->saveToFile(m_document->filePath(), &errorMsg))
    {
        QMessageBox::warning(this, tr("Save Error"), errorMsg);
        return;
    }
    
    m_document->setDirty(false);
    statusBar()->showMessage(tr("Saved: %1").arg(m_document->filePath()));
}

void MainWindow::onSaveAs()
{
    if (!m_document) return;
    
    QSettings settings;
    QString lastDir = settings.value("directories/lastSaveLoad",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    
    QString path = QFileDialog::getSaveFileName(this, tr("Save Session As"),
        lastDir, tr("ShotPlot Sessions (*.spz)"));
    
    if (path.isEmpty()) return;
    
    // Ensure .spz extension
    if (!path.toLower().endsWith(".spz"))
    {
        path += ".spz";
    }
    
    // Save directory preference
    settings.setValue("directories/lastSaveLoad", QFileInfo(path).absolutePath());
    
    QString errorMsg;
    if (!m_document->saveToFile(path, &errorMsg))
    {
        QMessageBox::warning(this, tr("Save Error"), errorMsg);
        return;
    }
    
    m_document->setFilePath(path);
    m_document->setDirty(false);
    updateWindowTitle();
    statusBar()->showMessage(tr("Saved: %1").arg(path));
}

void MainWindow::onExportImage()
{
    // TODO: Implement image export
    QMessageBox::information(this, tr("Export"), tr("Export functionality not yet implemented."));
}

void MainWindow::onEditMetadata()
{
    // TODO: Show metadata editor dialog
    QMessageBox::information(this, tr("Metadata"), tr("Metadata editor not yet implemented."));
}

void MainWindow::onZoomIn()
{
    if (m_targetView)
    {
        m_targetView->zoomIn();
    }
}

void MainWindow::onZoomOut()
{
    if (m_targetView)
    {
        m_targetView->zoomOut();
    }
}

void MainWindow::onZoomFit()
{
    if (m_targetView)
    {
        m_targetView->zoomFit();
    }
}

void MainWindow::onDisplayUnitsChanged(QAction *action)
{
    QSettings settings;
    settings.setValue("display/units", action->data().toString());
    
    // Update statistics panel
    if (m_statisticsPanel)
    {
        m_statisticsPanel->updateDisplay();
    }
}

void MainWindow::onDocumentDirtyChanged(bool dirty)
{
    Q_UNUSED(dirty)
    updateWindowTitle();
    updateMenuState();
}

void MainWindow::onWorkflowStateChanged(int stateIndex)
{
    setCurrentState(stateIndex);
}

void MainWindow::updateWindowTitle()
{
    QString title = "ShotPlot";
    
    if (m_document)
    {
        if (m_document->hasFilePath())
        {
            title += " - " + QFileInfo(m_document->filePath()).fileName();
        }
        else
        {
            title += " - " + tr("Untitled");
        }
        
        if (m_document->isDirty())
        {
            title += "*";
        }
    }
    
    setWindowTitle(title);
}

void MainWindow::updateMenuState()
{
    bool hasDocument = (m_document != nullptr);
    bool isDirty = hasDocument && m_document->isDirty();
    
    m_saveAction->setEnabled(isDirty);
    m_saveAsAction->setEnabled(hasDocument);
    m_exportAction->setEnabled(hasDocument);
    m_metadataAction->setEnabled(hasDocument);
    m_zoomInAction->setEnabled(hasDocument);
    m_zoomOutAction->setEnabled(hasDocument);
    m_zoomFitAction->setEnabled(hasDocument);
}

bool MainWindow::maybeSave()
{
    if (!m_document || !m_document->isDirty())
    {
        return true;
    }
    
    QMessageBox::StandardButton result = QMessageBox::warning(this, tr("Unsaved Changes"),
        tr("You have unsaved changes. Do you want to save before continuing?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    switch (result)
    {
        case QMessageBox::Save:
            onSave();
            return !m_document->isDirty();
        case QMessageBox::Discard:
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
    }
}

void MainWindow::setCurrentState(int stateIndex)
{
    if (stateIndex < 0 || stateIndex >= static_cast<int>(m_states.size()))
    {
        return;
    }
    
    // Exit current state
    if (m_currentStateIndex >= 0 && m_currentStateIndex < static_cast<int>(m_states.size()))
    {
        m_states[m_currentStateIndex]->onExit();
    }
    
    m_currentStateIndex = stateIndex;
    
    // Enter new state
    m_states[m_currentStateIndex]->onEnter();
    
    // Update UI
    m_workflowToolbar->setCurrentState(stateIndex);
    m_perStateToolbar->setCurrentState(m_states[stateIndex].get());
    
    // Update target view with current state
    m_targetView->setWorkflowState(m_states[stateIndex].get());
}

void MainWindow::createNewDocument(const QImage &image)
{
    // Create new document
    m_document = std::make_unique<ShotGroupDocument>();
    m_document->setTargetImage(image);
    
    // Connect document signals
    connect(m_document.get(), &ShotGroupDocument::dirtyChanged,
            this, &MainWindow::onDocumentDirtyChanged);
    
    // Set up scene with image
    m_targetScene->setTargetImage(image);
    m_targetView->zoomFit();
    
    // Create workflow states
    m_states.clear();
    m_states.push_back(std::make_unique<SetCaliberState>(m_document.get(), m_targetView, this));
    m_states.push_back(std::make_unique<ScaleFactorState>(m_document.get(), m_targetView, m_targetScene));
    m_states.push_back(std::make_unique<POAState>(m_document.get(), m_targetView, m_targetScene));
    m_states.push_back(std::make_unique<MarkImpactsState>(m_document.get(), m_targetView, m_targetScene, m_undoStack));
    m_states.push_back(std::make_unique<VisualizationState>(m_document.get(), m_targetView, m_targetScene));
    
    // Update workflow toolbar
    m_workflowToolbar->setDocument(m_document.get());
    
    // Start with caliber state
    setCurrentState(0);
    
    // Mark as dirty (new unsaved document)
    m_document->setDirty(true);
    
    updateWindowTitle();
    updateMenuState();
}
