#include "DocumentSerializer.h"
#include "Core/ShotGroupDocument.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QFile>

#include <zip.h>

bool DocumentSerializer::save(const ShotGroupDocument &document, const QString &filePath, QString *errorMsg)
{
    int error = 0;
    zip_t *archive = zip_open(filePath.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!archive)
    {
        if (errorMsg)
        {
            *errorMsg = QString("Failed to create ZIP archive: error code %1").arg(error);
        }
        return false;
    }
    
    bool success = true;
    
    // Save target image as PNG
    QImage image = document.targetImage();
    if (!image.isNull())
    {
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        buffer.close();
        
        zip_source_t *imageSource = zip_source_buffer(archive, imageData.constData(), imageData.size(), 0);
        if (imageSource)
        {
            if (zip_file_add(archive, IMAGE_FILENAME, imageSource, ZIP_FL_OVERWRITE) < 0)
            {
                zip_source_free(imageSource);
                if (errorMsg)
                {
                    *errorMsg = QString("Failed to add image to archive: %1").arg(zip_strerror(archive));
                }
                success = false;
            }
        }
        else
        {
            if (errorMsg)
            {
                *errorMsg = "Failed to create image source";
            }
            success = false;
        }
    }
    
    // Save metadata as JSON
    if (success)
    {
        QJsonObject json = document.toJson();
        QJsonDocument jsonDoc(json);
        QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
        
        zip_source_t *jsonSource = zip_source_buffer(archive, jsonData.constData(), jsonData.size(), 0);
        if (jsonSource)
        {
            if (zip_file_add(archive, METADATA_FILENAME, jsonSource, ZIP_FL_OVERWRITE) < 0)
            {
                zip_source_free(jsonSource);
                if (errorMsg)
                {
                    *errorMsg = QString("Failed to add metadata to archive: %1").arg(zip_strerror(archive));
                }
                success = false;
            }
        }
        else
        {
            if (errorMsg)
            {
                *errorMsg = "Failed to create metadata source";
            }
            success = false;
        }
    }
    
    zip_close(archive);
    return success;
}

bool DocumentSerializer::load(ShotGroupDocument &document, const QString &filePath, QString *errorMsg)
{
    int error = 0;
    zip_t *archive = zip_open(filePath.toUtf8().constData(), ZIP_RDONLY, &error);
    
    if (!archive)
    {
        if (errorMsg)
        {
            *errorMsg = QString("Failed to open ZIP archive: error code %1").arg(error);
        }
        return false;
    }
    
    bool success = true;
    
    // Load target image
    zip_stat_t imageStat;
    if (zip_stat(archive, IMAGE_FILENAME, 0, &imageStat) == 0)
    {
        zip_file_t *imageFile = zip_fopen(archive, IMAGE_FILENAME, 0);
        if (imageFile)
        {
            QByteArray imageData(imageStat.size, 0);
            zip_fread(imageFile, imageData.data(), imageStat.size);
            zip_fclose(imageFile);
            
            QImage image;
            if (image.loadFromData(imageData, "PNG"))
            {
                document.setTargetImage(image);
            }
            else
            {
                if (errorMsg)
                {
                    *errorMsg = "Failed to parse image data";
                }
                success = false;
            }
        }
        else
        {
            if (errorMsg)
            {
                *errorMsg = QString("Failed to open image file: %1").arg(zip_strerror(archive));
            }
            success = false;
        }
    }
    else
    {
        if (errorMsg)
        {
            *errorMsg = "Image file not found in archive";
        }
        success = false;
    }
    
    // Load metadata
    if (success)
    {
        zip_stat_t jsonStat;
        if (zip_stat(archive, METADATA_FILENAME, 0, &jsonStat) == 0)
        {
            zip_file_t *jsonFile = zip_fopen(archive, METADATA_FILENAME, 0);
            if (jsonFile)
            {
                QByteArray jsonData(jsonStat.size, 0);
                zip_fread(jsonFile, jsonData.data(), jsonStat.size);
                zip_fclose(jsonFile);
                
                QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
                if (!jsonDoc.isNull() && jsonDoc.isObject())
                {
                    QString parseError;
                    if (!document.fromJson(jsonDoc.object(), &parseError))
                    {
                        if (errorMsg)
                        {
                            *errorMsg = QString("Failed to parse metadata: %1").arg(parseError);
                        }
                        success = false;
                    }
                }
                else
                {
                    if (errorMsg)
                    {
                        *errorMsg = "Invalid JSON in metadata file";
                    }
                    success = false;
                }
            }
            else
            {
                if (errorMsg)
                {
                    *errorMsg = QString("Failed to open metadata file: %1").arg(zip_strerror(archive));
                }
                success = false;
            }
        }
        else
        {
            if (errorMsg)
            {
                *errorMsg = "Metadata file not found in archive";
            }
            success = false;
        }
    }
    
    zip_close(archive);
    return success;
}
