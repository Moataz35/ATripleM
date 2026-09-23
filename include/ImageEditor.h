#pragma once
#include "Image.h"
#include "ImageFilter.h"
#include <deque>
#include <stack>
#include <string>
#include <stdexcept>

class ImageEditor {
private:
    Image originalImage;
    Image currentImage;
    std::deque<Image> changes;
    std::stack<Image> changesUndone;
    bool modified;

public:
    ImageEditor() = default;

    ImageEditor(const std::string& imageFilePath);

    bool loadImageFromFile(const std::string& imageFilePath);

    bool saveToSameImage();

    void saveState();

    bool saveNewCopy(const std::string& newFilePath);

    void reset();

    void undo();

    void redo();

    void applyFilter(FilterType filterType);

private:
    bool isImageLoaded();
};
