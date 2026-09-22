#include "Image.h"
#include "ImageFilter.h"



void ImageFilter::BlackandWhite(Image& image) {
    const unsigned char threshold = 128;

    for (int h = 0; h < image.height; h++) {
        for (int w = 0; w < image.width; w++) {
            unsigned char grayValue = 0;

            if (image.channels < 3) {
                grayValue = image.getPixel(w, h, 0);
            }
            else {
                unsigned char redIntensity = image.getPixel(w, h, 0);
                unsigned char greenIntensity = image.getPixel(w, h, 1);
                unsigned char blueIntensity = image.getPixel(w, h, 2);
                grayValue = (redIntensity + greenIntensity + blueIntensity) / 3;
            }

            unsigned char newPixelValue = grayValue < threshold ? 0 : 255;

            image.setPixel(w, h, 0, newPixelValue);
            if (image.channels > 2) {
                image.setPixel(w, h, 1, newPixelValue);
                image.setPixel(w, h, 2, newPixelValue);\
            }
        }
    }
}

void ImageFilter::Rotate90(Image& image) {
    
}

void ImageFilter::Rotate180(Image& image) {
    
}

void ImageFilter::Rotate270(Image& image) {
    
}
