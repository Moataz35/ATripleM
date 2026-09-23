#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include"..\include\Image.h"
#include"..\include\ImageFilter.h"
#include"..\include\ImageEditor.h"
#include <string>

ImageEditor::ImageEditor(const std::string& imageFilePath) {

}

bool ImageEditor::loadImageFromFile(const std::string& imageFilePath) {
    return true;
}

bool ImageEditor::saveToSameImage() {
    return true;
}

bool ImageEditor::saveNewCopy(const std::string& newFilePath) {
    return true;
}

void ImageEditor::reset() {

}

void ImageEditor::undo() {

}

void ImageEditor::redo() {

}

void ImageEditor::applyFilter(FilterType filterType) {

}

bool ImageEditor::isImageLoaded() {
    return true;
}
