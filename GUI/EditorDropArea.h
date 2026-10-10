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

class EditorDropArea : public QFrame
{
    Q_OBJECT

public:
    explicit EditorDropArea(QWidget *parent = nullptr);

    void setImage(const Image& image);
    void removeImage();

    void showCropBox();

signals:
    void imageSelected(const QString& filePath);
    void imageRemoved();

private slots:
    void browseImage();

protected:
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
};

#endif
