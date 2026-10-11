#ifndef STATUSMENU_H
#define STATUSMENU_H

#include <QWidget>
#include <QString>

class QLabel;
class QComboBox;
class SpinnerWidget;
class Image;

class StatusMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMenu(QWidget *parent = nullptr);

    void setOperationStatus(const QString &text, bool running);
    void setImageInfo(const Image *image);
    void setZoomPercent(int percent);

signals:
    void zoomRequested(int percent);

private:
    SpinnerWidget *spinner;
    QLabel *operationStatusLabel;
    QLabel *imageInfoLabel;
    QLabel *zoomLabel;
    QComboBox *zoomCombo;
    int currentZoomPercent = 100;
};

#endif // STATUSMENU_H
