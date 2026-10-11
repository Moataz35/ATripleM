#ifndef STATUSMENU_H
#define STATUSMENU_H

#include <QWidget>
#include <QString>

class QLabel;
class SpinnerWidget;
class Image;

class StatusMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StatusMenu(QWidget *parent = nullptr);

    void setOperationStatus(const QString &text, bool running);
    void setImageInfo(const Image *image);

private:
    SpinnerWidget *spinner;
    QLabel *operationStatusLabel;
    QLabel *imageInfoLabel;
};

#endif // STATUSMENU_H
