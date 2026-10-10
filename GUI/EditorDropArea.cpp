#include "EditorDropArea.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPixmap>
#include <QStyle>
#include <QPainter>
#include <QImageReader>
#include <QDebug>

EditorDropArea::EditorDropArea(QWidget *parent)
    : QFrame(parent)
{
    setAcceptDrops(true);

    setMinimumSize(350, 250);



    // -------------------------
    // Icon
    // -------------------------

    iconLabel = new QLabel(this);

    iconLabel->setText("＋");
    iconLabel->setAlignment(Qt::AlignCenter);

    iconLabel->setFixedSize(70, 70);

    iconLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #252525;"
        "    color: #8ab4f8;"
        "    border-radius: 35px;"
        "    font-size: 36px;"
        "    font-weight: 300;"
        "}"
        );

    // -------------------------
    // Title
    // -------------------------

    titleLabel = new QLabel("Drop an image here", this);

    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eeeeee;"
        "    font-size: 20px;"
        "    font-weight: 600;"
        "}"
        );

    // -------------------------
    // Subtitle
    // -------------------------

    subtitleLabel = new QLabel(
        "Drag & drop your image here or browse your computer",
        this
        );

    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);

    subtitleLabel->setStyleSheet(
        "QLabel {"
        "    color: #888888;"
        "    font-size: 13px;"
        "}"
        );

    // -------------------------
    // Browse button
    // -------------------------

    browseButton = new QPushButton("Browse files", this);

    browseButton->setCursor(Qt::PointingHandCursor);

    browseButton->setMinimumSize(130, 40);

    browseButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3b82f6;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 8px 18px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "}"
        ""
        "QPushButton:hover {"
        "    background-color: #2563eb;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #1d4ed8;"
        "}"
        );

    // ----------------
    // Image Holder
    // ----------------


    emptyWidget = new QWidget(this);
    imageWidget = new QWidget(this);


    graphicsView = new QGraphicsView(imageWidget);

    graphicsView->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    graphicsView->setFrameShape(QFrame::NoFrame);

    graphicsView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    graphicsView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
        );

    graphicsView->setStyleSheet(
        "QGraphicsView {"
        "    background-color: #111111;"
        "    border: none;"
        "}"
        );

    scene = new QGraphicsScene(this);

    graphicsView->setScene(scene);

    pixmapItem = nullptr;


    QVBoxLayout *imageLayout = new QVBoxLayout(imageWidget);

    imageLayout->setContentsMargins(0, 0, 0, 0);
    imageLayout->setSpacing(0);

    imageLayout->addWidget(graphicsView);



    QVBoxLayout *emptyLayout = new QVBoxLayout(emptyWidget);

    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setContentsMargins(30, 30, 30, 30);
    emptyLayout->setSpacing(10);

    emptyLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    emptyLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    emptyLayout->addWidget(subtitleLabel, 0, Qt::AlignCenter);

    emptyLayout->addSpacing(8);

    emptyLayout->addWidget(browseButton,0,Qt::AlignCenter);



    // stack layout

    stackedLayout = new QStackedLayout(this);

    stackedLayout->setContentsMargins(30, 30, 30, 30);


    stackedLayout->addWidget(emptyWidget);
    stackedLayout->addWidget(imageWidget);

    stackedLayout->setCurrentWidget(emptyWidget);


    connect(
        browseButton,
        &QPushButton::clicked,
        this,
        &EditorDropArea::browseImage
        );

    setNormalStyle();


}

void EditorDropArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasUrls())
    {
        event->ignore();
        return;
    }

    for (const QUrl &url : event->mimeData()->urls())
    {
        if (url.isLocalFile())
        {
            QString filePath = url.toLocalFile();

            QImageReader reader(filePath);

            if (reader.canRead())
            {
                setDragStyle();
                event->acceptProposedAction();
                return;
            }
        }
    }

    event->ignore();
}

void EditorDropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    setNormalStyle();
    event->accept();
}

void EditorDropArea::dropEvent(QDropEvent *event)
{
    setNormalStyle();

    const QList<QUrl> urls = event->mimeData()->urls();

    if (urls.isEmpty())
    {
        event->ignore();
        return;
    }

    for (const QUrl &url : urls)
    {
        if (!url.isLocalFile())
            continue;

        QString filePath = url.toLocalFile();

        QImageReader reader(filePath);

        if (reader.canRead())
        {
            handleImageFile(filePath);
            event->acceptProposedAction();
            return;
        }
    }

    event->ignore();
}

void EditorDropArea::browseImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Image",
        QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.webp *.ppm)"
        );

    if (!filePath.isEmpty())
    {
        handleImageFile(filePath);
    }
}

void EditorDropArea::handleImageFile(const QString &filePath)
{
    emit imageSelected(filePath);
}

void EditorDropArea::setNormalStyle()
{
    setStyleSheet(
        "EditorDropArea {"
        "    background-color: #171717;"
        "    border: 2px dashed #383838;"
        "    border-radius: 16px;"
        "}"
        );

    titleLabel->setText("Drop an image here");
    subtitleLabel->setText(
        "Drag & drop your image here or browse your computer"
        );
}

void EditorDropArea::setDragStyle()
{
    setStyleSheet(
        "EditorDropArea {"
        "    background-color: #1d2735;"
        "    border: 2px dashed #3b82f6;"
        "    border-radius: 16px;"
        "}"
        );

    titleLabel->setText("Drop image to open");
    subtitleLabel->setText("Release the mouse to import your image");
}

void EditorDropArea::setImage(const Image& image)
{
    currentQImage = imageToQImage(image);

    scene->clear();

    QPixmap pixmap = QPixmap::fromImage(currentQImage);

    pixmapItem = scene->addPixmap(pixmap);

    scene->setSceneRect(
        pixmapItem->boundingRect()
        );

    showImageState();

    graphicsView->fitInView(
        pixmapItem,
        Qt::KeepAspectRatio
        );
}

void EditorDropArea::removeImage(){
    currentQImage = QImage();

    scene->clear();

    showEmptyState();

    emit imageRemoved();
}

void EditorDropArea::showCropBox() {
    if (pixmapItem && !cropBox) {
        cropBox = new CropBox(pixmapItem->boundingRect());
        scene->addItem(cropBox);
    } else if (cropBox) {
        cropBox->show();
    }
}

void EditorDropArea::cropConfirmed() {
    if (!cropBox)
        return;

    QRect cropRect = cropBox->rect().toRect();

    if (!currentQImage.isNull()) {

        cropRect = cropRect.intersected(QRect(0, 0, currentQImage.width(), currentQImage.height()));


        qDebug() << cropRect.x() << " " << cropRect.y() << " " << cropRect.width() << " " << cropRect.height() << "\n";

        if (cropRect.width() > 0 && cropRect.height() > 0) {
            emit cropRequested(cropRect);
        }
    }


    // delete cropBox;
    cropBox->hide();
}

void EditorDropArea::cropCancelled() {
    if (cropBox) {
        // delete cropBox;
        cropBox->hide();
    }
}


void EditorDropArea::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    if (pixmapItem)
    {
        graphicsView->fitInView(
            pixmapItem,
            Qt::KeepAspectRatio
            );
    }
}

QImage EditorDropArea::imageToQImage(const Image& image){
    return QImage(image.imageData , image.width , image.height , image.width * image.channels , QImage::Format_RGB888);
}



void EditorDropArea::showEmptyState()
{
    stackedLayout->setCurrentWidget(emptyWidget);
}


void EditorDropArea::showImageState()
{
    stackedLayout->setCurrentWidget(imageWidget);
}
