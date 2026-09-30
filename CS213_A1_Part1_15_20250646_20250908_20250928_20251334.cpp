/*
-------------------------------------------------------------------------------------
|       Name        |       ID       |       Section    |     Filters               |
-------------------------------------------------------------------------------------
|Moustafa Mahmoud   |    20250646    |          15      |   Grayscale, Flip         |
|Moataz Mohamed     |    20250908    |          15      |   Black&White, Rotate     |
|Abdallah Ibrahim   |    20250928    |          15      |   Invert, Darken&Lighten  |
|Mohamed Sherif     |    20251334    |          15      |   Add Frame, Blur         |
-------------------------------------------------------------------------------------
*/
#include "Image_Class.h"
#include <algorithm>
#include <string>
#include <deque>
#include <stack>
#include <exception>
#include <conio.h>
#include <filesystem>

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
    Rotate90, Rotate180, Rotate270, Darken, Lighten, Resize, Blur
};

enum AdjustmentType{
	Brightness = 1, Contrast, Saturation
};

namespace Adjustments {

    void Brightness(Image& image, int brightness) {

        brightness = std::max(-100, std::min(100, brightness));

        for (int w = 0; w < image.width; w++) {
            for (int h = 0; h < image.height; h++) {
                for (int c = 0; c < image.channels; c++) {

                    if((image.channels == 4 || image.channels == 2) && (c == image.channels - 1)) {
                        continue; 
                    }

                    unsigned char currentPixel = image.getPixel(w, h, c);
                    int newPixelValue = (static_cast<int>(currentPixel)) * (1.0 + (brightness / 100.0));
                    newPixelValue = std::max(0, std::min(255, newPixelValue));
                    image.setPixel(w, h, c, static_cast<unsigned char>(newPixelValue));

                }
            }
        }
    }

    void Contrast(Image &image, int contrast){
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

    void Saturation(Image &image, int saturation) {
        saturation = std::max(-100, std::min(100, saturation));
        float factor = 1.0f + saturation / 100.0f;
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                unsigned char& red = image.getPixel(x, y, 0);
                unsigned char& green = image.getPixel(x, y, 1);
                unsigned char& blue = image.getPixel(x, y, 2);

                unsigned char grayValue = (red + green + blue) / 3;

                int newRed = static_cast<int>(grayValue + factor * (red - grayValue));
                int newGreen = static_cast<int>(grayValue + factor * (green - grayValue));
                int newBlue = static_cast<int>(grayValue + factor * (blue - grayValue));

                red = std::max(0, std::min(255, newRed));
                green = std::max(0, std::min(255, newGreen));
                blue = std::max(0, std::min(255, newBlue));
            }
        }
    }
}

namespace ImageFilter {

