#include "StatisticsPanel.h"
#include "Core/ShotGroupDocument.h"
#include "Core/UnitConverter.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSettings>

StatisticsPanel::StatisticsPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

StatisticsPanel::~StatisticsPanel() = default;

void StatisticsPanel::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Shot count
    m_shotCountLabel = new QLabel(tr("Shots: 0"));
    m_shotCountLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(m_shotCountLabel);
    
    layout->addSpacing(10);
    
    // Statistics group
    QGroupBox *statsGroup = new QGroupBox(tr("Group Statistics"));
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_fullGroupLabel = new QLabel(tr("Full Group: --"));
    m_group80Label = new QLabel(tr("80% Group: --"));
    m_group90Label = new QLabel(tr("90% Group: --"));
    m_meanRadiusLabel = new QLabel(tr("Mean Radius: --"));
    m_stdDevLabel = new QLabel(tr("Std Dev: --"));
    
    statsLayout->addWidget(m_fullGroupLabel);
    statsLayout->addWidget(m_group80Label);
    statsLayout->addWidget(m_group90Label);
    statsLayout->addWidget(m_meanRadiusLabel);
    statsLayout->addWidget(m_stdDevLabel);
    
    layout->addWidget(statsGroup);
    
    layout->addSpacing(10);
    
    // Display options
    QGroupBox *displayGroup = new QGroupBox(tr("Display Options"));
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    
    m_showFullGroupCheck = new QCheckBox(tr("Full Group Circle"));
    m_showFullGroupCheck->setChecked(true);
    connect(m_showFullGroupCheck, &QCheckBox::toggled,
            this, &StatisticsPanel::onShowFullGroupChanged);
    
    m_showPOACheck = new QCheckBox(tr("Point of Aim"));
    m_showPOACheck->setChecked(true);
    connect(m_showPOACheck, &QCheckBox::toggled,
            this, &StatisticsPanel::onShowPOAChanged);
    
    displayLayout->addWidget(m_showFullGroupCheck);
    displayLayout->addWidget(m_showPOACheck);
    
    layout->addWidget(displayGroup);
    
    layout->addStretch();
}

void StatisticsPanel::setDocument(ShotGroupDocument *doc)
{
    m_document = doc;
    
    if (m_document)
    {
        connect(m_document, &ShotGroupDocument::statisticsChanged,
                this, &StatisticsPanel::updateStatistics);
        
        // Sync checkboxes with document
        m_showFullGroupCheck->setChecked(m_document->showFullGroupCircle());
        m_showPOACheck->setChecked(m_document->showPointOfAim());
    }
    
    updateStatistics();
}

void StatisticsPanel::updateDisplay()
{
    updateStatistics();
}

void StatisticsPanel::updateStatistics()
{
    if (!m_document)
    {
        m_shotCountLabel->setText(tr("Shots: 0"));
        m_fullGroupLabel->setText(tr("Full Group: --"));
        m_group80Label->setText(tr("80% Group: --"));
        m_group90Label->setText(tr("90% Group: --"));
        m_meanRadiusLabel->setText(tr("Mean Radius: --"));
        m_stdDevLabel->setText(tr("Std Dev: --"));
        return;
    }
    
    m_shotCountLabel->setText(tr("Shots: %1").arg(m_document->impactCount()));
    
    if (m_document->impactCount() < 2 || !m_document->hasScaleFactorSet())
    {
        m_fullGroupLabel->setText(tr("Full Group: --"));
        m_group80Label->setText(tr("80% Group: --"));
        m_group90Label->setText(tr("90% Group: --"));
        m_meanRadiusLabel->setText(tr("Mean Radius: --"));
        m_stdDevLabel->setText(tr("Std Dev: --"));
        return;
    }
    
    // Get display unit preference
    QSettings settings;
    QString unitStr = settings.value("display/units", "inches").toString();
    UnitConverter::Unit unit = UnitConverter::unitFromString(unitStr);
    
    double ppi = m_document->pixelsPerInch();
    double distanceYards = m_document->targetDistance();
    if (m_document->distanceUnit() == ShotGroupDocument::DistanceUnit::Meters)
    {
        distanceYards = UnitConverter::metersToYards(distanceYards);
    }
    
    const Statistics &stats = m_document->statistics();
    
    auto formatValue = [&](double pixels) -> QString
    {
        double inches = UnitConverter::pixelsToInches(pixels, ppi);
        double value = inches;
        
        if (unit == UnitConverter::Unit::MOA)
        {
            value = UnitConverter::inchesToMOA(inches, distanceYards);
        }
        else if (unit == UnitConverter::Unit::MRAD)
        {
            value = UnitConverter::inchesToMRAD(inches, distanceYards);
        }
        
        return UnitConverter::formatWithUnit(value, unit);
    };
    
    // Full group (diameter, not radius)
    if (stats.fullGroupCircle.isValid())
    {
        m_fullGroupLabel->setText(tr("Full Group: %1").arg(formatValue(stats.fullGroupCircle.diameterPixels())));
    }
    
    if (stats.group80Circle.isValid())
    {
        m_group80Label->setText(tr("80%% Group: %1").arg(formatValue(stats.group80Circle.diameterPixels())));
    }
    
    if (stats.group90Circle.isValid())
    {
        m_group90Label->setText(tr("90%% Group: %1").arg(formatValue(stats.group90Circle.diameterPixels())));
    }
    
    m_meanRadiusLabel->setText(tr("Mean Radius: %1").arg(formatValue(stats.meanRadiusPixels)));
    m_stdDevLabel->setText(tr("Std Dev: %1").arg(formatValue(stats.standardDeviationPixels)));
}

void StatisticsPanel::onShowFullGroupChanged(bool checked)
{
    if (m_document)
    {
        m_document->setShowFullGroupCircle(checked);
    }
}

void StatisticsPanel::onShowPOAChanged(bool checked)
{
    if (m_document)
    {
        m_document->setShowPointOfAim(checked);
    }
}
