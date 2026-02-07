#include "GraphicsSettingsDialog.h"
#include <QX/Services.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QDialogButtonBox>
#include <QColorDialog>

GraphicsSettingsDialog::GraphicsSettingsDialog(QWidget* pParent)
    : QDialog(pParent)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
    setupUi();
    updateWidgetsFromService();
}

void GraphicsSettingsDialog::setupUi()
{
    setWindowTitle("Graphics Settings");
    setMinimumWidth(500);

    auto* pMainLayout = new QVBoxLayout(this);

    // Colors group
    auto* pColorsGroup = new QGroupBox("Colors");
    auto* pColorsLayout = new QFormLayout(pColorsGroup);

    // Define color roles in display order
    const QList<GraphicsSettingsService::ColorRole> colorRoles = {
        GraphicsSettingsService::ColorRole::CalibrationLine,
        GraphicsSettingsService::ColorRole::PointOfAim,
        GraphicsSettingsService::ColorRole::Impact,
        GraphicsSettingsService::ColorRole::FullGroupCircle,
        GraphicsSettingsService::ColorRole::Percent90Circle,
        GraphicsSettingsService::ColorRole::Percent80Circle,
        GraphicsSettingsService::ColorRole::Background
    };

    for (const auto& role : colorRoles)
    {
        m_colorLabels[role] = createColorLabel(role);
        m_colorButtons[role] = createColorButton(role);
        pColorsLayout->addRow(m_colorLabels[role], m_colorButtons[role]);
    }

    pMainLayout->addWidget(pColorsGroup);

    // Opacities group
    auto* pOpacitiesGroup = new QGroupBox("Opacities");
    auto* pOpacitiesLayout = new QFormLayout(pOpacitiesGroup);

    // Define opacity roles in display order
    const QList<GraphicsSettingsService::OpacityRole> opacityRoles = {
        GraphicsSettingsService::OpacityRole::FullGroupCircle,
        GraphicsSettingsService::OpacityRole::Percent90Circle,
        GraphicsSettingsService::OpacityRole::Percent80Circle
    };

    for (const auto& role : opacityRoles)
    {
        m_opacityLabels[role] = createOpacityLabel(role);
        m_opacitySliders[role] = createOpacitySlider(role);
        
        auto* pSliderLayout = new QHBoxLayout();
        pSliderLayout->addWidget(m_opacitySliders[role], 1);
        pSliderLayout->addWidget(m_opacityValueLabels[role]);
        
        pOpacitiesLayout->addRow(m_opacityLabels[role], pSliderLayout);
    }

    pMainLayout->addWidget(pOpacitiesGroup);

    // Buttons
    auto* pButtonBox = new QDialogButtonBox();
    auto* pRestoreButton = pButtonBox->addButton("Restore Defaults", QDialogButtonBox::ResetRole);
    auto* pCloseButton = pButtonBox->addButton(QDialogButtonBox::Close);

    connect(pRestoreButton, &QPushButton::clicked, this, &GraphicsSettingsDialog::onRestoreDefaults);
    connect(pCloseButton, &QPushButton::clicked, this, &QDialog::accept);

    pMainLayout->addWidget(pButtonBox);
    
    // Connect to service to update widgets when settings change externally
    connect(&m_service, &GraphicsSettingsService::settingsChanged,
            this, &GraphicsSettingsDialog::onServiceSettingsChanged);
}

void GraphicsSettingsDialog::updateWidgetsFromService()
{
    // Update color buttons
    for (auto it = m_colorButtons.constBegin(); it != m_colorButtons.constEnd(); ++it)
    {
        const QColor color = m_service.color(it.key());
        
        // Use stylesheet with border to display the color accurately
        const QString styleSheet = QString(
            "QPushButton {"
            "  background-color: %1;"
            "  border: 1px solid #888888;"
            "}"
            "QPushButton:hover {"
            "  border: 2px solid #000000;"
            "}"
        ).arg(color.name());
        
        it.value()->setStyleSheet(styleSheet);
    }

    // Update opacity sliders and value labels
    for (auto it = m_opacitySliders.constBegin(); it != m_opacitySliders.constEnd(); ++it)
    {
        const int opacity = m_service.opacity(it.key());
        it.value()->setValue(opacity);
        m_opacityValueLabels[it.key()]->setText(QString("%1%").arg(opacity));
    }
}

QString GraphicsSettingsDialog::colorRoleToShortName(GraphicsSettingsService::ColorRole role)
{
    switch (role)
    {
        case GraphicsSettingsService::ColorRole::CalibrationLine:
            return "Calibration Line";
        case GraphicsSettingsService::ColorRole::PointOfAim:
            return "Point Of Aim";
        case GraphicsSettingsService::ColorRole::Impact:
            return "Impact";
        case GraphicsSettingsService::ColorRole::FullGroupCircle:
            return "Full Group Circle";
        case GraphicsSettingsService::ColorRole::Percent90Circle:
            return "Percent90 Circle";
        case GraphicsSettingsService::ColorRole::Percent80Circle:
            return "Percent80 Circle";
        case GraphicsSettingsService::ColorRole::Background:
            return "Background";
    }
    return "";
}

