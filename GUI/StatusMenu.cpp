#include "StatusMenu.h"

#include "Image.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QTimer>

namespace {

QString imageSizeText(qint64 bytes)
{
    constexpr qint64 kibibyte = 1024;
    constexpr qint64 mebibyte = kibibyte * 1024;
    if (bytes >= mebibyte) {
        return QString("%1 MiB").arg(bytes / static_cast<double>(mebibyte), 0, 'f', 2);
    }
    if (bytes >= kibibyte) {
        return QString("%1 KiB").arg(bytes / static_cast<double>(kibibyte), 0, 'f', 1);
    }
    return QString("%1 B").arg(bytes);
}

}

class SpinnerWidget : public QWidget
{
public:
    explicit SpinnerWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , timer(new QTimer(this))
    {
        setFixedSize(18, 18);
        connect(timer, &QTimer::timeout, this, [this]() {
            angle = (angle + 30) % 360;
            update();
        });
    }

    void setSpinning(bool spinning)
    {
        if (spinning) {
            angle = 0;
            timer->start(80);
            show();
        } else {
            timer->stop();
            hide();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor("#60a5fa"), 2.2, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(rect().adjusted(3, 3, -3, -3), angle * 16, 250 * 16);
    }

private:
    QTimer *timer;
    int angle = 0;
};

StatusMenu::StatusMenu(QWidget *parent)
    : QWidget(parent)
    , spinner(new SpinnerWidget(this))
    , operationStatusLabel(new QLabel("Ready", this))
    , imageInfoLabel(new QLabel("No image", this))
{
    setObjectName("imageStatusPanel");
    setFixedHeight(34);

    auto *statusLayout = new QHBoxLayout(this);
    statusLayout->setContentsMargins(10, 0, 10, 0);
    statusLayout->setSpacing(8);

    operationStatusLabel->setObjectName("operationStatusLabel");
    imageInfoLabel->setObjectName("imageInfoLabel");
    imageInfoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    statusLayout->addWidget(spinner);
    statusLayout->addWidget(operationStatusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(imageInfoLabel);

    setStyleSheet(
        "#imageStatusPanel { background-color: #252526; border-top: 1px solid #333333; }"
        "#operationStatusLabel, #imageInfoLabel { color: #b8b8b8; font-size: 11px; }");
    spinner->setSpinning(false);
}

void StatusMenu::setOperationStatus(const QString &text, bool running)
{
    operationStatusLabel->setText(text);
    spinner->setSpinning(running);
}

void StatusMenu::setImageInfo(const Image *image)
{
    if (!image || !image->imageData) {
        imageInfoLabel->setText("No image");
        return;
    }

    const qint64 memorySize = static_cast<qint64>(image->width)
                              * image->height * image->channels;
    const QString colorMode = image->channels == 1 ? "Grayscale (8-bit)" : "RGB (24-bit)";
    imageInfoLabel->setText(
        QString("%1  |  %2 × %3 px  |  %4 (decoded)")
            .arg(colorMode)
            .arg(image->width)
            .arg(image->height)
            .arg(imageSizeText(memorySize)));
}
