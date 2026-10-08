#ifndef IMAGEOPS_H
#define IMAGEOPS_H

// NOTE: Qt headers must come before Image.h (see macro guards there).
#include <QString>
#include <functional>
#include <memory>

#include "ImageEditor.h"

/// Extra pixel operations that complement the ones in ImageFilter, plus
/// Unicode-safe file I/O helpers around ImageEditor / stb.
namespace ImageOps {

struct AdjustParams {
    int brightness = 0;  // -100..100
    int contrast = 0;    // -100..100
    int saturation = 0;  // -100..100
    int hue = 0;         // -180..180 degrees
    int blur = 0;        // 0..10 passes
    int sharpen = 0;     // 0..100
    bool isIdentity() const {
        return !brightness && !contrast && !saturation && !hue && !blur && !sharpen;
    }
};

void hueShift(Image &image, int degrees);
void sharpen(Image &image, int amount);
void oilPainting(Image &image, int radius = 3, int levels = 20);
/// Horizontal shear. The canvas grows to fit; empty areas are filled with `fill`.
void skew(Image &image, double degrees, Color fill);
/// Applies every non-zero adjustment in a fixed order using the ImageFilter/Adjustments API.
void applyAdjustments(Image &image, const AdjustParams &params);
/// Runs `op` on the rectangle (x,y,w,h) only; the result is pasted back if its size is unchanged.
void applyInRegion(Image &image, int x, int y, int w, int h, const std::function<void(Image &)> &op);

/// Writes png/jpg/bmp/tga chosen by file extension. Works with Unicode paths.
bool writeImage(const Image &image, const QString &path, int jpegQuality, QString *error);

/// Loads an image into a fresh ImageEditor (Unicode-safe). Returns nullptr and sets `error` on failure.
std::shared_ptr<ImageEditor> loadEditor(const QString &path, QString *error);
/// Saves through ImageEditor (saveToSameImage / saveNewCopy). Unicode-safe.
bool saveEditor(ImageEditor &editor, const QString &path, QString *error);

} // namespace ImageOps

#endif // IMAGEOPS_H
