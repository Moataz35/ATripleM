#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include"Image.h"
#include"ImageFilter.h"
#include"ImageEditor.h"
#include <string>
#include <exception>

ImageEditor::ImageEditor(const std::string& imageFilePath) {
    loadImageFromFile(imageFilePath);
}

bool ImageEditor::loadImageFromFile(const std::string& imageFilePath) {
    unloadImage();
    originalImage.loadNewImage(imageFilePath);
    originalFilePath = imageFilePath;
    currentImage = originalImage;
    return true;
}

int ImageEditor::getWidth() const {
    return currentImage.width;
}

int ImageEditor::getHeight() const {
    return currentImage.height;
}

std::string ImageEditor::getOriginalImagePath() {
    return originalFilePath;
}

Image& ImageEditor::getCurrentImage(){
    return currentImage;
}

bool ImageEditor::saveToSameImage() {
    currentImage.saveImage(originalFilePath);
    modified = false;
    return true;
}

void ImageEditor::saveState() {
    if (adjusting) {
        commitAdjust();
    }

    if (changes.size() == 5) {
        changes.pop_front();
        canReachInitialState = false;
    }

    changes.push_back(currentImage);
    
    while(!changesUndone.empty()) changesUndone.pop();

    modified = true;
}

bool ImageEditor::saveNewCopy(const std::string& newFilePath) {
    currentImage.saveImage(newFilePath);
    modified = false;
    return true;
}

bool ImageEditor::isImageLoaded() {
    return currentImage.imageData != nullptr
        && currentImage.width > 0
        && currentImage.height > 0;
}


void ImageEditor::reset() {
    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }

    if(!changes.empty()){
        currentImage = originalImage;
    }

    changes.clear();
    while (!changesUndone.empty()) { 
        changesUndone.pop();
    }

    modified = false;
    canReachInitialState = true;
}



void ImageEditor::undo() {
    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }

    if(changes.empty()) {
        throw std::runtime_error("No changes to undo.");
    }
    changesUndone.push(currentImage);
    currentImage = changes.back();
    changes.pop_back();

    if (changes.empty() && canReachInitialState) {
        modified = false;
    }
}

void ImageEditor::redo() {

    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }

    if(changesUndone.empty()) {
        throw std::runtime_error("No changes to redo.");
    }

    changes.push_back(currentImage);
    currentImage = changesUndone.top();
    changesUndone.pop();

    modified = true;
}

bool ImageEditor::isModified(){
    return modified;
}

void ImageEditor::applyFilter(FilterType filterType) {
    saveState();

    switch (filterType) {
        case FilterType::Grayscale:
            ImageFilter::grayscale(currentImage);
            break;

        case FilterType::BlackAndWhite:
            ImageFilter::BlackandWhite(currentImage);
            break;

        case FilterType::InvertImage:
            ImageFilter::Invert(currentImage);
            break;

        case FilterType::Darken:
            ImageFilter::DarkenandLighten(currentImage, -50);
            break;

        case FilterType::Lighten:
            ImageFilter::DarkenandLighten(currentImage, 50);
            break;

        case FilterType::Rotate90:
            ImageFilter::Rotate90(currentImage);
            break;

        case FilterType::Rotate180:
            ImageFilter::Rotate180(currentImage);
            break;

        case FilterType::Rotate270:
            ImageFilter::Rotate270(currentImage);
            break;

        case FilterType::FlipVertically:
            ImageFilter::FlipVertically(currentImage);
            break;

        case FilterType::FlipHorizontally:
            ImageFilter::FlipHorizontally(currentImage);
            break;
            
        case FilterType::SunLight:
            ImageFilter::sunLightEffect(currentImage);
            break;

        case FilterType::OldTV:
            ImageFilter::OldTV(currentImage);
            break;

        case FilterType::DetectEdges:
            ImageFilter::EdgeDetection(currentImage, 30);
            break;
        case FilterType::PurpleEffect:
            ImageFilter::PurpleEffect(currentImage);
            break;
        case FilterType::Infrared:
            ImageFilter::Infrared(currentImage);
            break;
        case FilterType::Blur:
            ImageFilter::Blur(currentImage, 1);
            break;
        case FilterType::Oil:
            ImageFilter::Oil(currentImage, 3, 255);
            break;
        
        default:
            throw std::invalid_argument("Unsupported Filter");
        
        }
        
}
void ImageEditor::applyAdjust(AdjustmentType adjustType, int value) {
    saveState();

    switch (adjustType)
    {
        case AdjustmentType::Brightness:
            Adjustments::Brightness(currentImage, value);
            break;
        case AdjustmentType::Contrast:
            Adjustments::Contrast(currentImage, value);
            break;
        case AdjustmentType::Saturation:
            Adjustments::Saturation(currentImage, value);
            break;
        default:
            throw std::invalid_argument("Unsupported Feature");

            
    }
    
}

