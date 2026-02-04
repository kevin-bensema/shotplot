#include "DocumentSerializer.h"
#include <Core/ShotGroupDocument.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QFile>

#include <zip.h>

namespace
{
    constexpr const char* kImageFilename = "target.png";
    constexpr const char* kMetadataFilename = "metadata.json";
}

bool DocumentSerializer::save(const ShotGroupDocument& document, const QString& filePath, QString* pErrorMsg)
{
    int error = 0;
    zip_t* pArchive = zip_open(filePath.toUtf8().constData(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!pArchive)
    {
        if (pErrorMsg)
        {
            *pErrorMsg = QString("Failed to create ZIP archive: error code %1").arg(error);
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
        
        zip_source_t* pImageSource = zip_source_buffer(pArchive, imageData.constData(), imageData.size(), 0);
        if (pImageSource)
        {
            if (zip_file_add(pArchive, kImageFilename, pImageSource, ZIP_FL_OVERWRITE) < 0)
            {
                zip_source_free(pImageSource);
                if (pErrorMsg)
                {
                    *pErrorMsg = QString("Failed to add image to archive: %1").arg(zip_strerror(pArchive));
                }
                success = false;
            }
        }
        else
        {
            if (pErrorMsg)
            {
                *pErrorMsg = "Failed to create image source";
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
        
        zip_source_t* pJsonSource = zip_source_buffer(pArchive, jsonData.constData(), jsonData.size(), 0);
        if (pJsonSource)
        {
            if (zip_file_add(pArchive, kMetadataFilename, pJsonSource, ZIP_FL_OVERWRITE) < 0)
            {
                zip_source_free(pJsonSource);
                if (pErrorMsg)
                {
                    *pErrorMsg = QString("Failed to add metadata to archive: %1").arg(zip_strerror(pArchive));
                }
                success = false;
            }
        }
        else
        {
            if (pErrorMsg)
            {
                *pErrorMsg = "Failed to create metadata source";
            }
            success = false;
        }
    }
    
    zip_close(pArchive);
    return success;
}

bool DocumentSerializer::load(ShotGroupDocument& document, const QString& filePath, QString* pErrorMsg)
{
    int error = 0;
    zip_t* pArchive = zip_open(filePath.toUtf8().constData(), ZIP_RDONLY, &error);
    
    if (!pArchive)
    {
        if (pErrorMsg)
        {
            *pErrorMsg = QString("Failed to open ZIP archive: error code %1").arg(error);
        }
        return false;
    }
    
    bool success = true;
    
    // Load target image
    zip_stat_t imageStat;
    if (zip_stat(pArchive, kImageFilename, 0, &imageStat) == 0)
    {
        zip_file_t* pImageFile = zip_fopen(pArchive, kImageFilename, 0);
        if (pImageFile)
        {
            QByteArray imageData(imageStat.size, 0);
            zip_fread(pImageFile, imageData.data(), imageStat.size);
            zip_fclose(pImageFile);
            
            QImage image;
            if (image.loadFromData(imageData, "PNG"))
            {
                document.setTargetImage(image);
            }
            else
            {
                if (pErrorMsg)
                {
                    *pErrorMsg = "Failed to parse image data";
                }
                success = false;
            }
        }
        else
        {
            if (pErrorMsg)
            {
                *pErrorMsg = QString("Failed to open image file: %1").arg(zip_strerror(pArchive));
            }
            success = false;
        }
    }
    else
    {
        if (pErrorMsg)
        {
            *pErrorMsg = "Image file not found in archive";
        }
        success = false;
    }
    
    // Load metadata
    if (success)
    {
        zip_stat_t jsonStat;
        if (zip_stat(pArchive, kMetadataFilename, 0, &jsonStat) == 0)
        {
            zip_file_t* pJsonFile = zip_fopen(pArchive, kMetadataFilename, 0);
            if (pJsonFile)
            {
                QByteArray jsonData(jsonStat.size, 0);
                zip_fread(pJsonFile, jsonData.data(), jsonStat.size);
                zip_fclose(pJsonFile);
                
                QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
                if (!jsonDoc.isNull() && jsonDoc.isObject())
                {
                    QString parseError;
                    if (!document.fromJson(jsonDoc.object(), &parseError))
                    {
                        if (pErrorMsg)
                        {
                            *pErrorMsg = QString("Failed to parse metadata: %1").arg(parseError);
                        }
                        success = false;
                    }
                }
                else
                {
                    if (pErrorMsg)
                    {
                        *pErrorMsg = "Invalid JSON in metadata file";
                    }
                    success = false;
                }
            }
            else
            {
                if (pErrorMsg)
                {
                    *pErrorMsg = QString("Failed to open metadata file: %1").arg(zip_strerror(pArchive));
                }
                success = false;
            }
        }
        else
        {
            if (pErrorMsg)
            {
                *pErrorMsg = "Metadata file not found in archive";
            }
            success = false;
        }
    }
    
    zip_close(pArchive);
    return success;
}
