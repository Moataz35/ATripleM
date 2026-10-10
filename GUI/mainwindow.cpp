#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "QMessageBox"
#include "navbar.h"
#include "EditorDropArea.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , editor(new ImageEditor)
    , editorDropArea(new EditorDropArea)
    , navBar(nullptr)
    , sideBar(nullptr)

{
    // 1. Initialize the MainWindow UI
    ui->setupUi(this);


    // 2. Create the components
    navBar = new Navbar(ui->centralwidget);
    sideBar = new SideBar(ui->centralwidget);
    sideBar->setMinimumWidth(180);


    // 3. Main vertical layout
    QVBoxLayout *mainLayout =
        new QVBoxLayout(ui->centralwidget);

    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 4. Content layout: sidebar + editor
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(50);

    contentLayout->addWidget(sideBar);
    contentLayout->addWidget(editorDropArea, 1);


    // 5. Assemble the main layout
    mainLayout->addWidget(navBar);
    mainLayout->addLayout(contentLayout, 1);




    connect(navBar, &Navbar::openFileRequested,
            this, &MainWindow::openFile);

    connect(navBar , &Navbar::saveFileRequested,
            this , &MainWindow::saveFile);

    connect(navBar, &Navbar::closeFileRequested,
            editorDropArea, &EditorDropArea::removeImage);

    connect(navBar, &Navbar::exitRequested,
            this, &MainWindow::exit);

    // 7. Connect editor signals
    connect(editorDropArea, &EditorDropArea::imageSelected,
            this, &MainWindow::loadFile);

    connect(editorDropArea, &EditorDropArea::imageRemoved,
            this, &MainWindow::removeImage);

    connect(sideBar , &SideBar::filterRequested , this , &MainWindow::applyFilter);

    connect(sideBar, &SideBar::cropRequested, editorDropArea, &EditorDropArea::showCropBox);

    connect(sideBar , &SideBar::frameRequested , this , &MainWindow::applyFrame);
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
        editor->loadImageFromFile(filePath.toStdString());

        editorDropArea->setImage(editor->getCurrentImage());


    }
    catch(...){
        QMessageBox::warning(this , "error" , "Wrong path");
    }
}

void MainWindow::saveFile(){
    try{
        editor->saveToSameImage();

        QMessageBox::information(this , "error" , "Wrong path");
    }
    catch(...){
        QMessageBox::warning(this , "error" , "No image to save");
    }

}

void MainWindow::applyFilter(FilterType type){
    editor->applyFilter(type);
    editorDropArea->setImage(editor->getCurrentImage());
}

void MainWindow::applyAdjust(int brightness, int contrast, int saturation){
    editor->applyAdjust(AdjustmentType::Brightness , brightness);
    editor->applyAdjust(AdjustmentType::Contrast , contrast);
    editor->applyAdjust(AdjustmentType::Saturation , saturation);

    editorDropArea->setImage(editor->getCurrentImage());

}

void MainWindow::on_pushButton_clicked() {

}

void MainWindow::applyFrame(int width, bool isFancy, QColor color){
    unsigned char r = static_cast<unsigned char>(color.red());
    unsigned char g = static_cast<unsigned char>(color.green());
    unsigned char b = static_cast<unsigned char>(color.blue());
    Color frameColor = {r, g, b};
    editor->addFrame(frameColor, width, isFancy);
    editorDropArea->setImage(editor->getCurrentImage());
}

void MainWindow::crop(QRect toCrop) {
    editor->crop(toCrop.x(), toCrop.y(), toCrop.width(), toCrop.height());
    editorDropArea->setImage(editor->getCurrentImage());
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
    editor->unloadImage();

}
