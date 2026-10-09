#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "QMessageBox"
#include "navbar.h"
#include "EditorDropArea.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) , editorDropArea(new EditorDropArea)
{
    // 1. Always setup Qt Designer UI first
    ui->setupUi(this);

    // 2. Create a fresh vertical container layout for centralwidget
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 3. Create Navbar and insert it AT THE TOP (index 0)
    Navbar *nav = new Navbar(this);
    mainLayout->addWidget(nav);

    mainLayout->addWidget(editorDropArea , 1);


    connect(nav , &Navbar::openFileRequested , this , &MainWindow::openFile);
    connect(nav , &Navbar::closeFileRequested , editorDropArea , &EditorDropArea::removeImage);

    connect(editorDropArea , &EditorDropArea::imageSelected , this , &MainWindow::loadFile);
    connect(editorDropArea , &EditorDropArea::imageRemoved , this , &MainWindow::removeImage);

    connect(nav , &Navbar::exitRequested , this , &MainWindow::exit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,                         // Parent widget
        "Select an Image",               // Dialog title
        "",                              // Starting directory (empty uses current default)
        "Image Files (*.png *.jpg *.bmp);;All Files (*)"
    );

    loadFile(filePath);


}




void MainWindow::loadFile(const QString filePath){
    try{
        editor.loadImageFromFile(filePath.toStdString());

        editorDropArea->setImage(editor.getCurrentImage());


    }
    catch(...){
        QMessageBox::warning(this , "error" , "Wrong path");
    }
}

void MainWindow::exit(){
    QMessageBox msgBox;
    msgBox.setText("exit");
    msgBox.setText("Are you sure do you want to exit?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int res = msgBox.exec();

    if(res == QMessageBox::Yes){
        this->close();
    }
}

void MainWindow::removeImage(){
    editor.unloadImage();

}