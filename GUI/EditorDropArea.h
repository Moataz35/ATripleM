#ifndef EDITORDROPAREA_H
#define EDITORDROPAREA_H

#include <QFrame>
#include <QImage>
#include "Image.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStackedLayout>
#include "CropBox.h"

class QLabel;
class QPushButton;
class QResizeEvent;
class QEvent;

class EditorDropArea : public QFrame
{
    Q_OBJECT

public:
    explicit EditorDropArea(QWidget *parent = nullptr);

    void setImage(const Image& image);
    void removeImage();

    void showCropBox();
    void cropConfirmed();
    void cropCancelled();

signals:
    void imageSelected(const QString& filePath);
    void imageRemoved();
    void cropRequested(QRect cropRect);
    void zoomChanged(int percent);

private slots:
    void browseImage();

public slots:
    void setZoomPercent(int percent);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void handleImageFile(const QString& filePath);

    void showEmptyState();
    void showImageState();
    void setNormalStyle();
    void setDragStyle();
    void fitImageToView();
    void setCropInteractionEnabled(bool enabled);

    QStackedLayout *stackedLayout;

    QWidget *emptyWidget;
    QWidget *imageWidget;

    QImage imageToQImage(const Image& image);
    void updatePreview();

    QLabel *iconLabel;
    QLabel *titleLabel;
    QLabel *subtitleLabel;

    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;

    CropBox *cropBox = nullptr;

    QPushButton *browseButton;
    QPushButton *removeButton;

    QImage currentQImage;
    int zoomPercent = 100;
    bool fitToView = true;

    static constexpr int MinZoomPercent = 10;
    static constexpr int MaxZoomPercent = 400;
};

#endif
