#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    editor = nullptr;
    ui->FILTERSMENU->setVisible(false);
    connect(ui->FIlters, &QPushButton::clicked, this, [=](){
        ui->FILTERSMENU->setVisible(!ui->FILTERSMENU->isVisible());
    });

    ui->RotateMenu->setVisible(false);
    connect(ui->Rotate, &QPushButton::clicked, this, [=](){
        ui->RotateMenu->setVisible(!ui->RotateMenu->isVisible());
    });

    ui->FlipMenu->setVisible(false);
    connect(ui->Flip, &QPushButton::clicked, this, [=](){
        ui->FlipMenu->setVisible(!ui->FlipMenu->isVisible());
    });


    connect(ui->LoadBtn, &QPushButton::clicked, this, &MainWindow::onLoadImageClicked);




    connect(ui->GRAY, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Grayscale);
    });
    connect(ui->BW, &QPushButton::clicked, this, [=](){
        applySpecificFilter((FilterType::BlackAndWhite));
    });
    connect(ui->INVERT, &QPushButton::clicked, this, [=](){
        applySpecificFilter((FilterType::InvertImage));
    });

    connect(ui->Darken, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Darken);
    });
    connect(ui->Lighten, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Lighten);
    });
    connect(ui->Detectedge, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::DetectEdges);
    });
    connect(ui->Blur, &QPushButton::clicked, this, &MainWindow::applySpecificFilter(FilterType::Blur));
    connect(ui->OldTV, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::OldTV);
    });
    connect(ui->Sunlight, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::SunLight);
    });
    connect(ui->PuroleEffect, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::PurpleEffect);
    });
    connect(ui->Infrared, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Infrared);
    });
    connect(ui->Rotate90, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Rotate90);
    });
    connect(ui->Rotat180, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Rotate180);
    });
    connect(ui->Rotate270, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::Rotate270);
    });
    connect(ui->FlipH, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::FlipHorizontally);
    });
    connect(ui->FLIPV, &QPushButton::clicked, this, [=](){
        applySpecificFilter(FilterType::FlipVertically);
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::onLoadImageClicked(){
    QString filePath = QFileDialog::getOpenFileName(
        this, "Select an Image","", "Image Files (*.png *.jpg *.jpeg *.bmp)"
        );
    if(!filePath.isEmpty()){
        if(editor != nullptr){
            delete editor;
        }
        editor = new ImageEditor(filePath.toStdString());
        QPixmap pixmap(filePath);
        ui->imageLable->setPixmap(pixmap.scaled(ui->imageLable->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    }
}



void MainWindow::applySpecificFilter(FilterType type){
    if(editor == nullptr){
        qDebug() << "No Image Loadede!";
        return;
    }

    try{
        editor->applyFilter((type));
        std::string tempPath = "temp_preview.jpg";
        editor->saveNewCopy(tempPath);
        editor->saveNewCopy(tempPath);
        QPixmap updatePixmap(QString::fromStdString(tempPath));
        ui->imageLable->setPixmap(updatePixmap.scaled(ui->imageLable->width(), ui->imageLable->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        qDebug()<<"Filter Applied successfully!";

    }catch(std::exception &e){
        std::cout<<"Error! "<<e.what()<<std::endl;
    }

}