void ImageEditor::previewAdjust(int brightness, int contrast, int saturation) {
    if (!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }

    if (!adjusting) {
        adjustBase = currentImage;
        adjusting = true;
    }

    currentImage = adjustBase;

    if (brightness != 0) Adjustments::Brightness(currentImage, brightness);
    if (contrast   != 0) Adjustments::Contrast(currentImage, contrast);
    if (saturation != 0) Adjustments::Saturation(currentImage, saturation);
}

void ImageEditor::commitAdjust() {
    if (!adjusting) {
        return;
    }

    if (changes.size() == 5) {
        changes.pop_front();
    }
    changes.push_back(adjustBase);
    while (!changesUndone.empty()) changesUndone.pop();

    adjustBase = Image();
    adjusting = false;
}

void ImageEditor::cancelAdjust() {
    if (!adjusting) {
        return;
    }

    currentImage = adjustBase;
    adjustBase = Image();
    adjusting = false;
}

void ImageEditor::addFrame(Color frameColor, int frameThickness, bool fancy) {
    saveState();
    ImageFilter::AddFrame(currentImage, frameColor, frameThickness, fancy);
}

void ImageEditor::resizeImage(int width, int height) {
    if (!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image dimensions must be greater than zero.");
    }

    saveState();
    ImageFilter::Resize(currentImage, width, height);
}

void ImageEditor::unloadImage() {
    if (!isImageLoaded()) {
        return;
    }

    originalImage = Image();
    currentImage = Image();
    adjustBase = Image();
    adjusting = false;
    changes.clear();
    while (!changesUndone.empty()) {
        changesUndone.pop();
    }
    modified = false;
    canReachInitialState = true;
    originalFilePath = "";
}

void ImageEditor::applyMergeByScaling(Image& overlayImage , int alpha){
    saveState();
    ImageFilter::MergeByScaling(currentImage , overlayImage , alpha);
}

void ImageEditor::applyMergeByScaling(const std::string& overlayPath, int alpha){
    if (!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }
    Image overlay(overlayPath);
    applyMergeByScaling(overlay, alpha);
}

void ImageEditor::applyMergeByCommonArea(Image& overlayImage , int alpha , int offsetX , int offsetY){
    saveState();
    ImageFilter::MergeByCommonArea(currentImage , overlayImage , alpha , offsetX , offsetY);
}

void ImageEditor::applyMergeByCommonArea(const std::string& overlayPath, int alpha, int offsetX, int offsetY){
    if (!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
    }
    Image overlay(overlayPath);
    applyMergeByCommonArea(overlay, alpha, offsetX, offsetY);
}

void ImageEditor::applyEdgeDetection(unsigned char threshold) {
    saveState();
    ImageFilter::EdgeDetection(currentImage, threshold);
}

void ImageEditor::crop(int startX, int startY, int newWidth, int newHeight) {
    saveState();
    ImageFilter::Crop(currentImage, startX, startY, newWidth, newHeight);
}

void ImageEditor::applySkew(double skewDegree) {
    saveState();
    ImageFilter::Skew(currentImage, skewDegree);
}
