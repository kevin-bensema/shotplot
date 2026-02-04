#include "ShotGroupDocument.h"
#include "StatisticsCalculator.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace {
    constexpr double kDefaultTargetDistance = 100.0;
    constexpr ShotGroupDocument::DistanceUnit kDefaultDistanceUnit = ShotGroupDocument::DistanceUnit::Yards;
    constexpr bool kDefaultShowFullGroupCircle = true;
    constexpr bool kDefaultShow80PercentCircle = false;
    constexpr bool kDefaultShow90PercentCircle = false;
    constexpr bool kDefaultShowPointOfAim = true;
    constexpr bool kDefaultPlaqueEnabled = false;
    constexpr int kDefaultPlaqueX = 50;
    constexpr int kDefaultPlaqueY = 50;
    constexpr int kDefaultPlaqueWidth = 300;
    constexpr int kDefaultPlaqueHeight = 200;
    const QString kDefaultPlaqueTitle = "Shot Group Statistics";
}

ShotGroupDocument::ShotGroupDocument(QObject* pParent)
    : QObject(pParent)
    , m_targetDistance(kDefaultTargetDistance)
    , m_distanceUnit(kDefaultDistanceUnit)
    , m_sessionDate(QDate::currentDate())
    , m_showFullGroupCircle(kDefaultShowFullGroupCircle)
    , m_show80PercentCircle(kDefaultShow80PercentCircle)
    , m_show90PercentCircle(kDefaultShow90PercentCircle)
    , m_showPointOfAim(kDefaultShowPointOfAim)
    , m_plaqueConfig{kDefaultPlaqueEnabled, kDefaultPlaqueX, kDefaultPlaqueY, kDefaultPlaqueWidth, kDefaultPlaqueHeight, kDefaultPlaqueTitle, QString()}
{
}

ShotGroupDocument::~ShotGroupDocument() = default;

// ===== Image Data =====

QImage ShotGroupDocument::targetImage() const
{
    return m_targetImage;
}

void ShotGroupDocument::setTargetImage(const QImage& image)
{
    m_targetImage = image;
    setDirty(true);
    emit dataChanged();
}

// ===== Calibration Data =====

double ShotGroupDocument::bulletDiameter() const
{
    return m_bulletDiameter;
}

void ShotGroupDocument::setBulletDiameter(double diameter)
{
    if (m_bulletDiameter != diameter)
    {
        m_bulletDiameter = diameter;
        setDirty(true);
        emit dataChanged();
    }
}

bool ShotGroupDocument::hasCaliberSet() const
{
    return m_bulletDiameter > 0.0;
}

double ShotGroupDocument::pixelsPerInch() const
{
    return m_pixelsPerInch;
}

void ShotGroupDocument::setPixelsPerInch(double ppi)
{
    if (m_pixelsPerInch != ppi)
    {
        m_pixelsPerInch = ppi;
        updateStatistics();
        setDirty(true);
        emit dataChanged();
    }
}

bool ShotGroupDocument::hasScaleFactorSet() const
{
    return m_pixelsPerInch > 0.0;
}

// ===== Point of Aim =====

QPointF ShotGroupDocument::pointOfAim() const
{
    return m_pointOfAim;
}

void ShotGroupDocument::setPointOfAim(const QPointF& poa)
{
    m_pointOfAim = poa;
    m_hasPointOfAim = true;
    setDirty(true);
    emit dataChanged();
}

void ShotGroupDocument::clearPointOfAim()
{
    m_pointOfAim = QPointF();
    m_hasPointOfAim = false;
    setDirty(true);
    emit dataChanged();
}

bool ShotGroupDocument::hasPointOfAimSet() const
{
    return m_hasPointOfAim;
}

// ===== Session Parameters =====

double ShotGroupDocument::targetDistance() const
{
    return m_targetDistance;
}

void ShotGroupDocument::setTargetDistance(double distance)
{
    if (m_targetDistance != distance) {
        m_targetDistance = distance;
        setDirty(true);
        emit dataChanged();
    }
}

ShotGroupDocument::DistanceUnit ShotGroupDocument::distanceUnit() const
{
    return m_distanceUnit;
}

void ShotGroupDocument::setDistanceUnit(DistanceUnit unit)
{
    if (m_distanceUnit != unit) {
        m_distanceUnit = unit;
        setDirty(true);
        emit dataChanged();
    }
}

QDate ShotGroupDocument::sessionDate() const
{
    return m_sessionDate;
}

void ShotGroupDocument::setSessionDate(const QDate& date)
{
    if (m_sessionDate != date) 
    {
        m_sessionDate = date;
        setDirty(true);
        emit dataChanged();
    }
}

// ===== Impacts =====

int ShotGroupDocument::impactCount() const
{
    return m_impacts.size();
}

QList<ShotImpact> ShotGroupDocument::impacts() const
{
    return m_impacts;
}

ShotImpact ShotGroupDocument::impactAt(int index) const
{
    if (index >= 0 && index < m_impacts.size()) {
        return m_impacts.at(index);
    }
    return ShotImpact();
}

