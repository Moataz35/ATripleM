#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "ImageEditor.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

    private slots:
        void applySpecificFilter(FilterType type);
        void onLoadImageClicked();

    private:
        Ui::MainWindow *ui;
        ImageEditor *editor;
        QString currentFilePath;

};
#endif // MAINWINDOW_H
