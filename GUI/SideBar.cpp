#include "SideBar.h"
#include "ui_SideBar.h"


SideBar::SideBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SideBar)
{
    ui->setupUi(this);
    ui->stackedWidget->setVisible(true);
}

SideBar::~SideBar()
{
    delete ui;
}


void SideBar::on_btnFilters_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setVisible(true);

}
void SideBar::on_btnAdjust_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
    ui->stackedWidget->setVisible(true);
}
void SideBar::on_btnRotate_clicked()  {
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget->setVisible(true);
}
void SideBar::on_btnFlip_clicked()    {
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget->setVisible(true);
}
void SideBar::on_AddFrame_clicked()   {
    ui->stackedWidget->setCurrentIndex(4);
    ui->stackedWidget->setVisible(true);
}
void SideBar::on_Resize_clicked()     {
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget->setVisible(true);
}

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
void SideBar::on_Rotate180_clicked()     {emit filterRequested(FilterType::Rotate180);}
void SideBar::on_Rotate270_clicked()     {emit filterRequested(FilterType::Rotate270);}



void SideBar::on_confirmAdjust_clicked(QAbstractButton *button) {
    if (ui->confirmAdjust->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        int b = ui->BrightnessSlider->value();
        int c = ui->ContrastSlider->value();
        int s = ui->SaturationSlider->value();
        emit adjustRequested(b, c, s);
        ui->stackedWidget->setVisible(false);
    }else{
        ui->stackedWidget->setVisible(false);
    }
}

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




void SideBar::on_btnPickColor_clicked() {
    QColor color = QColorDialog::getColor(currentFrameColor, this, "Choose Frame Color");

    if(color.isValid()){
        currentFrameColor = color;
        QString style = QString("background-color: %1; color: white;").arg(color.name());
        ui->btnPickColor->setStyleSheet(style);
    }
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













