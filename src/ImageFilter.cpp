#include "Image.h"
#include "ImageFilter.h"
#include <algorithm>




namespace Adjustments {


    void Brightness(Image& image, int brightness) {
        for (int w = 0; w < image.width; w++) {
            for (int h = 0; h < image.height; h++) {
                for (int c = 0; c < image.channels; c++) {
                    unsigned char currentPixel = image.getPixel(w, h, c);
                    int newPixelValue = static_cast<int>(currentPixel) + brightness;
                    newPixelValue = std::max(0, std::min(255, newPixelValue));
                    image.setPixel(w, h, c, static_cast<unsigned char>(newPixelValue));
                }
            }
        }
    }


    void Contrast(Image& image, float contrast) {
        float factor = (259 * (contrast + 255)) / (255 * (259 - contrast));
        for (int w = 0; w < image.width; w++) {
            for (int h = 0; h < image.height; h++) {
                for (int c = 0; c < image.channels; c++) {
                    unsigned char currentPixel = image.getPixel(w, h, c);
                    int newPixelValue = static_cast<int>(factor * (currentPixel - 128) + 128);
                    newPixelValue = std::max(0, std::min(255, newPixelValue));
                    image.setPixel(w, h, c, static_cast<unsigned char>(newPixelValue));
                }
            }
        }
    }

}

namespace ImageFilter {

	void blackandWhite(Image& image) {
        
    }

    void Invert(Image& image) {
        for (int w=0; w<image.width; w++) {
            for (int h=0; h<image.height; h++) {
                for(int c=0; c<image.channels; c++) {
                    unsigned char currentPixel = image.getPixel(w, h, c);
                    image.setPixel(w, h, c, 255 - currentPixel);
                }
            }
        }
    }

    void DarkenandLighten(Image& image, int brightness) {
        Adjustments::Brightness(image, brightness);
    }
	void rotate90(Image& image) {
        
    }
	void rotate180(Image& image) {
        
    }
	void rotate270(Image& image) {
        
    }
}
