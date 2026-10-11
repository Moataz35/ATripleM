#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>

class QToolButton;
class QMenu;
class QAction;

class Navbar : public QWidget
{
    Q_OBJECT

public:
    explicit Navbar(QWidget *parent = nullptr);
    void setButtonEnabled(bool canUndo, bool canRedo, bool hasImage);

signals:
    void openFileRequested();
    void saveFileRequested();
    void saveNewFileRequested();
    void closeFileRequested();
    void exitRequested();
    void undoRequested();
    void redoRequested();
    void resetRequested();

private:
    QToolButton *createNavButton(const QString &text);

    QMenu *createFileMenu();
    QMenu *createEditMenu();

    QAction *undoAction;
    QAction *redoAction;
    QAction *resetAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeAction;

};

#endif // NAVBAR_H
