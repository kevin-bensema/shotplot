#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <Core/ShotGroupDocument.h>
#include <IO/DocumentSerializer.h>

#include <QJsonObject>
#include <QTemporaryFile>
#include <QDir>
#include <QImage>

using Catch::Matchers::WithinAbs;

// ---------------------------------------------------------------------------
// Helper: build a fully-populated document for round-trip tests
// ---------------------------------------------------------------------------
static void populateDocument(ShotGroupDocument& doc)
{
    // Image
    QImage image(200, 150, QImage::Format_ARGB32);
    image.fill(Qt::red);
    doc.setTargetImage(image);

    // Calibration
    doc.setBulletDiameter(0.224);
    doc.setPixelsPerInch(72.5);

    // Point of aim
    doc.setPointOfAim(QPointF(100.5, 75.3));

    // Session parameters
    doc.setTargetDistance(100.0);
    doc.setDistanceUnit(ShotGroupDocument::DistanceUnit::Yards);
    doc.setSessionDate(QDate(2026, 2, 6));

    // Metadata
    doc.setFirearm("Ruger 10/22");
    doc.setAmmunition("CCI Standard Velocity");
    doc.setNotes("Bench rest, 5-shot group");

    // Impacts
    doc.addImpact(ShotImpact(doc.nextImpactId(), 105.3, 73.1));
    doc.addImpact(ShotImpact(doc.nextImpactId(), 98.7,  77.5));
    doc.addImpact(ShotImpact(doc.nextImpactId(), 110.0, 70.0));
    doc.addImpact(ShotImpact(doc.nextImpactId(), 101.2, 81.8));
    doc.addImpact(ShotImpact(doc.nextImpactId(), 96.5,  74.3));

    // Visualization settings
    doc.setShowGroupCircle(GroupCircle::Type::Full, true);
    doc.setShowGroupCircle(GroupCircle::Type::Percent80, true);
    doc.setShowGroupCircle(GroupCircle::Type::Percent90, false);
}

// ---------------------------------------------------------------------------
// Helper: compare two documents field-by-field
// ---------------------------------------------------------------------------
static void verifyDocumentsMatch(const ShotGroupDocument& original,
                                 const ShotGroupDocument& loaded,
                                 bool expectImage = false)
{
    // Calibration
    CHECK_THAT(loaded.pixelsPerInch(),  WithinAbs(original.pixelsPerInch(), 1e-9));
    CHECK_THAT(loaded.bulletDiameter(), WithinAbs(original.bulletDiameter(), 1e-9));

    // Point of aim
    CHECK(loaded.hasPointOfAimSet() == original.hasPointOfAimSet());
    if (original.hasPointOfAimSet())
    {
        CHECK_THAT(loaded.pointOfAim().x(), WithinAbs(original.pointOfAim().x(), 1e-9));
        CHECK_THAT(loaded.pointOfAim().y(), WithinAbs(original.pointOfAim().y(), 1e-9));
    }

    // Session parameters
    CHECK_THAT(loaded.targetDistance(), WithinAbs(original.targetDistance(), 1e-9));
    CHECK(loaded.distanceUnit() == original.distanceUnit());
    CHECK(loaded.sessionDate()  == original.sessionDate());

    // Metadata
    CHECK(loaded.firearm()    == original.firearm());
    CHECK(loaded.ammunition() == original.ammunition());
    CHECK(loaded.notes()      == original.notes());

    // Impacts
    REQUIRE(loaded.impactCount() == original.impactCount());
    for (int i = 0; i < original.impactCount(); ++i)
    {
        auto orig = original.impactAt(i);
        auto load = loaded.impactAt(i);
        CAPTURE(i);
        CHECK(load.id == orig.id);
        CHECK_THAT(load.x, WithinAbs(orig.x, 1e-9));
        CHECK_THAT(load.y, WithinAbs(orig.y, 1e-9));
    }

    // Visualization settings
    CHECK(loaded.showGroupCircle(GroupCircle::Type::Full) == original.showGroupCircle(GroupCircle::Type::Full));
    CHECK(loaded.showGroupCircle(GroupCircle::Type::Percent80) == original.showGroupCircle(GroupCircle::Type::Percent80));
    CHECK(loaded.showGroupCircle(GroupCircle::Type::Percent90) == original.showGroupCircle(GroupCircle::Type::Percent90));

    // Image (only for file round-trips where the image is stored)
    if (expectImage)
    {
        CHECK(loaded.targetImage().size() == original.targetImage().size());
        CHECK_FALSE(loaded.targetImage().isNull());
    }

    // Statistics should have been recalculated
    if (original.impactCount() >= 2)
    {
        CHECK(loaded.statistics().valid);
        CHECK_THAT(loaded.statistics().meanRadiusPixels,
                   WithinAbs(original.statistics().meanRadiusPixels, 1e-6));
    }
}

