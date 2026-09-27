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

}