#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "StatusMenu.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QThread>
#include <QVBoxLayout>
#include <QMetaObject>
#include <exception>
#include "QMessageBox"
#include "navbar.h"
#include "EditorDropArea.h"
// #include <QDebug>
#include <QtCore>
#include <filesystem>
#include <QFileInfo>

namespace {

QString filterName(FilterType type)
{
    switch (type) {
    case FilterType::Grayscale: return "Grayscale";
    case FilterType::BlackAndWhite: return "Black and White";
    case FilterType::InvertImage: return "Invert";
    case FilterType::FlipVertically: return "Vertical Flip";
    case FilterType::FlipHorizontally: return "Horizontal Flip";
    case FilterType::Rotate90: return "Rotate 90°";
    case FilterType::Rotate180: return "Rotate 180°";
    case FilterType::Rotate270: return "Rotate 270°";
    case FilterType::Darken: return "Darken";
    case FilterType::Lighten: return "Lighten";
    case FilterType::DetectEdges: return "Edge Detection";
    case FilterType::Blur: return "Blur";
    case FilterType::SunLight: return "Sunlight";
    case FilterType::OldTV: return "Old TV";
    case FilterType::PurpleEffect: return "Purple Effect";
    case FilterType::Infrared: return "Infrared";
    case FilterType::Oil: return "Oil Effect";
    default: return "Image Filter";
    }
}

}

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
    , statusMenu(nullptr)
    , imageOperationThread(nullptr)

{
    // 1. Initialize the MainWindow UI
    ui->setupUi(this);


    // 2. Create the components
    navBar = new Navbar(ui->centralwidget);
    sideBar = new SideBar(ui->centralwidget);
    sideBar->setMinimumWidth(90);


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

    // 6. Add the status panel below the editor content.
    statusMenu = new StatusMenu(ui->centralwidget);
    mainLayout->addWidget(statusMenu);


    //===========================================================
    //Buttons Connections
    //=========================================================
    connect(navBar, &Navbar::openFileRequested,
            this, &MainWindow::openFile);

    connect(navBar , &Navbar::saveFileRequested,
            this , &MainWindow::saveFile);

    connect(navBar, &Navbar::saveNewFileRequested,
            this, &MainWindow::saveNewFile);

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

    connect(sideBar, &SideBar::stackedMenuVisibilityChanged,
            sideBar, [this](bool visible) {
        sideBar->setMinimumWidth(visible ? 180 : 90);
    });

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
    updateImageInfo();
}


MainWindow::~MainWindow()
{
    if (imageOperationThread) {
        imageOperationThread->wait();
        delete imageOperationThread;
        imageOperationThread = nullptr;
    }
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
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tga);;All Files (*)"
    );

    loadFile(filePath);


}

void MainWindow::loadFile(const QString filePath){
    if (filePath.isEmpty()) {
        return;
    }

    runImageOperation("Loading image...", "Image loaded", [this, filePath]() {
        editor->loadImageFromFile(filePath.toStdString());
    });
}

void MainWindow::saveFile(){
    if (!editor->isImageLoaded()) {
        QMessageBox::warning(this, "Save Image", "No image to save.");
        return;
    }

    runImageOperation("Saving image...", "Image saved", [this]() {
        editor->saveToSameImage();
    });
}

void MainWindow::saveNewFile() {

    std::string originalImagePath = editor->getOriginalImagePath();
    std::filesystem::path originalPath(originalImagePath);

    std::filesystem::path dir = originalPath.parent_path();
    std::string stem = originalPath.stem().string();
    std::string ext = originalPath.extension().string();

    int counter = 1;
    QString candidatePath;

    do {
        std::string newFileName = stem + "(" + std::to_string(counter) + ")" + ext;
        std::filesystem::path fullPath = dir / newFileName;
        candidatePath = QString::fromStdString(fullPath.string());
        counter++;
    } while (QFileInfo::exists(candidatePath));

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Choose the new file path",
        candidatePath,
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tga)"
        );

    qDebug() << filePath << "\n";

    if (filePath.size() == 0) {
        return;
    }


    try {
        editor->saveNewCopy(filePath.toStdString());
    }
    catch (...) {
        QMessageBox::warning(this, "Fail", "Failed to save the image. Maybe the path is invalid or not accessible.");
    }
}


void MainWindow::undo(){
    runImageOperation("Undoing...", "Undo complete", [this]() {
        editor->undo();
    });
}

void MainWindow::redo(){
    runImageOperation("Redoing...", "Redo complete", [this]() {
        editor->redo();
    });
}

