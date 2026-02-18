#include "ImageCropRotateDialog.h"
#include "Graphics/CroppingToolItem.h"

#include <QX/Services.h>
#include <QX/QStylingService.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QWheelEvent>

namespace {
    constexpr double kZoomStep = 1.15;
    constexpr double kMinZoom = 0.1;
    constexpr double kMaxZoom = 10.0;
    constexpr double kScenePaddingFactor = 0.2;
}

ImageCropRotateView::ImageCropRotateView(QWidget* pParent)
    : QGraphicsView(pParent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QBrush(QColor(240, 240, 240))); // Light gray background
}

void ImageCropRotateView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) {
        double newZoom = transform().m11() * kZoomStep;
        if (newZoom <= kMaxZoom) {
            scale(kZoomStep, kZoomStep);
        }
    } else {
        double newZoom = transform().m11() / kZoomStep;
        if (newZoom >= kMinZoom) {
            scale(1.0 / kZoomStep, 1.0 / kZoomStep);
        }
    }
    event->accept();
}

ImageCropRotateDialog::ImageCropRotateDialog(const QImage& image, QWidget* pParent)
    : QDialog(pParent)
    , m_originalImage(image)
    , m_currentImage(image)
{
    setWindowTitle(tr("Crop and Rotate Image"));
    setupUi();
    updateScene();
    
    // Default size
    resize(800, 600);
}

ImageCropRotateDialog::~ImageCropRotateDialog() = default;

QImage ImageCropRotateDialog::resultImage() const
{
    if (!m_pCroppingTool) return m_currentImage;
    
    QRectF cropRect = m_pCroppingTool->cropRect();
    return m_currentImage.copy(cropRect.toRect());
}

void ImageCropRotateDialog::onRotateLeft()
{
    m_currentImage = m_currentImage.transformed(QTransform().rotate(-90));
    m_pPixmapItem->setPixmap(QPixmap::fromImage(m_currentImage));
    m_pCroppingTool->rotate90(false);
    m_pScene->setSceneRect(m_currentImage.rect());
}

void ImageCropRotateDialog::onRotateRight()
{
    m_currentImage = m_currentImage.transformed(QTransform().rotate(90));
    m_pPixmapItem->setPixmap(QPixmap::fromImage(m_currentImage));
    m_pCroppingTool->rotate90(true);
    m_pScene->setSceneRect(m_currentImage.rect());
}

void ImageCropRotateDialog::onReset()
{
    m_currentImage = m_originalImage;
    updateScene();
}

void ImageCropRotateDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    if (m_pView && m_pScene) {
        m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void ImageCropRotateDialog::setupUi()
{
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    
    // Instructions
    pMainLayout->addWidget(new QLabel(tr("Adjust the cropping area and rotate the image if necessary:")));
    
    // Rotation buttons, centered
    auto& styling = qx::GetService<QStylingService>();
    const QSize iconSize(32, 32);

    m_pRotateLeftBtn = new QPushButton(tr("Rotate Left"), this);
    m_pRotateLeftBtn->setIcon(styling.createThemedIconFromSvg(":/icons/rotate-left.svg", palette()));
    m_pRotateLeftBtn->setIconSize(iconSize);
    connect(m_pRotateLeftBtn, &QPushButton::clicked, this, &ImageCropRotateDialog::onRotateLeft);

    m_pRotateRightBtn = new QPushButton(tr("Rotate Right"), this);
    m_pRotateRightBtn->setIcon(styling.createThemedIconFromSvg(":/icons/rotate-right.svg", palette()));
    m_pRotateRightBtn->setIconSize(iconSize);
    connect(m_pRotateRightBtn, &QPushButton::clicked, this, &ImageCropRotateDialog::onRotateRight);

    QHBoxLayout* pRotateLayout = new QHBoxLayout();
    pRotateLayout->addStretch();
    pRotateLayout->addWidget(m_pRotateLeftBtn);
    pRotateLayout->addWidget(m_pRotateRightBtn);
    pRotateLayout->addStretch();
    pMainLayout->addLayout(pRotateLayout);
    
    // Graphics View
    m_pView = new ImageCropRotateView(this);
    m_pScene = new QGraphicsScene(this);
    m_pView->setScene(m_pScene);
    pMainLayout->addWidget(m_pView, 1);
    
    // Reset button at the bottom
    QHBoxLayout* pBottomLayout = new QHBoxLayout();
    
    m_pResetBtn = new QPushButton(tr("Reset"), this);
    connect(m_pResetBtn, &QPushButton::clicked, this, &ImageCropRotateDialog::onReset);
    pBottomLayout->addWidget(m_pResetBtn);
    
    pBottomLayout->addStretch();
    
    pMainLayout->addLayout(pBottomLayout);
    
    // Dialog Buttons
    QDialogButtonBox* pButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    pMainLayout->addWidget(pButtonBox);
}

void ImageCropRotateDialog::updateScene()
{
    m_pScene->clear();
    
    m_pPixmapItem = m_pScene->addPixmap(QPixmap::fromImage(m_currentImage));
    m_pPixmapItem->setZValue(0);
    
    m_pCroppingTool = new CroppingToolItem(m_currentImage.rect());
    m_pScene->addItem(m_pCroppingTool);
    
    QRectF imageRect = m_currentImage.rect();
    qreal padW = imageRect.width() * kScenePaddingFactor;
    qreal padH = imageRect.height() * kScenePaddingFactor;
    m_pScene->setSceneRect(imageRect.adjusted(-padW, -padH, padW, padH));
    m_pView->fitInView(m_pScene->sceneRect(), Qt::KeepAspectRatio);
}
