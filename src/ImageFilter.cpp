#include"..\include\Image.h"
#include"..\include\ImageFilter.h"
#include"..\include\ImageEditor.h"
#include <algorithm>


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
                image.setPixel(w, h, 2, newPixelValue);
            }
        }
    }
}


void Adjustments::Brightness(Image& image, int brightness) {

    brightness = std::max(-100, std::min(100, brightness));

    for (int w = 0; w < image.width; w++) {
        for (int h = 0; h < image.height; h++) {
            for (int c = 0; c < image.channels; c++) {

                if((image.channels == 4 || image.channels == 2) && (c == image.channels - 1)) {
                    continue; 
                }

                unsigned char currentPixel = image.getPixel(w, h, c);
                int newPixelValue = (static_cast<int>(currentPixel)) * (1.0+(brightness / 100.0));
                newPixelValue = std::max(0, std::min(255, newPixelValue));
                image.setPixel(w, h, c, static_cast<unsigned char>(newPixelValue));

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


void ImageFilter::Invert(Image& image) {
        for (int w=0; w<image.width; w++) {
            for (int h=0; h<image.height; h++) {
                for(int c=0; c<image.channels; c++) {

                    if((image.channels == 4 || image.channels == 2) && (c == image.channels - 1)) {
                    continue; 
                    }

                    unsigned char currentPixel = image.getPixel(w, h, c);
                    image.setPixel(w, h, c, 255 - currentPixel);
                }
            }
        }
}
    
void ImageFilter::DarkenandLighten(Image& image, int brightness) {
        Adjustments::Brightness(image, brightness);
}

void ImageFilter::grayscale(Image& image){

    for (int w = 0; w < image.width; w++) {
        for (int h = 0 ; h < image.height ; h++) {
            unsigned char &r = image.getPixel(w , h, 0);
            unsigned char &g = image.getPixel(w , h, 1);
            unsigned char &b = image.getPixel(w , h, 2);

            unsigned char gray = (r + g + b) /3;

            r = g = b = gray;
        }
    }
    
}
