#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "QMessageBox"
#include "navbar.h"
#include "EditorDropArea.h"
// #include <QDebug>
#include <QtCore>


//======================================================
//Cnstructor & Destructor
//======================================================

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

    mainLayout->setContentsMargins(20, 20, 20, 20); //20,20,20,20
    mainLayout->setSpacing(15);//15

    // 4. Content layout: sidebar + editor
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0); // 0,0,0,0
    contentLayout->setSpacing(50);

    contentLayout->addWidget(sideBar);
    contentLayout->addWidget(editorDropArea, 1);


    // 5. Assemble the main layout
    mainLayout->addWidget(navBar);
    mainLayout->addLayout(contentLayout, 1);


    //===========================================================
    //Buttons Connections
    //=========================================================
    connect(navBar, &Navbar::openFileRequested,
            this, &MainWindow::openFile);

    connect(navBar , &Navbar::saveFileRequested,
            this , &MainWindow::saveFile);

    connect(navBar, &Navbar::closeFileRequested,
            editorDropArea, &EditorDropArea::removeImage);

    connect(navBar, &Navbar::exitRequested,
            this, [&]() {this->close();} );


    connect(navBar, &Navbar::undoRequested,
            this, &MainWindow::undo);
    connect(navBar, &Navbar::redoRequested,
            this, &MainWindow::redo);
    connect(navBar, &Navbar::resetRequested,
            this, &MainWindow::reset);

    // 7. Connect editor signals
    connect(editorDropArea, &EditorDropArea::imageSelected,
            this, &MainWindow::loadFile);

    connect(editorDropArea, &EditorDropArea::imageRemoved,
            this, &MainWindow::removeImage);

    connect(sideBar , &SideBar::filterRequested , this , &MainWindow::applyFilter);

    connect(sideBar , &SideBar::adjustRequested , this , &MainWindow::applyAdjust);

    connect(sideBar , &SideBar::adjustApplied , this , &MainWindow::commitAdjust);

    connect(sideBar , &SideBar::adjustCancelled , this , &MainWindow::cancelAdjust);

    connect(sideBar , &SideBar::frameRequested , this , &MainWindow::applyFrame);

    connect(editorDropArea, &EditorDropArea::cropRequested, this, &MainWindow::crop);

    connect(sideBar, &SideBar::cropRequested, editorDropArea, &EditorDropArea::showCropBox);

    connect(sideBar, &SideBar::cropConfirmed, editorDropArea, &EditorDropArea::cropConfirmed);

    connect(sideBar, &SideBar::cropCancelled, editorDropArea, &EditorDropArea::cropCancelled);
    connect(sideBar, &SideBar::skewRequested, this, &MainWindow::applySkew);
    connect(sideBar, &SideBar::mergeByScalingRequested, this, &MainWindow::applyMergeByScaling);
    connect(sideBar, &SideBar::mergeByCommonAreaRequested, this, &MainWindow::applyMergeByCommonArea);


    updateNavBarButtons();
}


MainWindow::~MainWindow()
{
    delete ui;
}
//=============================================================================================
//Main Operations
//======================================================================================
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

        sideBar->setButtonsEnabled(true);
        updateNavBarButtons();

    }
    catch(...){
        QMessageBox::warning(this , "error" , "Wrong path");
    }
}

void MainWindow::saveFile(){
    try{
        editor->saveToSameImage();
    }
    catch(...){
        QMessageBox::warning(this , "error" , "No image to save");
    }

}

void MainWindow::undo(){
    try{
        editor->undo();
        editorDropArea->setImage(editor->getCurrentImage());
        updateNavBarButtons();
    }catch(const std::runtime_error &e){
        QMessageBox::warning(this, "Error! ", e.what());
    }catch(const std::exception &e){
        QMessageBox::critical(this, "Error! ", e.what());
    }
}

void MainWindow::redo(){
    try{
        editor->redo();
        editorDropArea->setImage(editor->getCurrentImage());
        updateNavBarButtons();
    }catch(const std::runtime_error &e){
        QMessageBox::warning(this, "Error! ", e.what());
    }catch(const std::exception &e){
        QMessageBox::critical(this, "Error! ", e.what());
    }
}