    void grayscale(Image& image){

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

    void FlipVertically(Image& image){
        
        for(int h = 0 ; h < image.height / 2 ; h++){
            for(int w = 0 ; w < image.width ; w++){
                int targetY = image.height - 1 - h;
                for(int c = 0 ; c < image.channels ; c++){
                    std::swap(image.getPixel(w , h , c) , image.getPixel(w , targetY , c));
                }
            }
        }
    }

    void FlipHorizontally(Image& image){

        for(int w = 0 ; w < image.width / 2 ; w++){
            for(int h = 0 ; h < image.height ; h++){
                int targetX = image.width - 1 - w;
                for(int c = 0 ; c < image.channels ; c++){
                    std::swap(image.getPixel(w , h , c) , image.getPixel(targetX , h , c));
                }
            }
        }
    }

    void BlackandWhite(Image& image) {
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

    void Rotate90(Image& image) {
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

    void Rotate180(Image& image) {
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

    void Rotate270(Image& image) {
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


    void Invert(Image& image) {
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
        
    void DarkenandLighten(Image& image, int brightness) {
        Adjustments::Brightness(image, brightness);
    }


    void AddFrame(Image& image, Color frameColor, int Framewidth, bool Fancy) {
        int oldW = image.width;
        int oldH = image.height;
        int newW = oldW + (2 * Framewidth);
        int newH = oldH + (2 * Framewidth);

        Color fancyLines = (frameColor == WHITE ? BLACK : WHITE);

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
                            newImage.setPixel(w, h, 0, fancyLines.red);
                            newImage.setPixel(w, h, 1, fancyLines.green);
                            newImage.setPixel(w, h, 2, fancyLines.blue);
                        }
                    }
                }
            }
        }

        image = newImage;
    }

    void Blur(Image& image, int level) {
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

}

class ImageEditor {
private:
    Image originalImage;
    Image currentImage;
    std::deque<Image> changes;
    std::stack<Image> changesUndone;
    bool modified = false;
    std::string originalFilePath;

public:
    ImageEditor() = default;

    ImageEditor(const std::string& imageFilePath) {
        loadImageFromFile(imageFilePath);
    }

    bool loadImageFromFile(const std::string& imageFilePath) {
        unloadImage();
        originalImage.loadNewImage(imageFilePath);
        originalFilePath = imageFilePath;
        currentImage = originalImage;
        return true;
    }

    bool saveToSameImage() {
        currentImage.saveImage(originalFilePath);
        modified = false;
        return true;
    }
    
    bool saveNewCopy(const std::string& newFilePath) {
        currentImage.saveImage(newFilePath);
        modified = false;
        return true;
    }
    
    void reset() {
        if (!isImageLoaded()) {
            throw std::runtime_error("ERROR! No image loaded.");
        }

        if (changes.empty()) {
            throw std::runtime_error("No changes to reset.");
        }

        currentImage = originalImage;
        changes.clear();
        while (!changesUndone.empty()) { 
            changesUndone.pop();
        }
    }
    
    void undo() {
        if (!isImageLoaded()) {
            throw std::runtime_error("ERROR! No image loaded.");
        }

        if (changes.empty()) {
            throw std::runtime_error("No changes to undo.");
        }
        changesUndone.push(currentImage);
        currentImage = changes.back();
        changes.pop_back();
    }
    
    void redo() {
        if (!isImageLoaded()) {
            throw std::runtime_error("ERROR! No image loaded.");
        }

        if (changesUndone.empty()) {
            throw std::runtime_error("No changes to redo.");
        }

        changes.push_back(currentImage);
        currentImage = changesUndone.top();
        changesUndone.pop();
    }
    
    void applyFilter(FilterType filterType) {
        saveState();

        switch (filterType) {
            case FilterType::Grayscale:
                ImageFilter::grayscale(currentImage);
                break;

            case FilterType::BlackAndWhite:
                ImageFilter::BlackandWhite(currentImage);
                break;

            case FilterType::InvertImage:
                ImageFilter::Invert(currentImage);
                break;

            case FilterType::Darken:
                ImageFilter::DarkenandLighten(currentImage, -50);
                break;

            case FilterType::Lighten:
                ImageFilter::DarkenandLighten(currentImage, 50);
                break;

            case FilterType::Rotate90:
                ImageFilter::Rotate90(currentImage);
                break;

            case FilterType::Rotate180:
                ImageFilter::Rotate180(currentImage);
                break;

            case FilterType::Rotate270:
                ImageFilter::Rotate270(currentImage);
                break;

            case FilterType::FlipVertically:
                ImageFilter::FlipVertically(currentImage);
                break;

            case FilterType::FlipHorizontally:
                ImageFilter::FlipHorizontally(currentImage);
                break;

            case FilterType::Blur:
                ImageFilter::Blur(currentImage, 2);
                break;

            default:
                throw std::invalid_argument("Unsupported Filter");
            
        }
        modified = true;
    }
    
    void applyAdjust(AdjustmentType adjustType, int value) {
        saveState();

        switch (adjustType) {
            case AdjustmentType::Brightness:
                Adjustments::Brightness(currentImage, value);
                break;
            case AdjustmentType::Contrast:
                Adjustments::Contrast(currentImage, value);
                break;
            case AdjustmentType::Saturation:
                Adjustments::Saturation(currentImage, value);
                break;
            default:
                throw std::invalid_argument("Unsupported Feature");
        }
        modified = true;
    }

    void addFrame(Color frameColor, int frameThickness, bool fancy) {
        saveState();
        ImageFilter::AddFrame(currentImage, frameColor, frameThickness, fancy);
    }
    
    bool isModified() {
        return modified;
    }
    
    void unloadImage() {
        if (!isImageLoaded()) {
            return;
        }

        originalImage = Image();
        currentImage = Image();
        changes.clear();
        while (!changesUndone.empty()) {
            changesUndone.pop();
        }
        modified = false;
        originalFilePath = "";
    }
    
private:
    void saveState() {
        if (changes.size() == 5) {
            changes.pop_front();
        }

        changes.push_back(currentImage);
        
        while (!changesUndone.empty()) {
            changesUndone.pop();
        }
    }
    
    bool isImageLoaded() {
        if (currentImage.imageData == nullptr) {
            return false;
        }
        return true;
    }
};

//=========================================================================
// Helper Functions
namespace InputValidation{

    int readInt(std::string msg = "enter a number : ",
        std::string errorMSg = "this is not a valid number , please enter a valid number : "){
        int num;
        std::cout << msg;
        while(true){
            
            std::cin >> num;
            if(std::cin.fail()){
                std::cout << errorMSg;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else{
                break;
            }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return num;
    }

    std::string readString(std::string msg = "enter a string : " ,
         std::string errorMsg = "invalid string , please enter a non empty string : "){
        std::cout << msg;
        std::string s;
        getline(std::cin , s);
        while(s == ""){
            std::cout << errorMsg;
            getline(std::cin , s);
        }
        return s;
    }

    bool isNumInRange(int n , int start , int end){
        return n <= end && n >= start;
    }

    bool isNumInRange(double n , double start , double end){
        return n <= end && n >= start;
    }

    int readIntInRange(int start , int end , std::string msg = "enter a number : ",
        std::string errorMSg = "this is not a valid number , please enter a valid number : "){
        int num = readInt(msg , errorMSg);
        while(!isNumInRange(num , start , end)){
            std::cout << errorMSg << '\n';
            num = readInt(msg , errorMSg);
        }
        return num;
    }

    int readPositiveInt(std::string msg = "enter a number : ",
        std::string errorMsg = "this is not a valid number , please enter a valid number : "){
        int num = InputValidation::readInt(msg , errorMsg);
        while (num <= 0){
            std::cout << errorMsg;
            num = InputValidation::readInt(msg , errorMsg);
        }

        return num;
        
        
    }

    bool confirm(const std::string &msg = "Are You Sure ?"){
        std::cout << msg << " (Y / N) : ";
        std::string cont; getline(std::cin , cont);
        return (cont == "y" || cont == "Y");
    }

};

//=========================================================================
// UI
class Screen {
public:
    static void headerScreen(std::string title){
        std::cout <<"\t\t\t__________________________________\n";
        std::cout << "\t\t\t\t" << title << '\n';
        std::cout <<"\t\t\t__________________________________\n\n";
        
    }

    static void clearScreen(){
        system("cls");
    }

    virtual void show() = 0;
};

namespace FilterScreen {

    void simpleScreen(ImageEditor &editor , const std::string &filterName , FilterType type){
        Screen::clearScreen();
        Screen::headerScreen(filterName);
        bool conf = InputValidation::confirm("\t\t\tApply " + filterName + " ?");

        if(conf){
            editor.applyFilter(type);
            std::cout << "\t\t\t" + filterName + " applied\a\n";
        }
    }

    void rotateScreen(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Rotate");

        std::cout << "\t\t\t[1] Rotate 90\n";
        std::cout << "\t\t\t[2] Rotate 180\n";
        std::cout << "\t\t\t[3] Rotate 270\n";
        std::cout <<"\t\t\t=======================================\n";

        int choiceNumber = InputValidation::readIntInRange(1,3 , "\t\t\tchoose what do you want to do [1 - 3] : " , "\t\t\tinvalid number , please enter a number in range 1 - 3");

        FilterType type = (choiceNumber == 1 ? FilterType::Rotate90 
            : (choiceNumber == 2 ? FilterType::Rotate180 : FilterType::Rotate270));

        bool conf = InputValidation::confirm("\t\t\tApply rotation ?");

        if(conf){
            editor.applyFilter(type);
            std::cout << "\t\t\tRotation applied\a\n";
        }
    }

    void FlipScreen(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Flip");

        std::cout << "\t\t\t[1] Flip Vertically\n";
        std::cout << "\t\t\t[2] Flip Horizontally\n";
        std::cout <<"\t\t\t=======================================\n";

        int choiceNumber = InputValidation::readIntInRange(1,2 ,
            "\t\t\tchoose what do you want to do [1 - 2] : ",
            "\t\t\tinvalid number , please enter a number in range 1 - 2");

        FilterType type = (choiceNumber == 1 ? FilterType::FlipVertically : FilterType::FlipHorizontally);

        bool conf = InputValidation::confirm("\t\t\tApply flip ?");

        if(conf){
            editor.applyFilter(type);
            std::cout << "\t\t\tFlip applied\a\n";
        }
    }

    void AddFrameScreen(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Add Frame");

        std::cout << "\t\t\t[1] Simple Frame\n";
        std::cout << "\t\t\t[2] Fancy Frame\n";
        std::cout <<"\t\t\t=======================================\n";

        int choiceNumber = InputValidation::readIntInRange(
            1, 2,
            "\t\t\tChoose Frame Style [1 - 2]: ",
            "\t\t\tInvalid Number, please enter a number in range 1 - 2"
        );
        bool isFancyFrame = choiceNumber == 1 ? false : true;

        std::cout << "\t\t\t[1] Black\n";
        std::cout << "\t\t\t[2] White\n";
        std::cout << "\t\t\t[3] Red\n";
        std::cout << "\t\t\t[4] Green\n";
        std::cout << "\t\t\t[5] Blue\n";

        Color frameColor;
        int ColorChoice = InputValidation::readIntInRange(
            1, 5,
            "\t\t\tChoose a color [1 - 5]: ",
            "\t\t\tInvalid Choice, Enter a number between 1 and 5"
        );

        switch (ColorChoice) {
            case 1:
                frameColor = BLACK;
                break;
            case 2:
                frameColor = WHITE;
                break;
            case 3:
                frameColor = RED;
                break;
            case 4:
                frameColor = GREEN;
                break;
            case 5:
                frameColor = BLUE;
                break;
            default:
                break;
        }

        int thickness = InputValidation::readIntInRange(
            1, 100,
            "\t\t\tEnter The Frame Thickness [1 - 100]: ",
            "\t\t\tInvalid Number, Enter a number between 1 and 100\n"
        );

        bool conf = InputValidation::confirm("\t\t\tApply frame ?");

        if (conf) {
            editor.addFrame(frameColor, thickness, isFancyFrame);
            std::cout << "\t\t\tFrame Added Successfully\a\n";
        }
    }
}

namespace AdjustmentScreen {
    void adjustmentScreen(ImageEditor &editor, const std::string &adjustmentname, AdjustmentType type){
        Screen::clearScreen();
        Screen::headerScreen(adjustmentname);
        int value = InputValidation::readIntInRange(-100, 100, "\t\t\tEnter value [-100 to 100]: ", "\t\t\tInvalid value! ");
        bool conf = InputValidation::confirm("\t\t\tApply " + adjustmentname +" ?");
        if(conf){
            editor.applyAdjust(type, value);
            std::cout<<"\t\t\t" <<adjustmentname <<" applied with " << value <<"%" <<"\a\n";
        }
    }
};

class AdjustMenuScreen : Screen {
private:
    ImageEditor &editor;

    enum enAdjustMenuOptions{Brightness = 1, Contrast , Saturation , BackToMain};

    static void _goBackToAdjustMenu(){
        std::cout << "\n\t\t\tpress any key to go back to Adjust menu ...";
        getch();
    }

public:
    AdjustMenuScreen(ImageEditor &ed) : editor(ed) {}

    void show() {
        bool running = true;

        while (running) {
            clearScreen();
            headerScreen("Adjust Menu");

            std::cout << "\t\t\t[1]  Brightness\n";
            std::cout << "\t\t\t[2]  Contrast\n";
            std::cout << "\t\t\t[3]  Saturation\n";
            std::cout << "\t\t\t[4]  Back to Main Menu\n";
            std::cout << "\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(
                1, 19, 
                "\t\t\tChoose what do you want to do [1 - 4] : ", 
                "\t\t\tInvalid number, please enter a number in range 1 - 4 : "
            );

            switch (static_cast<enAdjustMenuOptions>(choiceNumber)) {

                case enAdjustMenuOptions::Brightness:
                    AdjustmentScreen::adjustmentScreen(editor, "Brightness", AdjustmentType::Brightness);
                    _goBackToAdjustMenu();
                    break;
                case enAdjustMenuOptions::Contrast:
                    AdjustmentScreen::adjustmentScreen(editor, "Contrast", AdjustmentType::Contrast);
                    _goBackToAdjustMenu();
                    break;
                case enAdjustMenuOptions::Saturation:
                    AdjustmentScreen::adjustmentScreen(editor, "Saturation", AdjustmentType::Saturation);
                    _goBackToAdjustMenu();
                    break;

                case enAdjustMenuOptions::BackToMain:
                    running = false;
                    break;
            }
        }
    }   
};

class FiltersMenuScreen : Screen {

    ImageEditor &editor;

    enum FiltersMenuOption {
        Grayscale = 1,
        BlackAndWhite, InvertColors, AddFrame, Flip, Rotate, Darken, Lighten, Blur,
        Cancel
    };

    static void _goBackToMainMenu() {
        std::cout << "\n\t\t\tpress any key to go back to main menu ...";
        getch();
    }

public:
    FiltersMenuScreen(ImageEditor &ed) : editor(ed) {}

    void show() {
        clearScreen();
        headerScreen("Filters Menu");

        std::cout << "\t\t\t[1]  Grayscale Filter\n";
        std::cout << "\t\t\t[2]  Black & White Filter\n";
        std::cout << "\t\t\t[3]  Invert Image Colors\n";
        std::cout << "\t\t\t[4]  Add Frame\n";
        std::cout << "\t\t\t[5]  Flip Image\n";
        std::cout << "\t\t\t[6]  Rotate Image\n";
        std::cout << "\t\t\t[7]  Darken\n";
        std::cout << "\t\t\t[8]  Lighten\n";
        std::cout << "\t\t\t[9] Blur Image\n";
        std::cout << "\t\t\t[10] Cancel\n";
        std::cout << "\t\t\t=======================================\n";

        int choiceNumber = InputValidation::readIntInRange(
            1, 20, 
            "\t\t\tChoose what do you want to do [1 - 20] : ", 
            "\t\t\tInvalid number, please enter a number in range 1 - 20"
        );

        switch (static_cast<FiltersMenuOption>(choiceNumber)) {

            case FiltersMenuOption::Grayscale:
                FilterScreen::simpleScreen(editor, "Grayscale", FilterType::Grayscale);
                break;

            case FiltersMenuOption::BlackAndWhite:
                FilterScreen::simpleScreen(editor, "Black And White", FilterType::BlackAndWhite);
                break;

            case FiltersMenuOption::InvertColors:
                FilterScreen::simpleScreen(editor, "Invert Colors", FilterType::InvertImage);
                break;

            case FiltersMenuOption::AddFrame:
                FilterScreen::AddFrameScreen(editor);
                break;

            case FiltersMenuOption::Flip:
                FilterScreen::FlipScreen(editor);
                break;

            case FiltersMenuOption::Rotate:
                FilterScreen::rotateScreen(editor);
                break;

            case FiltersMenuOption::Darken:
                FilterScreen::simpleScreen(editor, "Darken", FilterType::Darken);
                break;

            case FiltersMenuOption::Lighten:
                FilterScreen::simpleScreen(editor, "Lighten", FilterType::Lighten);
                break;
            
            case FiltersMenuOption::Blur:
                FilterScreen::simpleScreen(editor, "Blur", FilterType::Blur);
                break;
        }
        
        if (static_cast<FiltersMenuOption>(choiceNumber) != FiltersMenuOption::Cancel) {
            _goBackToMainMenu();
        }
    }
};

class MainMenuScreen : protected Screen {
private:
    ImageEditor &editor;

    enum enMenuOptions{Filters = 1, Adjust , Undo , Redo , Reset , Save , SaveAs , Exit};

    void pressToContinue(){
        std::cout << "\n\t\t\tpress any key to continue ...";
        getch();
    }
    
public:
    MainMenuScreen(ImageEditor &ed) : editor(ed){}

    void show(){
        bool running = true;

        while (running) {
            clearScreen();
            headerScreen("Image Nightmare");
            std::cout << "\t\t\t[1] Apply Filters\n";
            std::cout << "\t\t\t[2] Adjust\n";
            std::cout << "\t\t\t[3] Undo\n";
            std::cout << "\t\t\t[4] Redo\n";
            std::cout << "\t\t\t[5] Reset\n";
            std::cout << "\t\t\t[6] save\n";
            std::cout << "\t\t\t[7] Save As\n";
            std::cout << "\t\t\t[8] Exit\n";
            std::cout <<"\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(1,8 , "\t\t\tchoose what do you want to do [1 - 8] : " , "\t\t\tinvalid number , please enter a number in range 1 - 8 : ");
            
            switch (static_cast<enMenuOptions>(choiceNumber)) {
            
                case enMenuOptions::Filters: {

                    FiltersMenuScreen filtersMenu(editor);
                    filtersMenu.show();
                    
                    break;
                }
                case enMenuOptions::Adjust: {

                    AdjustMenuScreen adjustMenu(editor);
                    adjustMenu.show();

                    break;
                }
                case enMenuOptions::Undo: {
                    
                    bool conf = InputValidation::confirm("\t\t\tAre you sure you want to undo changes?");

                    if (conf) {
                        try {
                            editor.undo();
                            std::cout << "\t\t\tChanges undone.\n";
                        }
                        catch (const std::exception& e) {
                            std::cout << "\t\t\t" <<e.what() << '\n';
                        }
                    }
                    pressToContinue();
                    break;
                }
                case enMenuOptions::Redo: {
                    bool conf = InputValidation::confirm("\t\t\tAre you sure you want to redo changes?");

                    if (conf) {
                        try {
                            editor.redo();

                            std::cout << "\t\t\tChanges redone.\n";
                            
                        }
                        catch(const std::exception& e) {
                            std::cout << "\t\t\t" <<e.what() << '\n';
                        }
                    }
                    pressToContinue();
                    break;
                }
                case enMenuOptions::Reset:{
                    
                    bool conf = InputValidation::confirm("\t\t\tAre you sure you want to reset changes?");

                    if(conf){
                        try
                        {
                            editor.reset();

                            std::cout << "\t\t\tImage reset to original state.\n";
                            
                        }
                        catch(const std::exception& e)
                        {
                            std::cout << "\t\t\t" <<e.what() << '\n';
                        }
                    }

                    pressToContinue();
                    
                    break;
                }

                case enMenuOptions::Save:{
                    editor.saveToSameImage();
                    std::cout << "\t\t\tImage Saved Successfuly\n\a";
                    
                    pressToContinue();

                    break;
                }

                case enMenuOptions::SaveAs: {
                    std::string newImagePath = InputValidation::readString("\t\t\tEnter new image path : ");
                    if (std::filesystem::exists(newImagePath)){
                        if(InputValidation::confirm("\t\t\tAre you sure you want to overwrite this file ?")){

                            try
                            {
                                editor.saveNewCopy(newImagePath);
                                editor.loadImageFromFile(newImagePath);
                                std::cout << "\t\t\tImage Saved Successfuly\n\a";
                            }
                            catch(const std::exception& e){}
                            
                        }
                    }
                    else{

                        try
                        {
                            editor.saveNewCopy(newImagePath);
                            editor.loadImageFromFile(newImagePath);
                            std::cout << "\t\t\tImage Saved Successfuly\n\a";
                        }
                        catch(const std::exception& e){}
                    
                    }
                    pressToContinue();
                    break;
                }

                case enMenuOptions::Exit:

                    if (editor.isModified()) {
                        bool conf = InputValidation::confirm("\t\t\tAre you sure you want to leave without saving?");

                        if (conf) {
                            running = false;
                            break;
                        }
                    }
                    else {
                        running = false;
                        break;
                    }
            }
        }
    }
};

class StartScreen : Screen {
private:
    ImageEditor &editor;

public:
    StartScreen(ImageEditor &ed) : editor(ed) {}

    void show(){
        clearScreen();
        headerScreen("Image Nightmare");
        while (true) {
            std::string imagePath = InputValidation::readString("\t\t\tEnter image path : ");
            
            try {
                editor.loadImageFromFile(imagePath);
                MainMenuScreen mainMenu(editor);
                mainMenu.show();
                clearScreen();
                headerScreen("Image Nightmare");
            }
            catch(const std::invalid_argument & e)
            {
                // std::cerr << e.what() << '\n';
                // imagePath = InputValidation::readString("\t\t\tPlease enter a valid path: ");
            }
        }
    }
    
};

int main() {
    ImageEditor editor;
    StartScreen appStartScreen(editor);
    appStartScreen.show();
    return 0;
}
