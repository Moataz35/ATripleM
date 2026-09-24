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
    originalImage.loadNewImage(imageFilePath);
    originalFilePath = imageFilePath;
    return true;
}

bool ImageEditor::saveToSameImage() {
    currentImage.saveImage(originalFilePath);
    return true;
}

void ImageEditor::saveState() {
    changes.push_back(currentImage);

    while(!changesUndone.empty()) {
        changesUndone.pop();
    }
}

bool ImageEditor::saveNewCopy(const std::string& newFilePath) {
    currentImage.saveImage(newFilePath);
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

    if(changes.empty()) {
        throw std::runtime_error("No changes to reset.");
    }

    changes.push_back(currentImage);
    currentImage = originalImage;
    changes.clear();
    while(!changesUndone.empty()) { 
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

void ImageEditor::applyFilter(FilterType filterType) {
    changes.push_back(currentImage);

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

        default:
            throw std::invalid_argument("Unsupported Filter");
    }
}
