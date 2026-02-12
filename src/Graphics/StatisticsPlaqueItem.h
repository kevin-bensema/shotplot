#pragma once

#include <QGraphicsObject>
#include <QFont>

class ShotGroupDocument;
class GraphicsSettingsService;

/// @brief Interactive statistics plaque overlay for the target visualization
///
/// Renders a two-region plaque on the graphics scene: a title bar at the top
/// and a statistics body below. The title bar uses a bold font 2pt larger than
/// the default; the body uses the default font. Statistics lines that overflow
/// are elided with an ellipsis.
///
/// The plaque supports interactive dragging by its title bar and resizing via
/// a grip handle in the bottom-right corner. Position and size changes are
/// written back to the document's PlaqueConfig.
///
/// Colors are read from GraphicsSettingsService; overall opacity is controlled
/// by the Plaque opacity role.
class StatisticsPlaqueItem : public QGraphicsObject
{
    Q_OBJECT

public:
    /// Constructs a statistics plaque item
    ///
    /// \param pDocument The document providing plaque config and statistics
    /// \param pParent Optional parent graphics item
    explicit StatisticsPlaqueItem(ShotGroupDocument* pDocument, QGraphicsItem* pParent = nullptr);

    /// Re-reads PlaqueConfig and Statistics from the document and repaints
    void updateFromDocument();

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent) override;

private:
    /// Returns the title bar rectangle in local coordinates
    QRectF titleBarRect() const;
    /// Returns the body rectangle in local coordinates
    QRectF bodyRect() const;
    /// Returns the resize handle hit area in local coordinates
    QRectF resizeHandleRect() const;
    /// Formats the list of enabled statistics as display strings
    QStringList formatStatistics() const;

    ShotGroupDocument* m_pDocument;
    GraphicsSettingsService& m_service;

    // Cached layout
    int m_width = 300;
    int m_height = 200;
    int m_titleBarHeight = 0;       ///< Computed from title font metrics
    QFont m_titleFont;
    QFont m_bodyFont;

    // Interaction state
    enum class DragMode { None, Move, Resize };
    DragMode m_dragMode = DragMode::None;
    QPointF m_dragStartPos;         ///< Scene position at drag start
    QPointF m_dragStartItemPos;     ///< Item position at drag start
    int m_dragStartWidth = 0;
    int m_dragStartHeight = 0;

    static constexpr int kResizeHandleSize = 12;
    static constexpr int kTitleBarPadding = 4;
    static constexpr int kBodyPadding = 6;
    static constexpr int kMinWidth = 120;
    static constexpr int kMinHeight = 80;
};
