#include "SideBar.h"
#include "ui_SideBar.h"


SideBar::SideBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SideBar)
{
    ui->setupUi(this);
    ui->stackedWidget->setVisible(false);
    setButtonsEnabled(false);
}

SideBar::~SideBar()
{
    delete ui;
}

void SideBar::setButtonsEnabled(bool hasImage)
{
    ui->btnFilters->setEnabled(hasImage);
    ui->btnAdjust->setEnabled(hasImage);
    ui->btnRotate->setEnabled(hasImage);
    ui->btnFlip->setEnabled(hasImage);
    ui->AddFrame->setEnabled(hasImage);
    ui->Resize->setEnabled(hasImage);
    ui->Crop->setEnabled(hasImage);
    ui->btnSkew->setEnabled(hasImage);
    if (!hasImage) {
        resetAdjustSliders();
        ui->stackedWidget->setVisible(false);
    }
}

//=================================================
//   Sidebar main Buttons actions
//=================================================

void SideBar::on_btnFilters_clicked(){
    if(ui->stackedWidget->currentIndex() == 0 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);

    }


}
void SideBar::on_btnAdjust_clicked(){
    if(ui->stackedWidget->currentIndex()==1 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(1);
    }
}
void SideBar::on_btnRotate_clicked()  {
    if(ui->stackedWidget->currentIndex()==2 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(2);
    }
}
void SideBar::on_btnFlip_clicked(){
    if(ui->stackedWidget->currentIndex()==3 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(3);
    }

}
void SideBar::on_AddFrame_clicked()   {
    if(ui->stackedWidget->currentIndex()==4 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(4);
    }
}

void SideBar::on_Resize_clicked()     {
    if(ui->stackedWidget->currentIndex()==5 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(5);
    }
}


void SideBar::on_Crop_clicked() {
    if(ui->stackedWidget->currentIndex() == 6 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(6);
    }


    setSidebarLocked(true);

    emit cropRequested();
}


void SideBar::on_btnSkew_clicked(){
    if(ui->stackedWidget->currentIndex() == 7 && ui->stackedWidget->isVisible()){
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(7);
    }
}
//============================================================================================
// Send Signals
//==========================================================================================
void SideBar::on_GrayScale_clicked()    { emit filterRequested(FilterType::Grayscale); }
void SideBar::on_DetectEdge_clicked()   { emit filterRequested(FilterType::DetectEdges); }
void SideBar::on_BW_clicked()           { emit filterRequested(FilterType::BlackAndWhite); }
void SideBar::on_Invert_clicked()       { emit filterRequested(FilterType::InvertImage); }
void SideBar::on_Infrared_clicked()     { emit filterRequested(FilterType::Infrared); }
void SideBar::on_Darken_clicked()       { emit filterRequested(FilterType::Darken); }
void SideBar::on_Sunlight_clicked()     { emit filterRequested(FilterType::SunLight); }
void SideBar::on_Lighten_clicked()      { emit filterRequested(FilterType::Lighten); }
void SideBar::on_Blur_clicked()         { emit filterRequested(FilterType::Blur); }
void SideBar::on_OldTV_clicked()        { emit filterRequested(FilterType::OldTV); }
void SideBar::on_Oil_clicked()          { emit filterRequested(FilterType::Oil); }
void SideBar::on_PurpleEffect_clicked() { emit filterRequested(FilterType::PurpleEffect); }
void SideBar::on_FlipH_clicked()        { emit filterRequested(FilterType::FlipHorizontally);}
void SideBar::on_FlipV_clicked()        { emit filterRequested(FilterType::FlipVertically);}
void SideBar::on_Rotate90_clicked()     {emit filterRequested(FilterType::Rotate90);}
void SideBar::on_Rotate180_clicked()    {emit filterRequested(FilterType::Rotate180);}
void SideBar::on_Rotate270_clicked()    {emit filterRequested(FilterType::Rotate270);}

