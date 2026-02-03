#pragma once

#include <QString>

class ShotGroupDocument;

/// @brief Handles saving and loading .spz files using libzip
class DocumentSerializer
{
public:
    /// Save document to .spz file
    static bool save(const ShotGroupDocument &document, const QString &filePath, QString *errorMsg = nullptr);
    
    /// Load document from .spz file
    static bool load(ShotGroupDocument &document, const QString &filePath, QString *errorMsg = nullptr);

private:
    DocumentSerializer() = delete;
    
    static constexpr const char* IMAGE_FILENAME = "target.png";
    static constexpr const char* METADATA_FILENAME = "metadata.json";
};
