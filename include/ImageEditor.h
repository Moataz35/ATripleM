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
    Image tempImage;
    std::deque<Image> changes;
    std::stack<Image> changesUndone;
    bool modified = false;
    bool canReachInitialState = true;
    std::string originalFilePath;
    Image adjustBase;          // image before the current adjust session started
    bool adjusting = false;    // true while sliders are being previewed
public:
    ImageEditor() = default;

    ImageEditor(const std::string& imageFilePath);

    int getWidth() const;
    int getHeight() const;
    
    std::string getOriginalImagePath();

    Image& getCurrentImage();

    bool loadImageFromFile(const std::string& imageFilePath);
    bool isImageLoaded();
    bool saveToSameImage();
    
    bool saveNewCopy(const std::string& newFilePath);
    
    void reset();
    
    void undo();
    bool canUndo() const{ return !changes.empty();};
    
    void redo();
    bool canRedo() const { return !changesUndone.empty();};

    void applyFilter(FilterType filterType);
    
    void applyAdjust(AdjustmentType adjustType, int value);

    // Non-destructive adjust: always computed from the image as it was
    // before the first slider move, never from the previous preview.
    void previewAdjust(int brightness, int contrast, int saturation);
    void commitAdjust();
    void cancelAdjust();

    void addFrame(Color frameColor, int frameThickness, bool fancy);

    void applyMergeByScaling(Image& overlayImage , int alpha);
    void applyMergeByScaling(const std::string& overlayPath, int alpha);

    void applyMergeByCommonArea(Image& overlayImage , int alpha , int offsetX , int offsetY);
    void applyMergeByCommonArea(const std::string& overlayPath, int alpha, int offsetX, int offsetY);

    void applyEdgeDetection(unsigned char threshold = 100);

    void crop(int startX, int startY, int newWidth, int newHeight);

    void applySkew(double skewDegree = 45.0);
    void previewSkew(double skewDegree);
    void commitSkew();
    void cancelSkew();
    
    bool isModified();
    
    void unloadImage();
    
private:
    void saveState();
    

};
