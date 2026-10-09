#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ImageEditor.h"
#include "EditorDropArea.h"

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
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    ImageEditor editor;
    EditorDropArea *editorDropArea;

    QImage imageToQImage(Image& image);
    void loadFile(const QString filePath);
    void openFile();
    void removeImage();
    void exit();

    QImage preview;

};
#endif // MAINWINDOW_H
