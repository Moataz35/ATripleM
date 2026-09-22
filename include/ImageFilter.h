#pragma once
#include "Image.h"

enum FilterType {
    Grayscale, BlackandWhite, NegativeEffect, EdgeDetection, SunlightEffect, 
    FlipHorizontally, FlipVertically,
    Rotation90, Rotation180, Rotation270
};

namespace ImageFilter {

	void BlackandWhite(Image& image);

	void Rotate90(Image& image);
	void Rotate180(Image& image);
	void Rotate270(Image& image);
}
