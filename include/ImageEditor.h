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
    bool modified = false;
    std::string originalFilePath;
public:
    ImageEditor() = default;

    ImageEditor(const std::string& imageFilePath);
    int getWidth() const;
    int getHeight() const;

    // Read-only access to the working image (used by the GUI for display/export).
    const Image& getImage() const;
    // Replaces the working image (used by the GUI history manager) and marks it modified.
    void setImage(const Image& image);
    // Path the image was originally loaded from.
    const std::string& getFilePath() const;

    bool loadImageFromFile(const std::string& imageFilePath);

    bool saveToSameImage();
    
    bool saveNewCopy(const std::string& newFilePath);
    
    void reset();
    
    void undo();
    
    void redo();
    
    void applyFilter(FilterType filterType);
    
    void applyAdjust(AdjustmentType adjustType, int value);

    void addFrame(Color frameColor, int frameThickness, bool fancy);

    void applyMergeByScaling(Image& overlayImage , int alpha);

    void applyMergeByCommonArea(Image& overlayImage , int alpha , int offsetX , int offsetY);

    void applyEdgeDetection(unsigned char threshold = 100);
    
    bool isModified();
    
    void unloadImage();
    
private:
    void saveState();
    
    bool isImageLoaded();
};
