#pragma once
#include "Image.h"

enum FilterType {
    Grayscale, BlackandWhite, NegativeEffect, EdgeDetection, SunlightEffect, 
    FlipHorizontally, FlipVertically,
    Rotation90, Rotation180, Rotation270
};

namespace ImageFilter {

	void blackandWhite(Image& image);

	void rotate90(Image& image);
	void rotate180(Image& image);
	void rotate270(Image& image);
}
