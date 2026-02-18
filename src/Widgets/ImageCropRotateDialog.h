#pragma once

#include <QDialog>
#include <QImage>
#include <QGraphicsView>

class QGraphicsScene;
class QGraphicsPixmapItem;
class CroppingToolItem;
class QPushButton;

class ImageCropRotateView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageCropRotateView(QWidget* pParent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
};

class ImageCropRotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageCropRotateDialog(const QImage& image, QWidget* pParent = nullptr);
    ~ImageCropRotateDialog() override;

    QImage resultImage() const;

private slots:
    void onRotateLeft();
    void onRotateRight();
    void onReset();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void setupUi();
    void updateScene();

    QImage m_originalImage;
    QImage m_currentImage;
    
    ImageCropRotateView* m_pView = nullptr;
    QGraphicsScene* m_pScene = nullptr;
    QGraphicsPixmapItem* m_pPixmapItem = nullptr;
    CroppingToolItem* m_pCroppingTool = nullptr;
    
    QPushButton* m_pRotateLeftBtn = nullptr;
    QPushButton* m_pRotateRightBtn = nullptr;
    QPushButton* m_pResetBtn = nullptr;
};