// ===========================================================================
// JSON round-trip tests (toJson / fromJson)
// ===========================================================================

TEST_CASE("JSON round-trip preserves all document fields", "[serialization][json]")
{
    ShotGroupDocument original;
    populateDocument(original);

    QJsonObject json = original.toJson();

    ShotGroupDocument loaded;
    QString errorMsg;
    REQUIRE(loaded.fromJson(json, &errorMsg));
    INFO("fromJson error: " << errorMsg.toStdString());

    verifyDocumentsMatch(original, loaded);
}

TEST_CASE("JSON round-trip without point of aim", "[serialization][json]")
{
    ShotGroupDocument original;
    original.setBulletDiameter(0.308);
    original.setPixelsPerInch(96.0);
    // Deliberately do NOT set point of aim

    QJsonObject json = original.toJson();

    ShotGroupDocument loaded;
    REQUIRE(loaded.fromJson(json));
    CHECK_FALSE(loaded.hasPointOfAimSet());
    CHECK_THAT(loaded.bulletDiameter(), WithinAbs(0.308, 1e-9));
    CHECK_THAT(loaded.pixelsPerInch(),  WithinAbs(96.0, 1e-9));
}

TEST_CASE("JSON round-trip with meters distance unit", "[serialization][json]")
{
    ShotGroupDocument original;
    original.setBulletDiameter(0.264);
    original.setPixelsPerInch(80.0);
    original.setTargetDistance(200.0);
    original.setDistanceUnit(ShotGroupDocument::DistanceUnit::Meters);

    QJsonObject json = original.toJson();

    ShotGroupDocument loaded;
    REQUIRE(loaded.fromJson(json));
    CHECK(loaded.distanceUnit() == ShotGroupDocument::DistanceUnit::Meters);
    CHECK_THAT(loaded.targetDistance(), WithinAbs(200.0, 1e-9));
}

TEST_CASE("JSON round-trip with empty impacts list", "[serialization][json]")
{
    ShotGroupDocument original;
    original.setBulletDiameter(0.224);
    original.setPixelsPerInch(72.0);
    // No impacts added

    QJsonObject json = original.toJson();

    ShotGroupDocument loaded;
    REQUIRE(loaded.fromJson(json));
    CHECK(loaded.impactCount() == 0);
    CHECK_FALSE(loaded.statistics().valid);
}

TEST_CASE("JSON round-trip preserves impact IDs", "[serialization][json]")
{
    ShotGroupDocument original;
    original.setPixelsPerInch(72.0);
    original.setBulletDiameter(0.224);

    // Add impacts with specific IDs
    int id1 = 1;
    int id2 = 2;
    int id3 = 3;
    original.addImpact(ShotImpact(id1, 10.0, 20.0));
    original.addImpact(ShotImpact(id2, 30.0, 40.0));
    original.addImpact(ShotImpact(id3, 50.0, 60.0));

    QJsonObject json = original.toJson();

    ShotGroupDocument loaded;
    REQUIRE(loaded.fromJson(json));

    REQUIRE(loaded.impactCount() == 3);
    CHECK(loaded.impactAt(0).id == id1);
    CHECK(loaded.impactAt(1).id == id2);
    CHECK(loaded.impactAt(2).id == id3);

    // After loading, nextImpactId should be count + 1
    int nextId = loaded.nextImpactId();
    CHECK(nextId == 4);
}

