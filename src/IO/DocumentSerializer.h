#pragma once

#include <QString>

class ShotGroupDocument;

/// @brief Handles saving and loading .spz files using libzip
class DocumentSerializer
{
public:
    /// Save document to .spz file
    static bool save(const ShotGroupDocument &document, const QString &filePath, QString* pErrorMsg = nullptr);
    
    /// Load document from .spz file
    static bool load(ShotGroupDocument &document, const QString &filePath, QString* pErrorMsg = nullptr);

private:
    DocumentSerializer() = delete;
};
