#include "ImageExporter.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

#include <QPainter>
#include <QFileInfo>

bool ImageExporter::exportImage(const ShotGroupDocument &document,
                                TargetScene *scene,
                                const QString &filePath,
                                const Options &options,
                                QString *errorMsg)
{
    QImage image = renderToImage(document, scene, options);
    
    if (image.isNull())
    {
        if (errorMsg)
        {
            *errorMsg = "Failed to render image";
        }
        return false;
    }
    
    // Determine format from extension
    QString ext = QFileInfo(filePath).suffix().toLower();
    const char *format = "PNG";
    int quality = -1;
    
    if (ext == "jpg" || ext == "jpeg")
    {
        format = "JPEG";
        quality = options.quality;
    }
    else if (ext == "bmp")
    {
        format = "BMP";
    }
    
    if (!image.save(filePath, format, quality))
    {
        if (errorMsg)
        {
            *errorMsg = QString("Failed to save image to %1").arg(filePath);
        }
        return false;
    }
    
    return true;
}

QImage ImageExporter::renderToImage(const ShotGroupDocument &document,
                                    TargetScene *scene,
                                    const Options &options)
{
    Q_UNUSED(options)  // TODO: Use options to filter what's rendered
    
    if (!scene)
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
    scene->render(&painter);
    
    painter.end();
    
    return result;
}