void MainWindow::reset(){
    try{
        editor->reset();
        editorDropArea->setImage(editor->getCurrentImage());
        updateNavBarButtons();
    }catch(const std::runtime_error &e){
        QMessageBox::warning(this, "Error! ", e.what());
    }catch(const std::exception &e){
        QMessageBox::critical(this, "Error! ", e.what());
    }
}

void MainWindow::removeImage(){
    editor->unloadImage();

    sideBar->setButtonsEnabled(false);
    updateNavBarButtons();

}


//===================================================================================
// Apply Filters
//=====================================================================================
void MainWindow::applyFilter(FilterType type){
    editor->applyFilter(type);
    editorDropArea->setImage(editor->getCurrentImage());
    updateNavBarButtons();
}

void MainWindow::applyFrame(int width, bool isFancy, QColor color){
    unsigned char r = static_cast<unsigned char>(color.red());
    unsigned char g = static_cast<unsigned char>(color.green());
    unsigned char b = static_cast<unsigned char>(color.blue());
    Color frameColor = {r, g, b};
    editor->addFrame(frameColor, width, isFancy);
    editorDropArea->setImage(editor->getCurrentImage());
    updateNavBarButtons();
}

void MainWindow::applySkew(double degree){
    editor->applySkew(degree);
    editorDropArea->setImage(editor->getCurrentImage());
    updateNavBarButtons();
}


void MainWindow::crop(QRect toCrop) {
    editor->crop(toCrop.x(), toCrop.y(), toCrop.width(), toCrop.height());
    editorDropArea->setImage(editor->getCurrentImage());
    updateNavBarButtons();
}

void MainWindow::applyMergeByScaling(const QString& overlayPath, int alpha){
    try {
        editor->applyMergeByScaling(overlayPath.toStdString(), alpha);
        editorDropArea->setImage(editor->getCurrentImage());
        updateNavBarButtons();
    } catch(const std::runtime_error &e){
        QMessageBox::warning(this, "Error!", e.what());
    } catch(const std::exception &e){
        QMessageBox::critical(this, "Error!", e.what());
    }
}

void MainWindow::applyMergeByCommonArea(const QString& overlayPath, int alpha, int offsetX, int offsetY){
    try {
        editor->applyMergeByCommonArea(overlayPath.toStdString(), alpha, offsetX, offsetY);
        editorDropArea->setImage(editor->getCurrentImage());
        updateNavBarButtons();
    } catch(const std::runtime_error &e){
        QMessageBox::warning(this, "Error!", e.what());
    } catch(const std::exception &e){
        QMessageBox::critical(this, "Error!", e.what());
    }
}
//=======================================================================
//Adjustments Functions
//======================================================================


void MainWindow::applyAdjust(int brightness, int contrast, int saturation){
    // Live preview only: computed from the image before the first slider move.
    editor->previewAdjust(brightness, contrast, saturation);

    editorDropArea->setImage(editor->getCurrentImage());

}

void MainWindow::commitAdjust(){
    editor->commitAdjust();
    updateNavBarButtons();
}

void MainWindow::cancelAdjust(){
    editor->cancelAdjust();
    editorDropArea->setImage(editor->getCurrentImage());
}

//=======================================================================


void MainWindow::updateNavBarButtons(){
    if(!editor || !navBar){
        return;
    }
    bool hasImage = editor->isImageLoaded();
    bool canUndo = hasImage && editor->canUndo();
    bool canRedo = hasImage && editor->canRedo();

    //qDebug() << "hasImage:" << hasImage << " | canUndo:" << canUndo << " | canRedo:" << canRedo;
    navBar->setButtonEnabled(canUndo, canRedo, hasImage);

}


void MainWindow::closeEvent(QCloseEvent* event) {
    if (!editor->isModified()) {
        event->accept();
        return;
    }

    QMessageBox warningMessage;
    warningMessage.setText("Do you want to save the changes made to the current image?");
    warningMessage.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard | QMessageBox::Save);

    int buttonCode = warningMessage.exec();
    switch (buttonCode) {
    case QMessageBox::Cancel:
        event->ignore();
        return;
    case QMessageBox::Discard:
        event->accept();
        return;
    case QMessageBox::Save:
        this->saveFile();
        event->accept();
        return;
    default:
        return;
    }
}
