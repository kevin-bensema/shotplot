#pragma once

#include <QIconEngine>
#include <QByteArray>

/// Private icon engine that renders a recolored SVG at any requested size.
///
/// Stores the SVG document as raw bytes and uses QSvgRenderer on each paint
/// call, so the icon is fully resolution-independent and HiDPI-aware. Qt's
/// icon machinery calls paint() or pixmap() at whatever size the style or
/// widget requests — no fixed resolution is baked in.
class SvgIconEngine : public QIconEngine
{
public:
    explicit SvgIconEngine(QByteArray svgData);

    void paint(QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state) override;

    QPixmap pixmap(const QSize& size,
                   QIcon::Mode mode, QIcon::State state) override;

    QIconEngine* clone() const override;

private:
    QByteArray m_svgData;
};