void ShotGroupDocument::addImpact(const ShotImpact &impact)
{
    m_impacts.append(impact);
    updateStatistics();
    setDirty(true);
    emit impactsChanged();
}

void ShotGroupDocument::removeImpact(int index)
{
    if (index >= 0 && index < m_impacts.size())
    {
        m_impacts.removeAt(index);
        updateStatistics();
        setDirty(true);
        emit impactsChanged();
    }
}

void ShotGroupDocument::clearImpacts()
{
    if (!m_impacts.isEmpty())
    {
        m_impacts.clear();
        updateStatistics();
        setDirty(true);
        emit impactsChanged();
    }
}

void ShotGroupDocument::replaceImpacts(const QList<ShotImpact>& impacts)
{
    m_impacts = impacts;
    updateStatistics();
    setDirty(true);
    emit impactsChanged();
}

int ShotGroupDocument::nextImpactId()
{
    return m_nextImpactId++;
}

// ===== Metadata =====

QString ShotGroupDocument::firearm() const
{
    return m_firearm;
}

void ShotGroupDocument::setFirearm(const QString& firearm)
{
    if (m_firearm != firearm)
    {
        m_firearm = firearm;
        setDirty(true);
        emit dataChanged();
    }
}

QString ShotGroupDocument::ammunition() const
{
    return m_ammunition;
}

void ShotGroupDocument::setAmmunition(const QString& ammo)
{
    if (m_ammunition != ammo)
    {
        m_ammunition = ammo;
        setDirty(true);
        emit dataChanged();
    }
}

QString ShotGroupDocument::notes() const
{
    return m_notes;
}

void ShotGroupDocument::setNotes(const QString& notes)
{
    if (m_notes != notes)
    {
        m_notes = notes;
        setDirty(true);
        emit dataChanged();
    }
}

// ===== Visualization Settings =====

bool ShotGroupDocument::showFullGroupCircle() const
{
    return m_showFullGroupCircle;
}

void ShotGroupDocument::setShowFullGroupCircle(bool show)
{
    if (m_showFullGroupCircle != show)
    {
        m_showFullGroupCircle = show;
        setDirty(true);
        emit visualizationSettingsChanged();
    }
}

bool ShotGroupDocument::show80PercentCircle() const
{
    return m_show80PercentCircle;
}

void ShotGroupDocument::setShow80PercentCircle(bool show)
{
    if (m_show80PercentCircle != show)
    {
        m_show80PercentCircle = show;
        setDirty(true);
        emit visualizationSettingsChanged();
    }
}

bool ShotGroupDocument::show90PercentCircle() const
{
    return m_show90PercentCircle;
}

void ShotGroupDocument::setShow90PercentCircle(bool show)
{
    if (m_show90PercentCircle != show)
    {
        m_show90PercentCircle = show;
        setDirty(true);
        emit visualizationSettingsChanged();
    }
}

bool ShotGroupDocument::showPointOfAim() const
{
    return m_showPointOfAim;
}

void ShotGroupDocument::setShowPointOfAim(bool show)
{
    if (m_showPointOfAim != show)
    {
        m_showPointOfAim = show;
        setDirty(true);
        emit visualizationSettingsChanged();
    }
}

ShotGroupDocument::PlaqueConfig ShotGroupDocument::plaqueConfig() const
{
    return m_plaqueConfig;
}

void ShotGroupDocument::setPlaqueConfig(const PlaqueConfig& config)
{
    m_plaqueConfig = config;
    setDirty(true);
    emit visualizationSettingsChanged();
}

// ===== Statistics =====

const Statistics &ShotGroupDocument::statistics() const
{
    return m_statistics;
}

void ShotGroupDocument::updateStatistics()
{
    m_statistics = StatisticsCalculator::calculate(m_impacts);
    emit statisticsChanged();
}

// ===== File Management =====

QString ShotGroupDocument::filePath() const
{
    return m_filePath;
}

void ShotGroupDocument::setFilePath(const QString &path)
{
    m_filePath = path;
}

bool ShotGroupDocument::hasFilePath() const
{
    return !m_filePath.isEmpty();
}

bool ShotGroupDocument::isDirty() const
{
    return m_isDirty;
}

void ShotGroupDocument::setDirty(bool dirty)
{
    if (m_isDirty != dirty)
    {
        m_isDirty = dirty;
        emit dirtyChanged(dirty);
    }
}

// ===== Workflow State Queries =====

bool ShotGroupDocument::canEnableScaleFactorState() const
{
    return hasCaliberSet();
}

bool ShotGroupDocument::canEnablePointOfAimState() const
{
    return hasScaleFactorSet();
}

bool ShotGroupDocument::canEnableMarkImpactsState() const
{
    return hasScaleFactorSet();
}

bool ShotGroupDocument::canEnableVisualizationState() const
{
    return impactCount() >= 1;
}

// ===== Serialization =====

bool ShotGroupDocument::saveToFile(const QString& filePath, QString* pErrorMsg)
{
    // TODO: Implement using DocumentSerializer with libzip
    Q_UNUSED(filePath)
    if (pErrorMsg)
    {
        *pErrorMsg = "Save not yet implemented";
    }
    return false;
}

