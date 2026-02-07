#pragma once

#include <QDialog>
#include <QHash>
#include <Services/GraphicsSettingsService.h>

class QPushButton;
class QSlider;
class QLabel;

/// @brief Non-modal dialog for configuring graphics colors and opacities
/// 
/// GraphicsSettingsDialog provides a live-preview interface for customizing
/// graphics colors and opacities. Changes are applied immediately through the
/// GraphicsSettingsService, allowing users to see updates in real-time while
/// the dialog remains open.
class GraphicsSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphicsSettingsDialog(QWidget* pParent = nullptr);

private:
    void setupUi();
    void updateWidgetsFromService();

    QString colorRoleToShortName(GraphicsSettingsService::ColorRole role);
    QString colorRoleToDescription(GraphicsSettingsService::ColorRole role);
    QString opacityRoleToShortName(GraphicsSettingsService::OpacityRole role);
    QString opacityRoleToDescription(GraphicsSettingsService::OpacityRole role);

    QPushButton* createColorButton(GraphicsSettingsService::ColorRole role);
    QLabel* createColorLabel(GraphicsSettingsService::ColorRole role);
    QSlider* createOpacitySlider(GraphicsSettingsService::OpacityRole role);
    QLabel* createOpacityLabel(GraphicsSettingsService::OpacityRole role);

    void onColorButtonClicked(GraphicsSettingsService::ColorRole role);
    void onOpacitySliderChanged(GraphicsSettingsService::OpacityRole role, int value);
    void onRestoreDefaults();
    void onServiceSettingsChanged();

    GraphicsSettingsService& m_service;

    // Color buttons and labels indexed by role
    QHash<GraphicsSettingsService::ColorRole, QPushButton*> m_colorButtons;
    QHash<GraphicsSettingsService::ColorRole, QLabel*> m_colorLabels;

    // Opacity sliders and labels indexed by role
    QHash<GraphicsSettingsService::OpacityRole, QSlider*> m_opacitySliders;
    QHash<GraphicsSettingsService::OpacityRole, QLabel*> m_opacityLabels;
    QHash<GraphicsSettingsService::OpacityRole, QLabel*> m_opacityValueLabels;
};
