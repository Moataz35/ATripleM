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

	void BlackandWhite(Image& image);
	void Invert(Image& image);
	void DarkenandLighten(Image& image, int brightness);
	void Rotate90(Image& image);
	void Rotate180(Image& image);
	void Rotate270(Image& image);

	void grayscale(Image& image);
}
