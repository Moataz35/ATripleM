#include"Image.h"
#include"ImageFilter.h"
#include"ImageEditor.h"
#include <algorithm>


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

void ImageFilter::blackandWhite(Image& image) {
        
}

void ImageFilter::rotate90(Image& image) {
        
}

void ImageFilter::rotate180(Image& image) {
        
}

void ImageFilter::rotate270(Image& image) {
        
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