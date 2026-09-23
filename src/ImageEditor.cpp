#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "ImageFilter.h"
#include "ImageEditor.h"
#include <string>




ImageEditor::ImageEditor(const std::string& imageFilePath) {

}

bool ImageEditor::loadImageFromFile(const std::string& imageFilePath) {

}

bool ImageEditor::saveToSameImage() {

}

void ImageEditor::saveState() {
    changes.push_back(currentImage);

    while(!changesUndone.empty()) {
        changesUndone.pop();
    }
}

bool ImageEditor::saveNewCopy(const std::string& newFilePath) {

}

bool ImageEditor::isImageLoaded() {
    if (currentImage.imageData == nullptr) {
        throw std::runtime_error("ERROR! No image loaded.");
        return false;
    }
    return true;
}


void ImageEditor::reset() {
    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
        return;
    }

    if(changes.empty()) {
        throw std::runtime_error("No changes to reset.");
        return;
    }

    changes.push_back(currentImage);
    currentImage = originalImage;
    changes.clear();
    while(!changesUndone.empty()) { 
        changesUndone.pop();
    }
    return;
}



void ImageEditor::undo() {
    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
        return;
    }

    if(changes.empty()) {
        throw std::runtime_error("No changes to undo.");
        return;

    }
    changesUndone.push(currentImage);
    currentImage = changes.back();
    changes.pop_back();
    std::cout << "Undo successful." << std::endl;
}

void ImageEditor::redo() {

    if(!isImageLoaded()) {
        throw std::runtime_error("ERROR! No image loaded.");
        return;
    }

    if(changesUndone.empty()) {
        throw std::runtime_error("No changes to redo.");
        return;
    }
    
    changes.push_back(currentImage);
    currentImage = changesUndone.top();
    changesUndone.pop();
    std::cout << "Redo successful." << std::endl;
}

void ImageEditor::applyFilter(FilterType filterType) {
    saveState();
}

