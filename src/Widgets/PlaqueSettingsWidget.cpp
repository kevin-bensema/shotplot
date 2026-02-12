#include "PlaqueSettingsWidget.h"
#include <Core/ShotGroupDocument.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QGroupBox>
#include <QFont>

PlaqueSettingsWidget::PlaqueSettingsWidget(QWidget* pParent)
    : QDockWidget(tr("Plaque Settings"), pParent)
{
    setupUi();
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

void PlaqueSettingsWidget::setDocument(ShotGroupDocument* pDocument)
{
    if (m_pDocument)
    {
        m_pDocument->disconnect(this);
    }

    m_pDocument = pDocument;

    if (m_pDocument)
    {
        connect(m_pDocument, &ShotGroupDocument::plaqueSettingsChanged,
                this, &PlaqueSettingsWidget::syncFromDocument);
    }

    syncFromDocument();
}

void PlaqueSettingsWidget::setupUi()
{
    QWidget* pContent = new QWidget(this);
    QVBoxLayout* pLayout = new QVBoxLayout(pContent);

    // Title edit
    QLabel* pTitleLabel = new QLabel(tr("Title:"), pContent);
    m_pTitleEdit = new QLineEdit(pContent);
    m_pTitleEdit->setPlaceholderText(tr("Enter plaque title..."));
    pLayout->addWidget(pTitleLabel);
    pLayout->addWidget(m_pTitleEdit);

    connect(m_pTitleEdit, &QLineEdit::textChanged,
            this, &PlaqueSettingsWidget::applyToDocument);

    pLayout->addSpacing(8);

    // Font size slider
    QLabel* pFontSizeLabel = new QLabel(tr("Font Size:"), pContent);
    pLayout->addWidget(pFontSizeLabel);

    QHBoxLayout* pFontSizeLayout = new QHBoxLayout();
    m_pFontSizeSlider = new QSlider(Qt::Horizontal, pContent);
    m_pFontSizeSlider->setRange(6, 30);
    m_pFontSizeSlider->setSingleStep(1);
    m_pFontSizeSlider->setPageStep(2);
    m_pFontSizeValueLabel = new QLabel(pContent);
    m_pFontSizeValueLabel->setMinimumWidth(30);

    pFontSizeLayout->addWidget(m_pFontSizeSlider);
    pFontSizeLayout->addWidget(m_pFontSizeValueLabel);
    pLayout->addLayout(pFontSizeLayout);

    connect(m_pFontSizeSlider, &QSlider::valueChanged, this, [this](int value)
    {
        m_pFontSizeValueLabel->setText(QString::number(value) + QStringLiteral(" pt"));
        applyToDocument();
    });

    pLayout->addSpacing(8);

    // Statistics checkboxes
    QGroupBox* pStatsGroup = new QGroupBox(tr("Displayed Statistics"), pContent);
    QVBoxLayout* pStatsLayout = new QVBoxLayout(pStatsGroup);

    m_pShotCountCheck = new QCheckBox(tr("Shot Count"), pStatsGroup);
    m_pFullGroupCheck = new QCheckBox(tr("Full Group"), pStatsGroup);
    m_pGroup80Check = new QCheckBox(tr("80% Group"), pStatsGroup);
    m_pGroup90Check = new QCheckBox(tr("90% Group"), pStatsGroup);
    m_pMeanRadiusCheck = new QCheckBox(tr("Mean Radius"), pStatsGroup);
    m_pStdDevCheck = new QCheckBox(tr("Std Dev"), pStatsGroup);
    m_pOffsetXCheck = new QCheckBox(tr("Offset X"), pStatsGroup);
    m_pOffsetYCheck = new QCheckBox(tr("Offset Y"), pStatsGroup);

    pStatsLayout->addWidget(m_pShotCountCheck);
    pStatsLayout->addWidget(m_pFullGroupCheck);
    pStatsLayout->addWidget(m_pGroup80Check);
    pStatsLayout->addWidget(m_pGroup90Check);
    pStatsLayout->addWidget(m_pMeanRadiusCheck);
    pStatsLayout->addWidget(m_pStdDevCheck);
    pStatsLayout->addWidget(m_pOffsetXCheck);
    pStatsLayout->addWidget(m_pOffsetYCheck);

    pLayout->addWidget(pStatsGroup);
    pLayout->addStretch();

    setWidget(pContent);

    // Connect all checkboxes to apply
    auto connectCheck = [this](QCheckBox* pCheck)
    {
        connect(pCheck, &QCheckBox::toggled,
                this, &PlaqueSettingsWidget::applyToDocument);
    };

    connectCheck(m_pShotCountCheck);
    connectCheck(m_pFullGroupCheck);
    connectCheck(m_pGroup80Check);
    connectCheck(m_pGroup90Check);
    connectCheck(m_pMeanRadiusCheck);
    connectCheck(m_pStdDevCheck);
    connectCheck(m_pOffsetXCheck);
    connectCheck(m_pOffsetYCheck);
}

void PlaqueSettingsWidget::applyToDocument()
{
    if (!m_pDocument || m_updatingFromDocument)
        return;

    auto config = m_pDocument->plaqueConfig();
    config.title = m_pTitleEdit->text();

    using PS = ShotGroupDocument::PlaqueStat;
    QSet<PS> stats;

    if (m_pShotCountCheck->isChecked()) stats.insert(PS::ShotCount);
    if (m_pFullGroupCheck->isChecked()) stats.insert(PS::FullGroup);
    if (m_pGroup80Check->isChecked())   stats.insert(PS::Group80);
    if (m_pGroup90Check->isChecked())   stats.insert(PS::Group90);
    if (m_pMeanRadiusCheck->isChecked()) stats.insert(PS::MeanRadius);
    if (m_pStdDevCheck->isChecked())    stats.insert(PS::StdDev);
    if (m_pOffsetXCheck->isChecked())   stats.insert(PS::OffsetX);
    if (m_pOffsetYCheck->isChecked())   stats.insert(PS::OffsetY);

    config.enabledStats = stats;
    config.baseFontSize = m_pFontSizeSlider->value();
    m_pDocument->setPlaqueConfig(config);
}

void PlaqueSettingsWidget::syncFromDocument()
{
    m_updatingFromDocument = true;

    if (!m_pDocument)
    {
        m_pTitleEdit->clear();
        m_pShotCountCheck->setChecked(false);
        m_pFullGroupCheck->setChecked(false);
        m_pGroup80Check->setChecked(false);
        m_pGroup90Check->setChecked(false);
        m_pMeanRadiusCheck->setChecked(false);
        m_pStdDevCheck->setChecked(false);
        m_pOffsetXCheck->setChecked(false);
        m_pOffsetYCheck->setChecked(false);
        m_pFontSizeSlider->setValue(QFont().pointSize() + 6);
        m_pFontSizeValueLabel->setText(QString::number(m_pFontSizeSlider->value()) + QStringLiteral(" pt"));
        m_updatingFromDocument = false;
        return;
    }

    const auto config = m_pDocument->plaqueConfig();
    using PS = ShotGroupDocument::PlaqueStat;

    m_pTitleEdit->setText(config.title);
    m_pShotCountCheck->setChecked(config.enabledStats.contains(PS::ShotCount));
    m_pFullGroupCheck->setChecked(config.enabledStats.contains(PS::FullGroup));
    m_pGroup80Check->setChecked(config.enabledStats.contains(PS::Group80));
    m_pGroup90Check->setChecked(config.enabledStats.contains(PS::Group90));
    m_pMeanRadiusCheck->setChecked(config.enabledStats.contains(PS::MeanRadius));
    m_pStdDevCheck->setChecked(config.enabledStats.contains(PS::StdDev));
    m_pOffsetXCheck->setChecked(config.enabledStats.contains(PS::OffsetX));
    m_pOffsetYCheck->setChecked(config.enabledStats.contains(PS::OffsetY));

    m_pFontSizeSlider->setValue(config.baseFontSize);
    m_pFontSizeValueLabel->setText(QString::number(config.baseFontSize) + QStringLiteral(" pt"));

    m_updatingFromDocument = false;
}
