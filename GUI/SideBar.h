#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <exception>
#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>

#include "ImageEditor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SideBar;
}
QT_END_NAMESPACE

class SideBar : public QMainWindow
{
    Q_OBJECT

    public:
        explicit SideBar(QWidget *parent = nullptr);
        ~SideBar() override;

    private slots:
        void applySpecificFilter(FilterType type);
        void previewAdjustments();
        void onLoadImageClicked();
        void onColorCLicked();
        void onApplyFrameClicked();
        void onResizeClicked();

    private:
        Image baseAdjustImage;
        Ui::SideBar *ui;
        ImageEditor *editor;
        QString currentFilePath;
        Color frameColor;

        void openMenu(int pageIndex);
        void updatePreview();
        bool hasImage();
        void setupFilterConnections();
        void setupAdjustConnections();
        void setupFlipConnections();
        void setupRotateConnections();
        void setupFrameConnections();
        void setupResizeConnections();
};

#endif // SIDEBAR_H
