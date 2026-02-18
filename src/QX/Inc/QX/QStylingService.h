#pragma once

#include <QX/Export.h>
#include <QX/QService.h>

#include <QByteArray>
#include <QColor>
#include <QHash>
#include <QIcon>
#include <QPalette>
#include <QString>

/// Service for creating resolution-independent themed icons from SVG resources.
///
/// QStylingService parses SVG files as XML and rewrites fill colors throughout
/// the document to a caller-supplied tint color. The resulting icon is backed
/// by a custom QIconEngine that renders via QSvgRenderer at whatever size Qt's
/// style machinery requests, so it is fully HiDPI-aware with no fixed pixel
/// resolution baked in.
///
/// Raw SVG bytes are cached by resource path to avoid repeated file I/O;
/// callers that need icons to track live palette changes are responsible for
/// re-requesting after a palette-change event.
class QX_EXPORT QStylingService : public QService
{
    Q_OBJECT

public:
    explicit QStylingService(QObject* parent = nullptr);

    /// Creates a themed icon from an SVG resource, recoloring all fills to
    /// match QPalette::WindowText from the supplied palette.
    ///
    /// Typical usage: service.createThemedIconFromSvg(":/icons/foo.svg", widget->palette())
    ///
    /// \param resourcePath Qt resource path, e.g. ":/icons/my-icon.svg"
    /// \param palette      Palette from which QPalette::WindowText supplies the tint color
    /// \return             A QIcon backed by a resolution-independent SVG engine,
    ///                     or a null QIcon if the resource cannot be loaded or parsed.
    QIcon createThemedIconFromSvg(const QString& resourcePath, const QPalette& palette);

    /// Creates a themed icon from an SVG resource, recoloring all fills to
    /// the given color.
    ///
    /// \param resourcePath Qt resource path, e.g. ":/icons/my-icon.svg"
    /// \param tintColor    Target fill color
    /// \return             A QIcon backed by a resolution-independent SVG engine,
    ///                     or a null QIcon if the resource cannot be loaded or parsed.
    QIcon createThemedIconFromSvg(const QString& resourcePath, const QColor& tintColor);

private:
    /// Loads raw SVG bytes from a Qt resource path, with caching.
    QByteArray loadSvgBytes(const QString& resourcePath);

    /// Parses svgBytes as an XML document and rewrites all fill colors to
    /// tintColor, returning the modified document serialized back to bytes.
    /// Returns an empty QByteArray on parse failure.
    QByteArray recolorSvg(const QByteArray& svgBytes, const QColor& tintColor);

    /// Cached raw SVG bytes keyed by resource path.
    QHash<QString, QByteArray> m_rawSvgCache;
};
