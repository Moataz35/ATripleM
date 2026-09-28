#include"Image.h"
#include"ImageFilter.h"
#include"ImageEditor.h"
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

void Adjustments::Contrast(Image &image, int contrast){
    contrast = std::max(-100, std::min(100, contrast));
    double factor = ((259.0*(contrast +255.0)))/(255.0*(259.0-contrast));
    for (int w = 0; w < image.width; w++) {
        for (int h = 0; h < image.height; h++) {
            for (int c = 0; c < image.channels; c++) {

                if((image.channels == 4 || image.channels == 2) && (c == image.channels - 1)) {
                    continue; 
                }

                unsigned char currentPixel = image.getPixel(w, h, c);
                int newPixelValue = (factor * (static_cast<int> (currentPixel) - 128) + 128);
                newPixelValue = std::max(0, std::min(255, newPixelValue));
                image.setPixel(w, h, c, static_cast<unsigned char>(newPixelValue));

            }
        }
    }
}

void ImageFilter::Rotate90(Image& image) {
    Image newImage;
    newImage.width = image.height;
    newImage.height = image.width;
    newImage.channels = image.channels;
    newImage.imageData = new unsigned char[newImage.width * newImage.height * image.channels];

    int originaly = 0;
    for (int x = newImage.width - 1; x >= 0; x--) {

        int originalx = 0;
        for (int y = 0; y < newImage.height; y++) {

            for (int c = 0; c < newImage.channels; c++) {

                unsigned char currentPixelValue = image.getPixel(originalx, originaly, c);
                newImage.setPixel(x, y, c, currentPixelValue);
            }
            originalx++;
        }
        originaly++;
    }

    image = newImage;
}

void ImageFilter::Rotate180(Image& image) {
    Image newImage(image);

    int originaly = 0;
    for (int y = image.height - 1; y >= 0; y--) {

        int originalx = 0;
        for (int x = image.width - 1; x >= 0; x--) {

            for (int c = 0; c < image.channels; c++) {

                unsigned char currentPixelValue = image.getPixel(originalx, originaly, c);
                newImage.setPixel(x, y, c, currentPixelValue);
            }
            originalx++;
        }
        originaly++;
    }

    image = newImage;
}

void ImageFilter::Rotate270(Image& image) {
    Image newImage;
    newImage.width = image.height;
    newImage.height = image.width;
    newImage.channels = image.channels;
    newImage.imageData = new unsigned char[newImage.width * newImage.height * newImage.channels];

    int originaly = 0;
    for (int x = 0; x < newImage.width; x++) {

        int originalx = 0;
        for (int y = newImage.height - 1; y >= 0; y--) {

            for (int c = 0; c < newImage.channels; c++) {

                unsigned char currentPixelValue = image.getPixel(originalx, originaly, c);
                newImage.setPixel(x, y, c, currentPixelValue);
            }
            originalx++;
        }
        originaly++;
    }

    image = newImage;
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




void ImageFilter::AddFrame(Image& image, Color frameColor, int Framewidth, bool Fancy) {
    int oldW = image.width;
    int oldH = image.height;
    int newW = oldW + (2 * Framewidth);
    int newH = oldH + (2 * Framewidth);

    Image newImage(newW, newH);

    for (int x = 0; x < oldW; x++) {
        for (int y = 0; y < oldH; y++) {
            newImage.setPixel(x + Framewidth, y + Framewidth, 0, image.getPixel(x, y, 0));
            newImage.setPixel(x + Framewidth, y + Framewidth, 1, image.getPixel(x, y, 1));
            newImage.setPixel(x + Framewidth, y + Framewidth, 2, image.getPixel(x, y, 2));
        }
    }

    for (int w = 0; w < newW; w++) {
        for (int h = 0; h < newH; h++) {
            if (w < Framewidth || w >= newW - Framewidth || 
                h < Framewidth || h >= newH - Framewidth) {

                newImage.setPixel(w, h, 0, frameColor.red);
                newImage.setPixel(w, h, 1, frameColor.green);
                newImage.setPixel(w, h, 2, frameColor.blue);

                if (Fancy) {
                    int mid = Framewidth / 2;

                    bool outerLine = (w < 2 || w >= newW - 2 || h < 2 || h >= newH - 2);

                    bool innerLine = (w == Framewidth - 2 || w == newW - Framewidth + 2 ||
                                        h == Framewidth - 2 || h == newH - Framewidth + 2);
                    
                    bool middleLine = (w == mid || w == newW - mid - 2 ||
                                        h == mid || h == newH - mid - 2);

                    if (outerLine || innerLine || middleLine) {
                        newImage.setPixel(w, h, 0, 255);
                        newImage.setPixel(w, h, 1, 255);
                        newImage.setPixel(w, h, 2, 255);
                    }
                }
            }
        }
    }

    image = newImage;
}



void ImageFilter::FlipVertically(Image& image){
    
    for(int h = 0 ; h < image.height / 2 ; h++){
        for(int w = 0 ; w < image.width ; w++){
            int targetY = image.height - 1 - h;
            for(int c = 0 ; c < image.channels ; c++){
                std::swap(image.getPixel(w , h , c) , image.getPixel(w , targetY , c));
            }
        }
    }
}

void ImageFilter::FlipHorizontally(Image& image){

    for(int w = 0 ; w < image.width / 2 ; w++){
        for(int h = 0 ; h < image.height ; h++){
            int targetX = image.width - 1 - w;
            for(int c = 0 ; c < image.channels ; c++){
                std::swap(image.getPixel(w , h , c) , image.getPixel(targetX , h , c));
            }
        }
    }
}




void ImageFilter::Resize(Image& image, int newWidth, int newHeight) {
    Image newImage(newWidth, newHeight);

    for (int w = 0; w < newWidth; w++) {
        for (int h = 0; h < newHeight; h++) {
            int OldWidth = (w * image.width) / newWidth;
            int OldHeight = (h * image.height) / newHeight;

            newImage.setPixel(w, h, 0, image.getPixel(OldWidth, OldHeight, 0));
            newImage.setPixel(w, h, 1, image.getPixel(OldWidth, OldHeight, 1));
            newImage.setPixel(w, h, 2, image.getPixel(OldWidth, OldHeight, 2));
        }
    }

    image = newImage;
}


void ImageFilter::Blur(Image& image, int level) {
    while (level--) {
        Image newImage = image;

        for (int w = 2; w < image.width - 2; w++) {
            for (int h = 2; h < image.height - 2; h++) {
                
                int sumr = 0, sumg = 0, sumb = 0;

                for (int Dw = -2; Dw <= 2; Dw++) {
                    for (int Dh = -2; Dh <= 2; Dh++) {
                        sumr += image.getPixel(w + Dw, h + Dh, 0);
                        sumg += image.getPixel(w + Dw, h + Dh, 1);
                        sumb += image.getPixel(w + Dw, h + Dh, 2);
                    }
                }

                newImage.setPixel(w, h, 0, sumr / 25);
                newImage.setPixel(w, h, 1, sumg / 25);
                newImage.setPixel(w, h, 2, sumb / 25);
            }
        }

        image = newImage;
    }
}
