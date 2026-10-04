#pragma once
#include "Image.h"

struct Color {
	unsigned char red, green, blue;
	bool operator==(const Color& other) {
        return (red == other.red && green == other.green && blue == other.blue);
    }
};

const Color BLACK = {0, 0, 0};
const Color WHITE = {255, 255, 255};
const Color RED = {255, 0, 0};
const Color GREEN = {0, 255, 0};
const Color BLUE = {0, 0, 255};

enum FilterType {
    Grayscale = 1, BlackAndWhite, InvertImage, AddFrame, FlipVertically , FlipHorizontally ,
    Rotate90, Rotate180, Rotate270, Darken, Lighten, Resize, MergeByScaling , MergeByCommonArea, DetectEdges, Crop, Blur, SunLight,
    OldTV, PurpleEffect, Infrared, Skew, Oil
};

enum AdjustmentType{
	Brightness =1, Contrast, Saturation , BackToMain
};

namespace Adjustments {
	void Brightness(Image& image, int brightness);
	void Contrast(Image& image, int contrast);
	void Saturation(Image& image, int saturation);
}
namespace ImageFilter {

	void BlackandWhite(Image& image);
	void Invert(Image& image);
	void DarkenandLighten(Image& image, int brightness);
	void Rotate90(Image& image);
	void Rotate180(Image& image);
	void Rotate270(Image& image);
	void grayscale(Image& image);
	void AddFrame(Image& image, Color frameColor, int Framewidth, bool Fancy);
	void FlipVertically(Image& image);
	void FlipHorizontally(Image& image);
	void Resize(Image& image, int newWidth , int newHeight);
	void Blur(Image& image, int level) ; 
	void Crop(Image& image, int x, int y, int newWidth, int newHight);
	void EdgeDetection(Image& image, unsigned char threshold = 100);
	void MergeByScaling(Image& baseImage , Image& overlayImage , int alpha);
	void MergeByCommonArea(Image& baseImage , Image& overlayImage , int alpha , int offsetX , int offsetY);
	void Infrared(Image& image);
}