void SideBar::on_confirmAdjust_clicked(QAbstractButton *button) {
    if (ui->confirmAdjust->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        emit adjustApplied();
    }else{
        emit adjustCancelled();
    }
    resetAdjustSliders();
    ui->stackedWidget->setVisible(false);
}

//===============================================================================================
// Sliders
//============================================================================================
void SideBar::resetAdjustSliders() {
    // Block signals so resetting does not trigger a new preview.
    const QSignalBlocker b1(ui->BrightnessSlider);
    const QSignalBlocker b2(ui->ContrastSlider);
    const QSignalBlocker b3(ui->SaturationSlider);

    ui->BrightnessSlider->setValue(0);
    ui->ContrastSlider->setValue(0);
    ui->SaturationSlider->setValue(0);

    ui->Brightness->setText("Brightness: 0");
    ui->Contrast->setText("Contrast: 0");
    ui->Saturation->setText("Saturation: 0");
}
void SideBar::on_BrightnessSlider_valueChanged(int value) {
    ui->Brightness->setText(QString("Brightness: %1").arg(value));

    emit adjustRequested(value, ui->ContrastSlider->value(), ui->SaturationSlider->value());
}

void SideBar::on_ContrastSlider_valueChanged(int value) {
    ui->Contrast->setText(QString("Contrast: %1").arg(value));

    emit adjustRequested(ui->BrightnessSlider->value(), value, ui->SaturationSlider->value());
}

void SideBar::on_SaturationSlider_valueChanged(int value) {
    ui->Saturation->setText(QString("Saturation: %1").arg(value));

    emit adjustRequested(ui->BrightnessSlider->value(), ui->ContrastSlider->value(), value);
}

void SideBar::on_skewSlider_valueChanged(int degree){
    ui->SkewDegree->setText(QString("Skew: %1").arg(degree));

}

//=======================================================================================
// Confirm Buttons
//======================================================================================
void SideBar::on_confirmResize_clicked(QAbstractButton *button) {
    if (ui->confirmResize->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        int w = ui->inputWidth->value();
        int h = ui->inputHight->value();
        emit resizeRequested(w, h);
        ui->stackedWidget->setVisible(false);
    }
    else{
        ui->stackedWidget->setVisible(false);
    }

}

void SideBar::on_confirmSkew_clicked(QAbstractButton *button) {
    if (ui->confirmSkew->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        int degree = ui->skewSlider->value();
        emit skewRequested(degree);
        ui->stackedWidget->setVisible(false);
    } else {
        ui->stackedWidget->setVisible(false);
    }
}

void SideBar::on_ConfirmCrop_clicked(QAbstractButton *button) {

    setSidebarLocked(false);

    if (ui->ConfirmCrop->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        emit cropConfirmed();
        ui->stackedWidget->setVisible(false);
    } else {
        emit cropCancelled();
        ui->stackedWidget->setVisible(false);
    }
}

//===========================================================================



void SideBar::on_ApplyFrame_clicked(QAbstractButton *button) {
    if (ui->ApplyFrame->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        int width = ui->FrameWidth->value();
        bool isFancy = ui->checkFancy->isChecked();
        emit frameRequested(width, isFancy, currentFrameColor);
        ui->stackedWidget->setVisible(false);
    }else{
        ui->stackedWidget->setVisible(false);
    }
}





void SideBar::on_btnPickColor_clicked() {
    QColor color = QColorDialog::getColor(currentFrameColor, this, "Choose Frame Color");

    if(color.isValid()){
        currentFrameColor = color;
        QString style = QString("background-color: %1; color: white;").arg(color.name());
        ui->btnPickColor->setStyleSheet(style);
    }
}










void SideBar::setSidebarLocked(bool locked) {

    ui->btnFilters->setEnabled(!locked);
    ui->btnAdjust->setEnabled(!locked);
    ui->btnRotate->setEnabled(!locked);
    ui->btnFlip->setEnabled(!locked);
    ui->AddFrame->setEnabled(!locked);
    ui->Resize->setEnabled(!locked);
    ui->Crop->setEnabled(!locked);
    ui->Skew->setEnabled(!locked);
}