TEST_CASE("fromJson rejects missing version field", "[serialization][json]")
{
    QJsonObject empty;
    ShotGroupDocument doc;
    QString errorMsg;
    CHECK_FALSE(doc.fromJson(empty, &errorMsg));
    CHECK_FALSE(errorMsg.isEmpty());
}

// ===========================================================================
// File round-trip tests (save / load .spz via DocumentSerializer)
// ===========================================================================

TEST_CASE("SPZ file round-trip preserves all document data", "[serialization][file]")
{
    ShotGroupDocument original;
    populateDocument(original);

    // Save to temp file
    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate(QDir::tempPath() + "/shotplot_test_XXXXXX.spz");
    REQUIRE(tmpFile.open());
    QString path = tmpFile.fileName();
    tmpFile.close();

    QString errorMsg;
    REQUIRE(original.saveToFile(path, &errorMsg));
    INFO("Save error: " << errorMsg.toStdString());

    // Load into a fresh document
    ShotGroupDocument loaded;
    REQUIRE(loaded.loadFromFile(path, &errorMsg));
    INFO("Load error: " << errorMsg.toStdString());

    verifyDocumentsMatch(original, loaded, /*expectImage=*/true);
}

TEST_CASE("SPZ file round-trip with minimal document", "[serialization][file]")
{
    ShotGroupDocument original;

    // Minimal image
    QImage image(50, 50, QImage::Format_ARGB32);
    image.fill(Qt::blue);
    original.setTargetImage(image);

    original.setBulletDiameter(0.308);
    original.setPixelsPerInch(100.0);
    // No POA, no impacts, no metadata

    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate(QDir::tempPath() + "/shotplot_test_XXXXXX.spz");
    REQUIRE(tmpFile.open());
    QString path = tmpFile.fileName();
    tmpFile.close();

    QString errorMsg;
    REQUIRE(original.saveToFile(path, &errorMsg));

    ShotGroupDocument loaded;
    REQUIRE(loaded.loadFromFile(path, &errorMsg));

    CHECK_FALSE(loaded.hasPointOfAimSet());
    CHECK(loaded.impactCount() == 0);
    CHECK_THAT(loaded.bulletDiameter(), WithinAbs(0.308, 1e-9));
    CHECK_THAT(loaded.pixelsPerInch(),  WithinAbs(100.0, 1e-9));
    CHECK(loaded.targetImage().size() == QSize(50, 50));
}

TEST_CASE("SPZ file round-trip preserves image pixel data", "[serialization][file]")
{
    ShotGroupDocument original;

    // Create an image with a known pixel pattern
    QImage image(10, 10, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    image.setPixelColor(0, 0, QColor(255, 0, 0, 255));
    image.setPixelColor(5, 5, QColor(0, 255, 0, 255));
    image.setPixelColor(9, 9, QColor(0, 0, 255, 255));
    original.setTargetImage(image);

    original.setBulletDiameter(0.224);
    original.setPixelsPerInch(72.0);

    QTemporaryFile tmpFile;
    tmpFile.setFileTemplate(QDir::tempPath() + "/shotplot_test_XXXXXX.spz");
    REQUIRE(tmpFile.open());
    QString path = tmpFile.fileName();
    tmpFile.close();

    QString errorMsg;
    REQUIRE(original.saveToFile(path, &errorMsg));

    ShotGroupDocument loaded;
    REQUIRE(loaded.loadFromFile(path, &errorMsg));

    QImage loadedImage = loaded.targetImage();
    REQUIRE(loadedImage.size() == QSize(10, 10));
    CHECK(loadedImage.pixelColor(0, 0) == QColor(255, 0, 0, 255));
    CHECK(loadedImage.pixelColor(5, 5) == QColor(0, 255, 0, 255));
    CHECK(loadedImage.pixelColor(9, 9) == QColor(0, 0, 255, 255));
}

TEST_CASE("Loading non-existent file fails gracefully", "[serialization][file]")
{
    ShotGroupDocument doc;
    QString errorMsg;
    CHECK_FALSE(doc.loadFromFile("/tmp/shotplot_nonexistent_file.spz", &errorMsg));
    CHECK_FALSE(errorMsg.isEmpty());
}
