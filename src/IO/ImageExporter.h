#pragma once

#include <QString>
#include <QImage>

class ShotGroupDocument;
class TargetScene;

/// @brief Configuration options for image export
/// 
/// Controls which visual elements are included in the exported image
/// and quality settings for lossy formats.
struct ImageExportOptions
{
    bool includeMarkers = true;        ///< Include shot impact markers
    bool includeGroupCircles = true;   ///< Include group boundary circles
    bool includePOA = true;            ///< Include point of aim marker
    bool includePlaque = true;         ///< Include statistics plaque overlay
    int quality = 95;                  ///< JPEG quality (1-100), ignored for other formats
};

/// @brief Static utility class for exporting annotated target images
/// 
/// Provides functionality to render a TargetScene with all annotations
/// (shot impacts, group circles, point of aim, statistics plaque) onto
/// the document's target image and export it to various image formats.
/// 
/// The exporter uses the target image from the document as the base layer
/// and renders the scene's graphics items on top. Supported formats include
/// PNG, JPEG, and BMP, with format detection based on file extension.
class ImageExporter
{
public:
    using Options = ImageExportOptions;

    /// Exports the annotated scene to an image file
    ///
    /// Renders the scene with all annotations onto the document's target image
    /// and saves it to the specified file path. The image format is determined
    /// from the file extension (.png, .jpg/.jpeg, .bmp).
    ///
    /// \param document The document containing the target image and calibration data
    /// \param scene The graphics scene to render (must not be null)
    /// \param filePath Output file path (format inferred from extension)
    /// \param options Export options controlling which elements to include
    /// \param errorMsg Optional pointer to receive error message on failure
    /// \return True if export succeeded, false otherwise
    /// \note Returns false if scene is null, target image is invalid, or file save fails
    static bool exportImage(const ShotGroupDocument &document,
                           TargetScene* pScene,
                           const QString &filePath,
                           const Options &options = Options(),
                           QString* pErrorMsg = nullptr);
    
    /// Renders the annotated scene to a QImage
    ///
    /// Creates a QImage by rendering the scene's graphics items onto the
    /// document's target image. The resulting image has the same dimensions
    /// as the target image and uses ARGB32 format with antialiasing enabled.
    ///
    /// \param document The document containing the target image
    /// \param scene The graphics scene to render (must not be null)
    /// \param options Export options (currently not fully implemented)
    /// \return Rendered QImage, or null QImage if scene is null or target image is invalid
    static QImage renderToImage(const ShotGroupDocument &document,
                                TargetScene* pScene,
                                const Options &options = Options());

private:
    ImageExporter() = delete;
};
