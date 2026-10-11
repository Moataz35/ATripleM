#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <exception>
#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QScrollArea>

#include "ImageFilter.h"

QT_BEGIN_NAMESPACE


namespace Ui {
class SideBar;
}
QT_END_NAMESPACE

class SideBar : public QWidget
{
    Q_OBJECT

    public:
        explicit SideBar(QWidget *parent = nullptr);
        ~SideBar() ;

        void setButtonsEnabled(bool hasImage);


    signals:
        void filterRequested(FilterType type);
        void adjustRequested(int brightness, int contrast, int saturation);
        void adjustApplied();
        void adjustCancelled();
        void frameRequested(int width, bool isFancy, QColor color);
        void resizeRequested(int width, int height);
        void cropRequested();
        void cropConfirmed();
        void cropCancelled();
        void skewRequested(int degree);
        void skewApplied();
        void skewCancelled();
        void mergeByScalingRequested(const QString& overlayPath, int alpha);
        void mergeByCommonAreaRequested(const QString& overlayPath, int alpha, int offsetX, int offsetY);


    private slots:
        void on_btnFilters_clicked();
        void on_btnAdjust_clicked();
        void on_btnRotate_clicked();
        void on_btnFlip_clicked();
        void on_AddFrame_clicked();
        void on_Resize_clicked();
        void on_btnPickColor_clicked();
        void on_GrayScale_clicked();
        void on_DetectEdge_clicked();
        void on_BW_clicked();
        void on_Invert_clicked();
        void on_Infrared_clicked();
        void on_Darken_clicked();
        void on_Sunlight_clicked();
        void on_Lighten_clicked();
        void on_Blur_clicked();
        void on_OldTV_clicked();
        void on_Oil_clicked();
        void on_PurpleEffect_clicked();
        void on_FlipV_clicked();
        void on_FlipH_clicked();
        void on_Rotate90_clicked();
        void on_Rotate180_clicked();
        void on_Rotate270_clicked();
        void on_btnSkew_clicked();
        // Merge ===
        void on_btnMerge_clicked();
        void on_btnMergeCommanArea_clicked();
        void on_btnBrowseMerge_clicked();
        void on_btnBrowseMergeCommon_clicked();
        void on_confirmMerge_clicked(QAbstractButton *button);
        void on_confirmMergeCommanArea_clicked(QAbstractButton *button);
        void on_alphaSlider_valueChanged(int value);
        void on_alphaSliderCommanArea_valueChanged(int value);
        //==================================
        void on_skewSlider_valueChanged(int degree);
        void on_confirmSkew_clicked(QAbstractButton *button);

        void on_BrightnessSlider_valueChanged(int value);
        void on_ContrastSlider_valueChanged(int value);
        void on_SaturationSlider_valueChanged(int value);

        void on_confirmAdjust_clicked(QAbstractButton *button);
        void on_ApplyFrame_clicked(QAbstractButton *button);
        void on_confirmResize_clicked(QAbstractButton *button);

        void on_Crop_clicked();
        void on_ConfirmCrop_clicked(QAbstractButton *button);


    protected:
        void resizeEvent(QResizeEvent *event) override;

    private:
        void setSidebarLocked(bool locked);
        void setupScrollableSidebar();
        Ui::SideBar *ui;
        QScrollArea *sideBarScrollArea;
        void resetAdjustSliders();
        void resetMergeInputs();
        QColor currentFrameColor = Qt::white;
        void resetSkewSlider();
};

#endif // SIDEBAR_H