QString GraphicsSettingsDialog::colorRoleToDescription(GraphicsSettingsService::ColorRole role)
{
    switch (role)
    {
        case GraphicsSettingsService::ColorRole::CalibrationLine:
            return "Calibration measurement tool";
        case GraphicsSettingsService::ColorRole::PointOfAim:
            return "Point of aim glyph";
        case GraphicsSettingsService::ColorRole::Impact:
            return "Impact glyph";
        case GraphicsSettingsService::ColorRole::FullGroupCircle:
            return "Full group circle";
        case GraphicsSettingsService::ColorRole::Percent90Circle:
            return "90% group circle";
        case GraphicsSettingsService::ColorRole::Percent80Circle:
            return "80% group circle";
        case GraphicsSettingsService::ColorRole::Background:
            return "Background color";
    }
    return "";
}

QString GraphicsSettingsDialog::opacityRoleToShortName(GraphicsSettingsService::OpacityRole role)
{
    switch (role)
    {
        case GraphicsSettingsService::OpacityRole::FullGroupCircle:
            return "Full Group Circle";
        case GraphicsSettingsService::OpacityRole::Percent90Circle:
            return "Percent90 Circle";
        case GraphicsSettingsService::OpacityRole::Percent80Circle:
            return "Percent80 Circle";
    }
    return "";
}

QString GraphicsSettingsDialog::opacityRoleToDescription(GraphicsSettingsService::OpacityRole role)
{
    switch (role)
    {
        case GraphicsSettingsService::OpacityRole::FullGroupCircle:
            return "Full group circle fill opacity";
        case GraphicsSettingsService::OpacityRole::Percent90Circle:
            return "90% percentile circle fill opacity";
        case GraphicsSettingsService::OpacityRole::Percent80Circle:
            return "80% percentile circle fill opacity";
    }
    return "";
}

QLabel* GraphicsSettingsDialog::createColorLabel(GraphicsSettingsService::ColorRole role)
{
    auto* pLabel = new QLabel(colorRoleToShortName(role) + ":");
    pLabel->setToolTip(colorRoleToDescription(role));
    return pLabel;
}

QPushButton* GraphicsSettingsDialog::createColorButton(GraphicsSettingsService::ColorRole role)
{
    auto* pButton = new QPushButton();
    
    // No text - button is a color preview
    pButton->setFixedSize(60, 30);
    
    // Set tooltip to match the label
    pButton->setToolTip(colorRoleToDescription(role));
    
    connect(pButton, &QPushButton::clicked, this, [this, role]() {
        onColorButtonClicked(role);
    });

    return pButton;
}

QLabel* GraphicsSettingsDialog::createOpacityLabel(GraphicsSettingsService::OpacityRole role)
{
    auto* pLabel = new QLabel(opacityRoleToShortName(role) + ":");
    pLabel->setToolTip(opacityRoleToDescription(role));
    return pLabel;
}

QSlider* GraphicsSettingsDialog::createOpacitySlider(GraphicsSettingsService::OpacityRole role)
{
    auto* pSlider = new QSlider(Qt::Horizontal);
    pSlider->setRange(0, 100);
    pSlider->setToolTip(opacityRoleToDescription(role));

    auto* pValueLabel = new QLabel("0%");
    pValueLabel->setMinimumWidth(40);
    pValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_opacityValueLabels[role] = pValueLabel;

    connect(pSlider, &QSlider::valueChanged, this, [this, role, pValueLabel](int value) {
        onOpacitySliderChanged(role, value);
        pValueLabel->setText(QString("%1%").arg(value));
    });

    return pSlider;
}

void GraphicsSettingsDialog::onColorButtonClicked(GraphicsSettingsService::ColorRole role)
{
    const QColor currentColor = m_service.color(role);
    const QColor newColor = QColorDialog::getColor(currentColor, this, "Choose Color");

    if (newColor.isValid())
    {
        m_service.setColor(role, newColor);
        // Widget will be updated via onServiceSettingsChanged signal
    }
}

void GraphicsSettingsDialog::onOpacitySliderChanged(GraphicsSettingsService::OpacityRole role, int value)
{
    m_service.setOpacity(role, value);
}

void GraphicsSettingsDialog::onRestoreDefaults()
{
    m_service.restoreDefaults();
    // Widgets will be updated via onServiceSettingsChanged signal
}

void GraphicsSettingsDialog::onServiceSettingsChanged()
{
    updateWidgetsFromService();
}
