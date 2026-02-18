#include <QX/QStylingService.h>

#include "SvgIconEngine.h"

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QPalette>

namespace {

/// Rewrites the fill color within a CSS-style attribute string.
/// Handles the "fill:#rrggbb" or "fill:color-name" token while preserving
/// all other style properties.
QString recolorStyleAttribute(const QString& style, const QString& hexColor)
{
    QStringList properties = style.split(';', Qt::SkipEmptyParts);
    for (QString& prop : properties)
    {
        const QString trimmed = prop.trimmed();
        if (trimmed.startsWith(QLatin1String("fill:"), Qt::CaseInsensitive))
        {
            const QString value = trimmed.mid(5).trimmed();
            // Preserve fill:none — it marks intentionally transparent elements.
            if (value.compare(QLatin1String("none"), Qt::CaseInsensitive) == 0)
                continue;
            prop = QStringLiteral("fill:") + hexColor;
        }
    }
    return properties.join(';');
}

/// Recursively recolors fill attributes throughout an SVG DOM subtree.
void recolorElement(QDomElement element, const QString& hexColor)
{
    if (element.isNull())
        return;

    // Handle the 'fill' presentation attribute.
    if (element.hasAttribute(QStringLiteral("fill")))
    {
        const QString current = element.attribute(QStringLiteral("fill")).trimmed();
        if (current.compare(QLatin1String("none"), Qt::CaseInsensitive) != 0)
            element.setAttribute(QStringLiteral("fill"), hexColor);
    }
    else
    {
        // No explicit fill — SVG defaults to black for shape elements.
        // Inject an explicit fill so the tint color applies correctly.
        // We skip the root <svg> element itself (no tag-name filter needed;
        // setting fill on <svg> is harmless and acts as an inherited default).
        const QString tag = element.tagName().toLower();
        // Only inject on leaf content elements; skip structural containers
        // whose children will be handled by recursion.
        static const QStringList contentElements = {
            QStringLiteral("path"),    QStringLiteral("circle"),
            QStringLiteral("ellipse"), QStringLiteral("rect"),
            QStringLiteral("polygon"), QStringLiteral("polyline"),
            QStringLiteral("line"),    QStringLiteral("text"),
            QStringLiteral("tspan"),   QStringLiteral("use"),
        };
        if (contentElements.contains(tag))
            element.setAttribute(QStringLiteral("fill"), hexColor);
    }

    // Handle fill embedded inside the 'style' CSS attribute.
    if (element.hasAttribute(QStringLiteral("style")))
    {
        const QString recolored = recolorStyleAttribute(
            element.attribute(QStringLiteral("style")), hexColor);
        element.setAttribute(QStringLiteral("style"), recolored);
    }

    // Recurse into child elements.
    QDomElement child = element.firstChildElement();
    while (!child.isNull())
    {
        recolorElement(child, hexColor);
        child = child.nextSiblingElement();
    }
}

} // anonymous namespace

// ---------------------------------------------------------------------------

QStylingService::QStylingService(QObject* parent)
    : QService(parent)
{
}

QIcon QStylingService::createThemedIconFromSvg(const QString& resourcePath,
                                               const QPalette& palette)
{
    return createThemedIconFromSvg(resourcePath,
                                   palette.color(QPalette::WindowText));
}

QIcon QStylingService::createThemedIconFromSvg(const QString& resourcePath,
                                               const QColor& tintColor)
{
    const QByteArray raw = loadSvgBytes(resourcePath);
    if (raw.isEmpty())
        return {};

    const QByteArray recolored = recolorSvg(raw, tintColor);
    if (recolored.isEmpty())
        return {};

    return QIcon(new SvgIconEngine(recolored));
}

QByteArray QStylingService::loadSvgBytes(const QString& resourcePath)
{
    auto it = m_rawSvgCache.find(resourcePath);
    if (it != m_rawSvgCache.end())
        return it.value();

    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("QStylingService: cannot open SVG resource '%s'",
                 qUtf8Printable(resourcePath));
        return {};
    }

    const QByteArray bytes = file.readAll();
    m_rawSvgCache.insert(resourcePath, bytes);
    return bytes;
}

QByteArray QStylingService::recolorSvg(const QByteArray& svgBytes,
                                       const QColor& tintColor)
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!doc.setContent(svgBytes, &errorMsg, &errorLine, &errorColumn))
    {
        qWarning("QStylingService: SVG parse error at line %d col %d: %s",
                 errorLine, errorColumn, qUtf8Printable(errorMsg));
        return {};
    }

    const QString hexColor = tintColor.name(QColor::HexRgb);
    recolorElement(doc.documentElement(), hexColor);

    return doc.toByteArray();
}
