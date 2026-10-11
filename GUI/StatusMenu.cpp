#include "StatusMenu.h"

#include "Image.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QSignalBlocker>
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
    , zoomLabel(new QLabel("Zoom:", this))
    , zoomCombo(new QComboBox(this))
{
    setObjectName("imageStatusPanel");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(34);

    auto *statusLayout = new QHBoxLayout(this);
    statusLayout->setContentsMargins(10, 0, 10, 0);
    statusLayout->setSpacing(8);

    operationStatusLabel->setObjectName("operationStatusLabel");
    imageInfoLabel->setObjectName("imageInfoLabel");
    imageInfoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    zoomLabel->setObjectName("zoomLabel");
    zoomCombo->setObjectName("zoomCombo");
    zoomCombo->setEditable(true);
    zoomCombo->setInsertPolicy(QComboBox::NoInsert);
    zoomCombo->setFixedWidth(78);
    zoomCombo->addItems({"10%", "25%", "50%", "75%", "100%", "125%",
                         "150%", "200%", "300%", "400%"});
    zoomCombo->setCurrentText("100%");

    spinner->setObjectName("spinner");

    statusLayout->addWidget(spinner);
    statusLayout->addWidget(operationStatusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(zoomLabel);
    statusLayout->addWidget(zoomCombo);
    statusLayout->addWidget(imageInfoLabel);

    setStyleSheet(
        "#imageStatusPanel { background-color: #252526; border-top: 1px solid #333333; }"
        "#operationStatusLabel, #imageInfoLabel, #zoomLabel { color: #b8b8b8; font-size: 11px; background-color : transparent}"
        "QComboBox#zoomCombo { color: #dddddd; background-color: #2d2d2d; border: 1px solid #454545; border-radius: 3px; padding: 2px 4px; }"
        "QComboBox#zoomCombo QAbstractItemView { color: #dddddd; background-color: #252526; selection-background-color: #3a3a3a; }"
        "#spinner {background-color : transparent}");

    const auto applyZoomText = [this]() {
        QString text = zoomCombo->currentText().trimmed();
        if (text.endsWith('%')) {
            text.chop(1);
        }
        bool ok = false;
        const int percent = text.toInt(&ok);
        if (ok) {
            emit zoomRequested(percent);
        } else {
            setZoomPercent(currentZoomPercent);
        }
    };
    connect(zoomCombo, qOverload<int>(&QComboBox::activated),
            this, [applyZoomText](int) { applyZoomText(); });
    connect(zoomCombo->lineEdit(), &QLineEdit::editingFinished,
            this, applyZoomText);
    zoomCombo->setEnabled(false);
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
        zoomCombo->setEnabled(false);
        zoomLabel->setEnabled(false);
        return;
    }

    zoomCombo->setEnabled(true);
    zoomLabel->setEnabled(true);
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

void StatusMenu::setZoomPercent(int percent)
{
    currentZoomPercent = percent;
    const QSignalBlocker blocker(zoomCombo);
    const QString text = QString("%1%").arg(percent);
    int index = zoomCombo->findText(text);
    zoomCombo->setCurrentIndex(index);
    zoomCombo->setEditText(text);
}
