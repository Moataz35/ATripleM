#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"Screen.h"

namespace FilterScreen{

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

        bool conf = InputValidation::confirm("\t\t\tApply flip ?");

        if (conf) {
            editor.addFrame(frameColor, thickness, isFancyFrame);
            std::cout << "\t\t\tFrame Added Successfully\a\n";
        }
    }

}
