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

bool ImageEditor::saveToSameImage() {
    currentImage.saveImage(originalFilePath);
    modified = false;
    return true;
}

void ImageEditor::saveState() {
    if (changes.size() == 5) {
        changes.pop_front();
    }

    changes.push_back(currentImage);
    
    while(!changesUndone.empty()) changesUndone.pop();
}

bool ImageEditor::saveNewCopy(const std::string& newFilePath) {
    currentImage.saveImage(newFilePath);
    modified = false;
    return true;
}

bool ImageEditor::isImageLoaded() {
    if (currentImage.imageData == nullptr) {
        return false;
    }
    return true;
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

        default:
            throw std::invalid_argument("Unsupported Filter");
        
        }
        modified = true;
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
    modified = true;
}

void ImageEditor::addFrame(Color frameColor, int frameThickness, bool fancy) {
    saveState();
    ImageFilter::AddFrame(currentImage, frameColor, frameThickness, fancy);
}

void ImageEditor::unloadImage() {
    if (!isImageLoaded()) {
        return;
    }

    originalImage = Image();
    currentImage = Image();
    changes.clear();
    while (!changesUndone.empty()) {
        changesUndone.pop();
    }
    modified = false;
    originalFilePath = "";
}

void ImageEditor::applyMergeByScaling(Image& overlayImage , int alpha){
    saveState();
    ImageFilter::MergeByScaling(currentImage , overlayImage , alpha);
}

void ImageEditor::applyMergeByCommonArea(Image& overlayImage , int alpha , int offsetX , int offsetY){
    saveState();
    ImageFilter::MergeByCommonArea(currentImage , overlayImage , alpha , offsetX , offsetY);
}