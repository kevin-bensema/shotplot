#include "MainWindow.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QActionGroup>
#include <QStandardPaths>

namespace {
    constexpr int kDefaultWindowWidth = 1200;
    constexpr int kDefaultWindowHeight = 800;
    
    constexpr const char* kImageExtensionPng = ".png";
    constexpr const char* kImageExtensionJpg = ".jpg";
    constexpr const char* kImageExtensionJpeg = ".jpeg";
    constexpr const char* kSessionExtension = ".spz";
    
    constexpr const char* kSettingsDisplayUnits = "display/units";
    constexpr const char* kSettingsLastImportDir = "directories/lastImport";
    constexpr const char* kSettingsLastSaveLoadDir = "directories/lastSaveLoad";
    constexpr const char* kDefaultDisplayUnit = "inches";
}

#include <Widgets/TargetView.h>
#include <Widgets/WorkflowToolbar.h>
#include <Widgets/PerStateToolbar.h>
#include <Widgets/StatisticsPanel.h>
#include <Widgets/GraphicsSettingsDialog.h>
#include <Graphics/TargetScene.h>
#include <Core/ShotGroupDocument.h>

#include <States/WorkflowState.h>
#include <States/SetCaliberState.h>
#include <States/ScaleFactorState.h>
#include <States/POAState.h>
#include <States/MarkImpactsState.h>
#include <States/VisualizationState.h>

MainWindow::MainWindow(QWidget* pParent)
    : QMainWindow(pParent)
    , m_pUndoStack(new QUndoStack(this))
{
    setupUi();
    setupMenus();
    setupToolbars();
    setupCentralWidget();
    setupDockWidgets();
    setupConnections();
    
    // Initial state
    updateWindowTitle();
    updateMenuState();
    
    // Set reasonable default size
    resize(kDefaultWindowWidth, kDefaultWindowHeight);
    
    statusBar()->showMessage(tr("No document loaded"));
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    // Create undo stack actions
    m_pUndoAction = m_pUndoStack->createUndoAction(this, tr("&Undo"));
    m_pUndoAction->setShortcut(QKeySequence::Undo);
    
    m_pRedoAction = m_pUndoStack->createRedoAction(this, tr("&Redo"));
    m_pRedoAction->setShortcut(QKeySequence::Redo);
}

void MainWindow::setupMenus()
{
    // File menu
    QMenu* pFileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction* pImportAction = pFileMenu->addAction(tr("&Import..."), this, &MainWindow::onImport);
    pImportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    
    QAction* pLoadAction = pFileMenu->addAction(tr("&Load..."), this, &MainWindow::onLoad);
    pLoadAction->setShortcut(QKeySequence::Open);
    
    m_pSaveAction = pFileMenu->addAction(tr("&Save"), this, &MainWindow::onSave);
    m_pSaveAction->setShortcut(QKeySequence::Save);
    
    m_pSaveAsAction = pFileMenu->addAction(tr("Save &As..."), this, &MainWindow::onSaveAs);
    m_pSaveAsAction->setShortcut(QKeySequence::SaveAs);
    
    pFileMenu->addSeparator();
    
    m_pExportAction = pFileMenu->addAction(tr("&Export Image..."), this, &MainWindow::onExportImage);
    
    pFileMenu->addSeparator();
    
    QAction* pQuitAction = pFileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    pQuitAction->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu* pEditMenu = menuBar()->addMenu(tr("&Edit"));
    
    pEditMenu->addAction(m_pUndoAction);
    pEditMenu->addAction(m_pRedoAction);
    
    pEditMenu->addSeparator();
    
    m_pMetadataAction = pEditMenu->addAction(tr("&Metadata..."), this, &MainWindow::onEditMetadata);
    
    // View menu
    QMenu* pViewMenu = menuBar()->addMenu(tr("&View"));
    
    m_pZoomInAction = pViewMenu->addAction(tr("Zoom &In"), this, &MainWindow::onZoomIn);
    m_pZoomInAction->setShortcut(QKeySequence::ZoomIn);
    
    m_pZoomOutAction = pViewMenu->addAction(tr("Zoom &Out"), this, &MainWindow::onZoomOut);
    m_pZoomOutAction->setShortcut(QKeySequence::ZoomOut);
    
    m_pZoomFitAction = pViewMenu->addAction(tr("Zoom &Fit"), this, &MainWindow::onZoomFit);
    m_pZoomFitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    
    pViewMenu->addSeparator();
    
    // Display units submenu
    QMenu* pUnitsMenu = pViewMenu->addMenu(tr("Display &Units"));
    QActionGroup* pUnitsGroup = new QActionGroup(this);
    pUnitsGroup->setExclusive(true);
    
    QAction* pInchesAction = pUnitsMenu->addAction(tr("Inches"));
    pInchesAction->setCheckable(true);
    pInchesAction->setData("inches");
    pUnitsGroup->addAction(pInchesAction);
    
    QAction* pMoaAction = pUnitsMenu->addAction(tr("MOA"));
    pMoaAction->setCheckable(true);
    pMoaAction->setData("moa");
    pUnitsGroup->addAction(pMoaAction);
    
    QAction* pMradAction = pUnitsMenu->addAction(tr("MRAD"));
    pMradAction->setCheckable(true);
    pMradAction->setData("mrad");
    pUnitsGroup->addAction(pMradAction);
    
    // Load saved preference
    QSettings settings;
    QString savedUnit = settings.value(kSettingsDisplayUnits, kDefaultDisplayUnit).toString();
    if (savedUnit == "moa")
    {
        pMoaAction->setChecked(true);
    }
    else if (savedUnit == "mrad")
    {
        pMradAction->setChecked(true);
    }
    else
    {
        pInchesAction->setChecked(true);
    }
    
    connect(pUnitsGroup, &QActionGroup::triggered, this, &MainWindow::onDisplayUnitsChanged);
    
    pViewMenu->addSeparator();
    
    QAction* pGraphicsSettingsAction = pViewMenu->addAction(tr("&Graphics Settings..."), this, &MainWindow::onGraphicsSettings);
}

