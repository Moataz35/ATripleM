#pragma once
#include "Image.h"

enum FilterType {
    Grayscale = 1, BlackAndWhite , InvertImage , AddFrame , Flip , 
    Rotate , DarkenAndLighten , Resize, Merge , DetectEdges , Crop , Blur , SunLight,
    OldTV , PurpleEffect , Infrared , Skew , Oil
};


namespace Adjustments {
	void Brightness(Image& image, int brightness);

}
namespace ImageFilter {

	void blackandWhite(Image& image);
	void Invert(Image& image);
	void DarkenandLighten(Image& image, int brightness);
	void rotate90(Image& image);
	void rotate180(Image& image);
	void rotate270(Image& image);

	void grayscale(Image& image);
}
