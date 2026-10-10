#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>

class QToolButton;
class QMenu;

class Navbar : public QWidget
{
    Q_OBJECT

public:
    explicit Navbar(QWidget *parent = nullptr);

signals:
    void openFileRequested();
    void saveFileRequested();
    void closeFileRequested();
    void exitRequested();


private:
    QToolButton *createNavButton(const QString &text);

    QMenu *createFileMenu();
};

#endif // NAVBAR_H