void MainWindow::reset(){
    runImageOperation("Resetting image...", "Image reset", [this]() {
        editor->reset();
    });
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
    const QString name = filterName(type);
    runImageOperation(QString("Applying %1...").arg(name),
                      QString("%1 applied").arg(name),
                      [this, type]() { editor->applyFilter(type); });
}

void MainWindow::applyFrame(int width, bool isFancy, QColor color){
    unsigned char r = static_cast<unsigned char>(color.red());
    unsigned char g = static_cast<unsigned char>(color.green());
    unsigned char b = static_cast<unsigned char>(color.blue());
    Color frameColor = {r, g, b};
    runImageOperation("Adding frame...", "Frame applied",
                      [this, frameColor, width, isFancy]() {
        editor->addFrame(frameColor, width, isFancy);
    });
}

void MainWindow::applySkew(double degree){
    runImageOperation("Applying skew...", "Skew applied",
                      [this, degree]() { editor->applySkew(degree); });
}


void MainWindow::crop(QRect toCrop) {
    runImageOperation("Cropping image...", "Crop applied",
                      [this, toCrop]() {
        editor->crop(toCrop.x(), toCrop.y(), toCrop.width(), toCrop.height());
    });
}

void MainWindow::applyMergeByScaling(const QString& overlayPath, int alpha){
    runImageOperation("Merging images...", "Images merged",
                      [this, overlayPath, alpha]() {
        editor->applyMergeByScaling(overlayPath.toStdString(), alpha);
    });
}

void MainWindow::applyMergeByCommonArea(const QString& overlayPath, int alpha, int offsetX, int offsetY){
    runImageOperation("Merging images...", "Images merged",
                      [this, overlayPath, alpha, offsetX, offsetY]() {
        editor->applyMergeByCommonArea(overlayPath.toStdString(), alpha, offsetX, offsetY);
    });
}
//=======================================================================
//Adjustments Functions
//======================================================================


void MainWindow::applyAdjust(int brightness, int contrast, int saturation){
    // Live preview only: computed from the image before the first slider move.
    runImageOperation("Applying adjustments...", "Adjustments previewed",
                      [this, brightness, contrast, saturation]() {
        editor->previewAdjust(brightness, contrast, saturation);
    });
}

void MainWindow::commitAdjust(){
    runImageOperation("Applying adjustments...", "Adjustments applied",
                      [this]() { editor->commitAdjust(); });
}

void MainWindow::cancelAdjust(){
    runImageOperation("Cancelling adjustments...", "Adjustments cancelled",
                      [this]() { editor->cancelAdjust(); });
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

void MainWindow::updateImageInfo()
{
    statusMenu->setImageInfo(editor->isImageLoaded() ? &editor->getCurrentImage() : nullptr);
}

void MainWindow::setImageOperationRunning(bool running)
{
    navBar->setEnabled(!running);
    sideBar->setEnabled(!running);
    editorDropArea->setEnabled(!running);
}

void MainWindow::runImageOperation(const QString &statusText,
                                   const QString &finishedText,
                                   std::function<void()> operation)
{
    if (imageOperationThread) {
        return;
    }

    statusMenu->setOperationStatus(statusText, true);
    setImageOperationRunning(true);

    imageOperationThread = QThread::create(
        [this, operation = std::move(operation), finishedText]() {
            QString errorMessage;
            try {
                operation();
            } catch (const std::exception &error) {
                errorMessage = QString::fromUtf8(error.what());
            } catch (...) {
                errorMessage = "An unknown error occurred while processing the image.";
            }

            QMetaObject::invokeMethod(
                this,
                [this, errorMessage, finishedText]() {
                    QThread *completedThread = imageOperationThread;
                    imageOperationThread = nullptr;
                    if (completedThread) {
                        completedThread->wait();
                        completedThread->deleteLater();
                    }

                    setImageOperationRunning(false);
                    const bool hasImage = editor->isImageLoaded();
                    if (hasImage) {
                        editorDropArea->setImage(editor->getCurrentImage());
                    } else if (!errorMessage.isEmpty()) {
                        editorDropArea->removeImage();
                    }
                    sideBar->setButtonsEnabled(hasImage);
                    updateNavBarButtons();
                    updateImageInfo();

                    if (errorMessage.isEmpty()) {
                        statusMenu->setOperationStatus(finishedText, false);
                    } else {
                        statusMenu->setOperationStatus("Operation failed", false);
                        QMessageBox::warning(this, "Image operation failed", errorMessage);
                    }
                },
                Qt::QueuedConnection);
        });
    imageOperationThread->start();
}
