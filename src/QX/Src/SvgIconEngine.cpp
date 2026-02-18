#include "SvgIconEngine.h"

#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

SvgIconEngine::SvgIconEngine(QByteArray svgData)
    : m_svgData(std::move(svgData))
{
}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect,
                          QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
    QSvgRenderer renderer(m_svgData);
    renderer.render(painter, QRectF(rect));
}

QPixmap SvgIconEngine::pixmap(const QSize& size,
                              QIcon::Mode mode, QIcon::State state)
{
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    return pm;
}

QIconEngine* SvgIconEngine::clone() const
{
    return new SvgIconEngine(*this);
}
