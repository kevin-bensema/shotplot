#pragma once

#include <QDockWidget>

class QLineEdit;
class QCheckBox;
class QSlider;
class QLabel;
class ShotGroupDocument;

/// @brief Dock widget for configuring the statistics plaque overlay
///
/// Provides a title text editor and checkboxes for toggling individual
/// statistics in the plaque. Changes are immediately written to the
/// document's PlaqueConfig, triggering a redraw of the plaque item.
class PlaqueSettingsWidget : public QDockWidget
{
    Q_OBJECT

public:
    /// Constructs the plaque settings widget
    /// \param pParent The parent widget
    explicit PlaqueSettingsWidget(QWidget* pParent = nullptr);

    /// Sets the document and syncs the UI controls from its PlaqueConfig
    /// \param pDocument The document to configure (can be nullptr)
    void setDocument(ShotGroupDocument* pDocument);

private:
    void setupUi();
    /// Pushes the current UI state to the document's PlaqueConfig
    void applyToDocument();
    /// Reads the document's PlaqueConfig and syncs all UI controls
    void syncFromDocument();

    ShotGroupDocument* m_pDocument = nullptr;

    QLineEdit* m_pTitleEdit = nullptr;

    QCheckBox* m_pShotCountCheck = nullptr;
    QCheckBox* m_pFullGroupCheck = nullptr;
    QCheckBox* m_pGroup80Check = nullptr;
    QCheckBox* m_pGroup90Check = nullptr;
    QCheckBox* m_pMeanRadiusCheck = nullptr;
    QCheckBox* m_pStdDevCheck = nullptr;
    QCheckBox* m_pOffsetXCheck = nullptr;
    QCheckBox* m_pOffsetYCheck = nullptr;

    QSlider* m_pFontSizeSlider = nullptr;
    QLabel* m_pFontSizeValueLabel = nullptr;

    /// Guard flag to prevent re-entrant updates when syncing from document
    bool m_updatingFromDocument = false;
};