void MainWindow::setupToolbars()
{
    // Workflow toolbar (pipeline visualization)
    m_pWorkflowToolbar = new WorkflowToolbar(this);
    addToolBar(Qt::TopToolBarArea, m_pWorkflowToolbar);
    
    // Force per-state toolbar onto a new row
    addToolBarBreak(Qt::TopToolBarArea);
    
    // Per-state toolbar (state-specific controls)
    m_pPerStateToolbar = new PerStateToolbar(this);
    addToolBar(Qt::TopToolBarArea, m_pPerStateToolbar);
}

void MainWindow::setupCentralWidget()
{
    // Create the graphics scene and view
    m_pTargetScene = new TargetScene(this);
    m_pTargetView = new TargetView(this);
    m_pTargetView->setTargetScene(m_pTargetScene);
    
    // Enable drag and drop on TargetView's viewport (QGraphicsView uses a viewport widget)
    m_pTargetView->viewport()->setAcceptDrops(true);
    m_pTargetView->viewport()->installEventFilter(this);
    
    setCentralWidget(m_pTargetView);
}

void MainWindow::setupDockWidgets()
{
    // Statistics panel (right dock)
    QDockWidget* pStatsDock = new QDockWidget(tr("Statistics"), this);
    pStatsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    
    m_pStatisticsPanel = new StatisticsPanel(this);
    pStatsDock->setWidget(m_pStatisticsPanel);
    
    addDockWidget(Qt::RightDockWidgetArea, pStatsDock);
}

void MainWindow::setupConnections()
{
    // Workflow toolbar state selection
    connect(m_pWorkflowToolbar, &WorkflowToolbar::stateSelected,
            this, [this](int stateIndex) {
                setCurrentState(stateIndex);
            });
}

void MainWindow::closeEvent(QCloseEvent* event)
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

