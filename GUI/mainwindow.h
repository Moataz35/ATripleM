#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ImageEditor.h"
#include "EditorDropArea.h"
#include "SideBar.h"
#include "navbar.h"
#include "ImageFilter.h"
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QThread;
class StatusMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void applyFilter(FilterType type);
    void applyAdjust(int brightness, int contrast, int saturation);
    void commitAdjust();
    void cancelAdjust();
    void applyFrame(int width, bool isFancy, QColor color);
    void applySkew(double degree);
    void crop(QRect toCrop);
    void applyMergeByScaling(const QString& overlayPath, int alpha);
    void applyMergeByCommonArea(const QString& overlayPath, int alpha, int offsetX, int offsetY);
    void undo();
    void redo();
    void reset();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;
    ImageEditor *editor;
    EditorDropArea *editorDropArea;
    Navbar *navBar;
    SideBar *sideBar;
    StatusMenu *statusMenu;


    QImage imageToQImage(Image& image);
    void loadFile(const QString filePath);
    void openFile();
    void saveFile();
    void saveNewFile();
    void removeImage();
    void updateNavBarButtons();
    void updateImageInfo();
    void runImageOperation(const QString &statusText,
                           const QString &finishedText,
                           std::function<void()> operation);
    void setImageOperationRunning(bool running);

    QImage preview;
    QThread *imageOperationThread;

};
#endif // MAINWINDOW_H