bool ShotGroupDocument::loadFromFile(const QString& filePath, QString* pErrorMsg)
{
    // TODO: Implement using DocumentSerializer with libzip
    Q_UNUSED(filePath)
    if (pErrorMsg)
    {
        *pErrorMsg = "Load not yet implemented";
    }
    return false;
}

QJsonObject ShotGroupDocument::toJson() const
{
    QJsonObject root;
    root["version"] = "1.0";
    
    // Image dimensions
    QJsonObject imageObj;
    imageObj["width"] = m_targetImage.width();
    imageObj["height"] = m_targetImage.height();
    root["image"] = imageObj;
    
    // Calibration
    QJsonObject calibrationObj;
    calibrationObj["pixelsPerInch"] = m_pixelsPerInch;
    root["calibration"] = calibrationObj;
    
    // Point of Aim
    if (m_hasPointOfAim)
    {
        QJsonObject poaObj;
        poaObj["x"] = m_pointOfAim.x();
        poaObj["y"] = m_pointOfAim.y();
        root["pointOfAim"] = poaObj;
    }
    
    // Session
    QJsonObject sessionObj;
    sessionObj["distance"] = m_targetDistance;
    sessionObj["distanceUnit"] = (m_distanceUnit == DistanceUnit::Yards) ? "yard" : "meter";
    sessionObj["bulletDiameter"] = m_bulletDiameter;
    sessionObj["numberOfShots"] = m_impacts.size();
    root["session"] = sessionObj;
    
    // Shots
    QJsonArray shotsArray;
    for (const auto &impact : m_impacts)
    {
        QJsonObject shotObj;
        shotObj["id"] = impact.id;
        shotObj["x"] = impact.x;
        shotObj["y"] = impact.y;
        shotsArray.append(shotObj);
    }
    root["shots"] = shotsArray;
    
    // Metadata
    QJsonObject metadataObj;
    metadataObj["firearm"] = m_firearm;
    metadataObj["ammunition"] = m_ammunition;
    metadataObj["date"] = m_sessionDate.toString(Qt::ISODate);
    metadataObj["notes"] = m_notes;
    root["metadata"] = metadataObj;
    
    // Visualization settings
    QJsonObject vizObj;
    vizObj["showFullGroupCircle"] = m_showFullGroupCircle;
    vizObj["show80PercentCircle"] = m_show80PercentCircle;
    vizObj["show90PercentCircle"] = m_show90PercentCircle;
    root["visualizationSettings"] = vizObj;
    
    return root;
}

bool ShotGroupDocument::fromJson(const QJsonObject& json, QString* pErrorMsg)
{
    // Version check
    QString version = json["version"].toString();
    if (version.isEmpty()) 
    {
        if (pErrorMsg) *pErrorMsg = "Missing version field";
        return false;
    }
    
    // Calibration
    QJsonObject calibrationObj = json["calibration"].toObject();
    m_pixelsPerInch = calibrationObj["pixelsPerInch"].toDouble();
    
    // Point of Aim
    if (json.contains("pointOfAim")) 
    {
        QJsonObject poaObj = json["pointOfAim"].toObject();
        m_pointOfAim = QPointF(poaObj["x"].toDouble(), poaObj["y"].toDouble());
        m_hasPointOfAim = true;
    } 
    else 
    {
        m_hasPointOfAim = false;
    }
    
    // Session
    QJsonObject sessionObj = json["session"].toObject();
    m_targetDistance = sessionObj["distance"].toDouble(kDefaultTargetDistance);
    QString unitStr = sessionObj["distanceUnit"].toString("yard");
    m_distanceUnit = (unitStr == "meter") ? DistanceUnit::Meters : DistanceUnit::Yards;
    m_bulletDiameter = sessionObj["bulletDiameter"].toDouble();
    
    // Shots
    m_impacts.clear();
    QJsonArray shotsArray = json["shots"].toArray();
    int maxId = 0;
    for (const auto& shotVal : shotsArray) 
    {
        QJsonObject shotObj = shotVal.toObject();
        ShotImpact impact;
        impact.id = shotObj["id"].toInt();
        impact.x = shotObj["x"].toDouble();
        impact.y = shotObj["y"].toDouble();
        m_impacts.append(impact);
        maxId = qMax(maxId, impact.id);
    }
    m_nextImpactId = maxId + 1;
    
    // Metadata
    QJsonObject metadataObj = json["metadata"].toObject();
    m_firearm = metadataObj["firearm"].toString();
    m_ammunition = metadataObj["ammunition"].toString();
    m_sessionDate = QDate::fromString(metadataObj["date"].toString(), Qt::ISODate);
    m_notes = metadataObj["notes"].toString();
    
    // Visualization settings
    QJsonObject vizObj = json["visualizationSettings"].toObject();
    m_showFullGroupCircle = vizObj["showFullGroupCircle"].toBool(kDefaultShowFullGroupCircle);
    m_show80PercentCircle = vizObj["show80PercentCircle"].toBool(kDefaultShow80PercentCircle);
    m_show90PercentCircle = vizObj["show90PercentCircle"].toBool(kDefaultShow90PercentCircle);
    
    updateStatistics();
    return true;
}
