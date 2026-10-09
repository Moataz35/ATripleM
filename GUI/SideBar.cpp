#include "SideBar.h"
#include "ui_SideBar.h"

SideBar::SideBar(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SideBar)
{
    ui->setupUi(this);

    editor = nullptr;

    ui->stackedWidget->setVisible(false);

    setupFilterConnections();
    setupAdjustConnections();
    setupRotateConnections();
    setupFlipConnections();
    setupFrameConnections();
    setupResizeConnections();

    connect(ui->btnLoad, &QPushButton::clicked, this, &SideBar::onLoadImageClicked);
}


bool SideBar::hasImage() {
    if (editor == nullptr) {
        qDebug() << "No Image Loaded!";
        return false;
    }
    return true;
}

void SideBar::updatePreview() {
    if (!hasImage()) return;
    std::string tempPath = "temp_preview.png";
    editor->saveNewCopy(tempPath);

    QPixmap pixmap(QString::fromStdString(tempPath));
    ui->imageLable->setPixmap(pixmap.scaled(
        ui->imageLable->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
}

void SideBar::openMenu(int pageIndex){
    if(ui->stackedWidget->isVisible() && ui->stackedWidget->currentIndex()==pageIndex){
        ui->stackedWidget->setVisible(false);

    }
    else{
        ui->stackedWidget->setCurrentIndex(pageIndex);
        ui->stackedWidget->setVisible(true);
    }
}

void SideBar::setupFilterConnections(){
    connect(ui->btnFilters, &QPushButton::clicked, this, [=](){
        if(!hasImage()) return;
    openMenu(0);
    });
    connect(ui->GrayScale, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Grayscale);});
    connect(ui->BW, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::BlackAndWhite);});
    connect(ui->Invert, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::InvertImage);});
    connect(ui->Darken, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Darken);});
    connect(ui->Lighten, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Lighten);});
    connect(ui->Blur, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Blur);});
    connect(ui->PurpleEffect, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::PurpleEffect);});
    connect(ui->Sunlight, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::SunLight);});
    connect(ui->Infrared, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Infrared);});
    connect(ui->OldTV, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::OldTV);});
    connect(ui->Oil, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Oil);});
    connect(ui->DetectEdge, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::DetectEdges);});

}

void SideBar::setupFlipConnections(){
     connect(ui->btnFlip, &QPushButton::clicked, this, [=](){
        if(!hasImage()) return;
    openMenu(3);
    });
    connect(ui->FlipH, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::FlipHorizontally);});
    connect(ui->FlipV, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::FlipVertically);});

}


void SideBar::setupAdjustConnections(){
    connect(ui->btnAdjust, &QPushButton::clicked, this, [=](){
        if(!hasImage()) return;
        editor->saveNewCopy("original_adjust.png");

        ui->BrightnessSlider->setValue(0);
        ui->ContrastSlider->setValue(0);
        ui->SaturationSlider->setValue(0);

        ui->Brightness->setText("Brightness: 0");
        ui->Contrast->setText("Contrast: 0");
        ui->Saturation->setText("Saturation: 0");


        openMenu(1);
    });
    connect(ui->BrightnessSlider, &QSlider::valueChanged, this, [=](int val) {
        ui->Brightness->setText(QString("Brightness: %1").arg(val));
    });
    connect(ui->ContrastSlider, &QSlider::valueChanged, this, [=](int val) {
        ui->Contrast->setText(QString("Contrast: %1").arg(val));
    });
    connect(ui->SaturationSlider, &QSlider::valueChanged, this, [=](int val) {
        ui->Saturation->setText(QString("Saturation: %1").arg(val));
    });

    connect(ui->BrightnessSlider, &QSlider::sliderReleased, this, &SideBar::previewAdjustments);
    connect(ui->ContrastSlider,   &QSlider::sliderReleased, this, &SideBar::previewAdjustments);
    connect(ui->SaturationSlider, &QSlider::sliderReleased, this, &SideBar::previewAdjustments);


    connect(ui->confirmAdjust, &QDialogButtonBox::accepted, this, [=](){
        if(hasImage()){
            delete editor;
            editor = new ImageEditor("original_adjust.png");
            updatePreview();
        }
        ui->stackedWidget->setVisible(false);
    });
    connect(ui->confirmAdjust, &QDialogButtonBox::rejected, this, [=](){
        if(hasImage()){
            delete editor;
            editor = new ImageEditor("original_adjust.png");
            updatePreview();
        }
        ui->stackedWidget->setVisible(false);
    });

}
void SideBar::setupRotateConnections(){
    connect(ui->btnRotate, &QPushButton::clicked, this, [=](){
        if(!hasImage()) return;
        openMenu(2);
    });
    connect(ui->Rotate90, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Rotate90);});
    connect(ui->Rotate180, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Rotate180);});
    connect(ui->Rotate270, &QPushButton::clicked, this, [=](){applySpecificFilter(FilterType::Rotate270);});
}


