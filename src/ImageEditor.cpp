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

void ImageEditor::reset() {
    changes.push_back(currentImage);
    currentImage = originalImage;
    changes.clear();
    while(!changesUndone.empty()) { 
        changesUndone.pop();
    }
    std::cout << "Image reset to original state." << std::endl;

}

void ImageEditor::undo() {
    if(changes.empty()) {
        std::cerr << "No changes to undo." << std::endl;
        return;
    }
    changesUndone.push(currentImage);
    currentImage = changes.back();
    changes.pop_back();
    std::cout << "Undo successful." << std::endl;
}

void ImageEditor::redo() {
    if(changesUndone.empty()) {
        std::cerr << "No changes to redo." << std::endl;
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

bool ImageEditor::isImageLoaded() {
    return true;
}