bool MainWindow::eventFilter(QObject* pObject, QEvent* pEvent)
{
    // Intercept drag-and-drop events on TargetView's viewport
    if (m_pTargetView && pObject == m_pTargetView->viewport())
    {
        if (pEvent->type() == QEvent::DragEnter)
        {
            QDragEnterEvent* pDragEvent = static_cast<QDragEnterEvent*>(pEvent);
            if (pDragEvent->mimeData()->hasUrls())
            {
                const QList<QUrl> urls = pDragEvent->mimeData()->urls();
                if (!urls.isEmpty())
                {
                    QString path = urls.first().toLocalFile().toLower();
                    if (path.endsWith(kImageExtensionPng) || path.endsWith(kImageExtensionJpg) || 
                        path.endsWith(kImageExtensionJpeg) || path.endsWith(kSessionExtension))
                    {
                        pDragEvent->acceptProposedAction();
                        return true;  // Event handled
                    }
                }
            }
            pDragEvent->ignore();
            return true;  // Event handled
        }
        else if (pEvent->type() == QEvent::DragMove)
        {
            QDragMoveEvent* pDragEvent = static_cast<QDragMoveEvent*>(pEvent);
            if (pDragEvent->mimeData()->hasUrls())
            {
                const QList<QUrl> urls = pDragEvent->mimeData()->urls();
                if (!urls.isEmpty())
                {
                    QString path = urls.first().toLocalFile().toLower();
                    if (path.endsWith(kImageExtensionPng) || path.endsWith(kImageExtensionJpg) || 
                        path.endsWith(kImageExtensionJpeg) || path.endsWith(kSessionExtension))
                    {
                        pDragEvent->acceptProposedAction();
                        return true;  // Event handled
                    }
                }
            }
            pDragEvent->ignore();
            return true;  // Event handled
        }
        else if (pEvent->type() == QEvent::Drop)
        {
            QDropEvent* pDropEvent = static_cast<QDropEvent*>(pEvent);
            const QList<QUrl> urls = pDropEvent->mimeData()->urls();
            if (!urls.isEmpty())
            {
                QString path = urls.first().toLocalFile();
                
                if (path.toLower().endsWith(kSessionExtension))
                {
                    // Load session file
                    if (maybeSave())
                    {
                        loadDocumentFromFile(path);
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
                pDropEvent->acceptProposedAction();
                return true;  // Event handled
            }
        }
    }
    
    // Pass event to base class
    return QMainWindow::eventFilter(pObject, pEvent);
}

void MainWindow::onImport()
{
    if (!maybeSave()) return;
    
    QSettings settings;
    QString lastDir = settings.value(kSettingsLastImportDir,
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    
    QString path = QFileDialog::getOpenFileName(this, tr("Import Target Image"),
        lastDir, tr("Images (*.png *.jpg *.jpeg);;All Files (*)"));
    
    if (path.isEmpty()) return;
    
    // Save directory preference
    settings.setValue(kSettingsLastImportDir, QFileInfo(path).absolutePath());
    
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
    QString lastDir = settings.value(kSettingsLastSaveLoadDir,
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    
    QString path = QFileDialog::getOpenFileName(this, tr("Open Session"),
        lastDir, tr("ShotPlot Sessions (*.spz);;All Files (*)"));
    
    if (path.isEmpty()) return;
    
    // Save directory preference
    settings.setValue(kSettingsLastSaveLoadDir, QFileInfo(path).absolutePath());
    
    loadDocumentFromFile(path);
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
    QString lastDir = settings.value(kSettingsLastSaveLoadDir,
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    
    QString path = QFileDialog::getSaveFileName(this, tr("Save Session As"),
        lastDir, tr("ShotPlot Sessions (*.spz)"));
    
    if (path.isEmpty()) return;
    
    // Ensure .spz extension
    if (!path.toLower().endsWith(kSessionExtension))
    {
        path += kSessionExtension;
    }
    
    // Save directory preference
    settings.setValue(kSettingsLastSaveLoadDir, QFileInfo(path).absolutePath());
    
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
    if (m_pTargetView)
    {
        m_pTargetView->zoomIn();
    }
}

void MainWindow::onZoomOut()
{
    if (m_pTargetView)
    {
        m_pTargetView->zoomOut();
    }
}

void MainWindow::onZoomFit()
{
    if (m_pTargetView)
    {
        m_pTargetView->zoomFit();
    }
}

void MainWindow::onDisplayUnitsChanged(QAction* action)
{
    QSettings settings;
    settings.setValue(kSettingsDisplayUnits, action->data().toString());
    
    // Update statistics panel
    if (m_pStatisticsPanel)
    {
        m_pStatisticsPanel->updateDisplay();
    }
}

void MainWindow::onGraphicsSettings()
{
    // Create dialog on first access (lazy initialization)
    if (!m_pGraphicsSettingsDialog)
    {
        m_pGraphicsSettingsDialog = new GraphicsSettingsDialog(this);
    }
    
    // Show non-modal dialog
    m_pGraphicsSettingsDialog->show();
    m_pGraphicsSettingsDialog->raise();
    m_pGraphicsSettingsDialog->activateWindow();
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
    
    m_pSaveAction->setEnabled(isDirty);
    m_pSaveAsAction->setEnabled(hasDocument);
    m_pExportAction->setEnabled(hasDocument);
    m_pMetadataAction->setEnabled(hasDocument);
    m_pZoomInAction->setEnabled(hasDocument);
    m_pZoomOutAction->setEnabled(hasDocument);
    m_pZoomFitAction->setEnabled(hasDocument);
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
    m_pWorkflowToolbar->setCurrentState(stateIndex);
    m_pPerStateToolbar->setCurrentState(m_states[stateIndex].get());
    
    // Update target view with current state
    m_pTargetView->setWorkflowState(m_states[stateIndex].get());
}

void MainWindow::createNewDocument(const QImage &image)
{
    // Create new document
    m_document = std::make_unique<ShotGroupDocument>();
    m_document->setTargetImage(image);
    
    // Connect document signals
    connect(m_document.get(), &ShotGroupDocument::dirtyChanged,
            this, [this](bool /*dirty*/) {
                updateWindowTitle();
                updateMenuState();
            });
    
    // Set up scene with image
    m_pTargetScene->setTargetImage(image);
    m_pTargetView->zoomFit();
    
    // Create workflow states
    m_states.clear();
    m_states.push_back(std::make_unique<SetCaliberState>(m_document.get(), m_pTargetView, this));
    m_states.push_back(std::make_unique<ScaleFactorState>(m_document.get(), m_pTargetView));
    m_states.push_back(std::make_unique<POAState>(m_document.get(), m_pTargetView));
    m_states.push_back(std::make_unique<MarkImpactsState>(m_document.get(), m_pTargetView, m_pUndoStack));
    m_states.push_back(std::make_unique<VisualizationState>(m_document.get(), m_pTargetView));
    
    // Update workflow toolbar
    m_pWorkflowToolbar->setDocument(m_document.get());
    
    // Update statistics panel
    m_pStatisticsPanel->setDocument(m_document.get());
    
    // Start with caliber state
    setCurrentState(0);
    
    // Mark as dirty (new unsaved document)
    m_document->setDirty(true);
    
    updateWindowTitle();
    updateMenuState();
}

void MainWindow::loadDocumentFromFile(const QString &filePath)
{
    // Create and load document
    auto document = std::make_unique<ShotGroupDocument>();
    QString errorMsg;
    if (!document->loadFromFile(filePath, &errorMsg))
    {
        QMessageBox::warning(this, tr("Load Error"), errorMsg);
        return;
    }
    
    // Take ownership
    m_document = std::move(document);
    m_document->setFilePath(filePath);
    
    // Connect document signals
    connect(m_document.get(), &ShotGroupDocument::dirtyChanged,
            this, [this](bool /*dirty*/) {
                updateWindowTitle();
                updateMenuState();
            });
    
    // Set up scene with loaded image
    m_pTargetScene->setTargetImage(m_document->targetImage());
    m_pTargetView->zoomFit();
    
    // Create workflow states
    m_states.clear();
    m_states.push_back(std::make_unique<SetCaliberState>(m_document.get(), m_pTargetView, this));
    m_states.push_back(std::make_unique<ScaleFactorState>(m_document.get(), m_pTargetView));
    m_states.push_back(std::make_unique<POAState>(m_document.get(), m_pTargetView));
    m_states.push_back(std::make_unique<MarkImpactsState>(m_document.get(), m_pTargetView, m_pUndoStack));
    m_states.push_back(std::make_unique<VisualizationState>(m_document.get(), m_pTargetView));
    
    // Update workflow toolbar
    m_pWorkflowToolbar->setDocument(m_document.get());
    
    // Update statistics panel
    m_pStatisticsPanel->setDocument(m_document.get());
    
    // Determine starting state based on loaded data
    int startState = 0;
    if (m_document->canEnableVisualizationState())
        startState = 3; // MarkImpacts - let user continue adding shots
    else if (m_document->canEnablePointOfAimState())
        startState = 2; // POA
    else if (m_document->canEnableScaleFactorState())
        startState = 1; // ScaleFactor
    
    setCurrentState(startState);
    
    // Document was just loaded - not dirty
    m_document->setDirty(false);
    
    updateWindowTitle();
    updateMenuState();
    statusBar()->showMessage(tr("Loaded: %1").arg(filePath));
}