void SideBar::setupFrameConnections(){
    connect(ui->AddFrame, &QPushButton::clicked, this, [=]() {
        if(!hasImage()) return;
        openMenu(4);
    });
    connect(ui->btnPickColor, &QPushButton::clicked, this, &SideBar::onColorCLicked);
    connect(ui->ApplyFrame->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SideBar::onApplyFrameClicked);
    connect(ui->ApplyFrame, &QDialogButtonBox::rejected, this, [=](){
        ui->stackedWidget->setVisible(false);
    });
}


void SideBar::setupResizeConnections(){
    connect(ui->Resize, &QPushButton::clicked, this, [=](){
        if(!hasImage()) return;
    openMenu(5);
    });
    connect(ui->confirmResize->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SideBar::onResizeClicked);
    connect(ui->confirmResize, &QDialogButtonBox::rejected, this, [=](){
        ui->stackedWidget->setVisible(false);
    });
}

void SideBar::onLoadImageClicked() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select an Image",
        "",
        "Image Files (*.png *.jpg *.jpeg *.bmp)"
        );

    if (!filePath.isEmpty()) {
        if (editor != nullptr) {
            delete editor;
        }
        editor = new ImageEditor(filePath.toStdString());

        updatePreview();
    }
}



void SideBar::applySpecificFilter(FilterType type){
    if(!hasImage()) return;
    editor->applyFilter(type);
    updatePreview();
}





void SideBar::previewAdjustments() {
    if (!hasImage()) return;

    int brightness = ui->BrightnessSlider->value();
    int contrast = ui->ContrastSlider->value();
    int saturation = ui->SaturationSlider->value();

    try {
        delete editor;
        editor = new ImageEditor("original_adjust.png");


        Adjustments::Brightness(editor->getCurrentImage(), brightness);
        Adjustments::Contrast(editor->getCurrentImage(), contrast);
        Adjustments::Saturation(editor->getCurrentImage(), saturation);

        updatePreview();
    } catch (std::exception &e) {
        qDebug() << "Adjust Error: " << e.what();
    }
}



void SideBar::onColorCLicked(){
    QColor picked = QColorDialog::getColor(Qt::white, this, "Choose Frame color");
    if(picked.isValid()){
        frameColor.red = picked.red();
        frameColor.green = picked.green();
        frameColor.blue = picked.blue();
        ui->btnPickColor->setStyleSheet(QString(
            "background-color: %1; color: %2; border-radius: 4px; padding: 4px;"
        ).arg(picked.name()).arg(picked.lightness() > 128 ? "black" : "white"));
    }
}


void SideBar::onApplyFrameClicked()
{
    if(!hasImage()) return ;

    int width = ui->FrameWidth->value();
    bool isFancy = ui->checkFancy->isChecked();

    try{
        editor->addFrame(frameColor, width, isFancy);
        updatePreview();

        ui->stackedWidget->setVisible(false);
        qDebug() << "Frame Added Successfully!";

    }catch(std::exception &e){
        std::cout<<"Error! "<<e.what()<<std::endl;
    }
}


void SideBar::onResizeClicked(){

    if(!hasImage()) return;

    int width = ui->inputWidth->value();
    int hight = ui->inputHight->value();

    if(width <16 || hight <16) width =16, hight=16;

    ImageFilter::Resize(editor->getCurrentImage(), width, hight);
    updatePreview();
    ui->stackedWidget->setVisible(false);
}

SideBar::~SideBar()
{
    if(editor != nullptr) delete editor;
    delete ui;
}











