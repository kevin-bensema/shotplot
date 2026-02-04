#include "ImageExporter.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

#include <QPainter>
#include <QFileInfo>

bool ImageExporter::exportImage(const ShotGroupDocument &document,
                                TargetScene* pScene,
                                const QString &filePath,
                                const Options &options,
                                QString* pErrorMsg)
{
    QImage image = renderToImage(document, pScene, options);
    
    if (image.isNull())
    {
        if (pErrorMsg)
        {
            *pErrorMsg = "Failed to render image";
        }
        return false;
    }
    
    // Determine format from extension
    QString ext = QFileInfo(filePath).suffix().toLower();
    const char* pFormat = "PNG";
    int quality = -1;
    
    if (ext == "jpg" || ext == "jpeg")
    {
        pFormat = "JPEG";
        quality = options.quality;
    }
    else if (ext == "bmp")
    {
        pFormat = "BMP";
    }
    
    if (!image.save(filePath, pFormat, quality))
    {
        if (pErrorMsg)
        {
            *pErrorMsg = QString("Failed to save image to %1").arg(filePath);
        }
        return false;
    }
    
    return true;
}

QImage ImageExporter::renderToImage(const ShotGroupDocument &document,
                                    TargetScene* pScene,
                                    const Options &options)
{
    Q_UNUSED(options)  // TODO: Use options to filter what's rendered
    
    if (!pScene)
    {
        return QImage();
    }
    
    // Use the document's target image size
    QImage targetImage = document.targetImage();
    if (targetImage.isNull())
    {
        return QImage();
    }
    
    QImage result(targetImage.size(), QImage::Format_ARGB32);
    result.fill(Qt::white);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Render the scene
    pScene->render(&painter);
    
    painter.end();
    
    return result;
}
