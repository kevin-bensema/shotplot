#include "StatisticsPanel.h"
#include <Core/ShotGroupDocument.h>
#include <Core/UnitConverter.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSettings>

namespace
{
    constexpr const char* kDisplayUnitsKey = "display/units";
    constexpr const char* kDefaultUnitString = "inches";
    constexpr const char* kShotCountStyleSheet = "font-weight: bold; font-size: 14px;";
    constexpr int kLayoutSpacing = 10;
    constexpr int kMinImpactCountForStatistics = 2;
}

StatisticsPanel::StatisticsPanel(QWidget* pParent)
    : QWidget(pParent)
{
    setupUi();
}

StatisticsPanel::~StatisticsPanel() = default;

void StatisticsPanel::setupUi()
{
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    
    // Shot count
    m_pShotCountLabel = new QLabel(tr("Shots: 0"));
    m_pShotCountLabel->setStyleSheet(kShotCountStyleSheet);
    pLayout->addWidget(m_pShotCountLabel);
    
    pLayout->addSpacing(kLayoutSpacing);
    
    // Statistics group
    QGroupBox* pStatsGroup = new QGroupBox(tr("Group Statistics"));
    QVBoxLayout* pStatsLayout = new QVBoxLayout(pStatsGroup);
    
    m_pFullGroupLabel = new QLabel(tr("Full Group: --"));
    m_pGroup80Label = new QLabel(tr("80% Group: --"));
    m_pGroup90Label = new QLabel(tr("90% Group: --"));
    m_pMeanRadiusLabel = new QLabel(tr("Mean Radius: --"));
    m_pStdDevLabel = new QLabel(tr("Std Dev: --"));
    m_pOffsetXLabel = new QLabel(tr("Offset X: --"));
    m_pOffsetYLabel = new QLabel(tr("Offset Y: --"));
    
    pStatsLayout->addWidget(m_pFullGroupLabel);
    pStatsLayout->addWidget(m_pGroup80Label);
    pStatsLayout->addWidget(m_pGroup90Label);
    pStatsLayout->addWidget(m_pMeanRadiusLabel);
    pStatsLayout->addWidget(m_pStdDevLabel);
    pStatsLayout->addWidget(m_pOffsetXLabel);
    pStatsLayout->addWidget(m_pOffsetYLabel);
    
    pLayout->addWidget(pStatsGroup);
    
    pLayout->addSpacing(kLayoutSpacing);
    
    // Display options
    QGroupBox* pDisplayGroup = new QGroupBox(tr("Display Options"));
    QVBoxLayout* pDisplayLayout = new QVBoxLayout(pDisplayGroup);
    
    m_pShowFullGroupCheck = new QCheckBox(tr("Full Group Circle"));
    m_pShowFullGroupCheck->setChecked(true);
    connect(m_pShowFullGroupCheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                if (m_pDocument)
                {
                    m_pDocument->setShowGroupCircle(GroupCircle::Type::Full, checked);
                }
            });

    m_pShow80GroupCheck = new QCheckBox(tr("80% Group Circle"));
    m_pShow80GroupCheck->setChecked(false);
    connect(m_pShow80GroupCheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                if (m_pDocument)
                {
                    m_pDocument->setShowGroupCircle(GroupCircle::Type::Percent80, checked);
                }
            });

    m_pShow90GroupCheck = new QCheckBox(tr("90% Group Circle"));
    m_pShow90GroupCheck->setChecked(false);
    connect(m_pShow90GroupCheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                if (m_pDocument)
                {
                    m_pDocument->setShowGroupCircle(GroupCircle::Type::Percent90, checked);
                }
            });
    
    m_pShowPOACheck = new QCheckBox(tr("Point of Aim"));
    m_pShowPOACheck->setChecked(true);
    connect(m_pShowPOACheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                if (m_pDocument)
                {
                    m_pDocument->setShowPointOfAim(checked);
                }
            });
    
    m_pShowCentroidCheck = new QCheckBox(tr("Centroid"));
    m_pShowCentroidCheck->setChecked(false);
    connect(m_pShowCentroidCheck, &QCheckBox::toggled,
            this, [this](bool checked) {
                if (m_pDocument)
                {
                    m_pDocument->setShowCentroid(checked);
                }
            });
    
    pDisplayLayout->addWidget(m_pShowFullGroupCheck);
    pDisplayLayout->addWidget(m_pShow80GroupCheck);
    pDisplayLayout->addWidget(m_pShow90GroupCheck);
    pDisplayLayout->addWidget(m_pShowPOACheck);
    pDisplayLayout->addWidget(m_pShowCentroidCheck);
    
    pLayout->addWidget(pDisplayGroup);
    
    pLayout->addStretch();
}

void StatisticsPanel::setDocument(ShotGroupDocument* pDocument)
{
    if (m_pDocument)
    {
        m_pDocument->disconnect(this);
    }

    m_pDocument = pDocument;
    
    if (m_pDocument)
    {
        connect(m_pDocument, &ShotGroupDocument::statisticsChanged,
                this, &StatisticsPanel::updateStatistics);
        connect(m_pDocument, &ShotGroupDocument::dataChanged,
                this, &StatisticsPanel::updateStatistics);
        connect(m_pDocument, &ShotGroupDocument::impactsChanged,
                this, &StatisticsPanel::updateStatistics);
        
        // Sync checkboxes with document
        m_pShowFullGroupCheck->setChecked(m_pDocument->showGroupCircle(GroupCircle::Type::Full));
        m_pShow80GroupCheck->setChecked(m_pDocument->showGroupCircle(GroupCircle::Type::Percent80));
        m_pShow90GroupCheck->setChecked(m_pDocument->showGroupCircle(GroupCircle::Type::Percent90));
        m_pShowPOACheck->setChecked(m_pDocument->showPointOfAim());
        m_pShowCentroidCheck->setChecked(m_pDocument->showCentroid());
    }
    
    updateStatistics();
}

void StatisticsPanel::updateDisplay()
{
    updateStatistics();
}

void StatisticsPanel::updateStatistics()
{
    if (!m_pDocument)
    {
        m_pShotCountLabel->setText(tr("Shots: 0"));
        m_pFullGroupLabel->setText(tr("Full Group: --"));
        m_pGroup80Label->setText(tr("80% Group: --"));
        m_pGroup90Label->setText(tr("90% Group: --"));
        m_pMeanRadiusLabel->setText(tr("Mean Radius: --"));
        m_pStdDevLabel->setText(tr("Std Dev: --"));
        m_pOffsetXLabel->setText(tr("Offset X: --"));
        m_pOffsetYLabel->setText(tr("Offset Y: --"));
        return;
    }
    m_pShotCountLabel->setText(tr("Shots: %1").arg(m_pDocument->impactCount()));
    
    if (m_pDocument->impactCount() < kMinImpactCountForStatistics || !m_pDocument->hasScaleFactorSet())
    {
        m_pFullGroupLabel->setText(tr("Full Group: --"));
        m_pGroup80Label->setText(tr("80% Group: --"));
        m_pGroup90Label->setText(tr("90% Group: --"));
        m_pMeanRadiusLabel->setText(tr("Mean Radius: --"));
        m_pStdDevLabel->setText(tr("Std Dev: --"));
        m_pOffsetXLabel->setText(tr("Offset X: --"));
        m_pOffsetYLabel->setText(tr("Offset Y: --"));
        return;
    }
    
    // Get display unit preference
    QSettings settings;
    QString unitStr = settings.value(kDisplayUnitsKey, kDefaultUnitString).toString();
    UnitConverter::Unit unit = UnitConverter::unitFromString(unitStr);
    
    double ppi = m_pDocument->pixelsPerInch();
    double distanceYards = m_pDocument->targetDistance();
    if (m_pDocument->distanceUnit() == ShotGroupDocument::DistanceUnit::Meters)
    {
        distanceYards = UnitConverter::metersToYards(distanceYards);
    }
    
    const Statistics& stats = m_pDocument->statistics();
    
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
        m_pFullGroupLabel->setText(tr("Full Group: %1").arg(formatValue(stats.fullGroupCircle.diameterPixels())));
    }
    
    if (stats.group80Circle.isValid())
    {
        m_pGroup80Label->setText(tr("80% Group: %1").arg(formatValue(stats.group80Circle.diameterPixels())));
    }
    
    if (stats.group90Circle.isValid())
    {
        m_pGroup90Label->setText(tr("90% Group: %1").arg(formatValue(stats.group90Circle.diameterPixels())));
    }
    
    m_pMeanRadiusLabel->setText(tr("Mean Radius: %1").arg(formatValue(stats.meanRadiusPixels)));
    m_pStdDevLabel->setText(tr("Std Dev: %1").arg(formatValue(stats.standardDeviationPixels)));
    
    // Display offset from point of aim (if calculated)
    if (stats.offsetFromPOA.has_value())
    {
        const QPointF& offset = stats.offsetFromPOA.value();
        
        // Convert offset to display units with coordinate transformation
        // Image coordinates: right=+X, down=+Y
        // Shooter perspective: right=+X, up=+Y (negate Y)
        double offsetXPixels = offset.x();
        double offsetYPixels = -offset.y();  // Negate for shooter perspective
        
        double offsetXInches = UnitConverter::pixelsToInches(offsetXPixels, ppi);
        double offsetYInches = UnitConverter::pixelsToInches(offsetYPixels, ppi);
        
        double offsetXValue = offsetXInches;
        double offsetYValue = offsetYInches;
        
        if (unit == UnitConverter::Unit::MOA)
        {
            offsetXValue = UnitConverter::inchesToMOA(offsetXInches, distanceYards);
            offsetYValue = UnitConverter::inchesToMOA(offsetYInches, distanceYards);
        }
        else if (unit == UnitConverter::Unit::MRAD)
        {
            offsetXValue = UnitConverter::inchesToMRAD(offsetXInches, distanceYards);
            offsetYValue = UnitConverter::inchesToMRAD(offsetYInches, distanceYards);
        }
        
        m_pOffsetXLabel->setText(tr("Offset X: %1").arg(UnitConverter::formatWithUnit(offsetXValue, unit)));
        m_pOffsetYLabel->setText(tr("Offset Y: %1").arg(UnitConverter::formatWithUnit(offsetYValue, unit)));
    }
    else
    {
        m_pOffsetXLabel->setText(tr("Offset X: --"));
        m_pOffsetYLabel->setText(tr("Offset Y: --"));
    }
}

