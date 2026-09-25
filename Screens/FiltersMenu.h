#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"string.h"
#include<conio.h>
#include"Screen.h"
#include"ImageFilter.h"
#include"FilterScreen.h"


class FiltersMenuScreen : Screen{
    
    ImageEditor &editor;

    enum FiltersMenuOption {
        Grayscale = 1,
        BlackAndWhite,
        InvertColors,
        AddFrame,
        Flip,
        Rotate,
        Darken,
        Lighten,
        Resize,
        Merge,
        DetectEdges,
        Crop,
        Blur,
        Sunlight,
        OldTV,
        PurpleEffect,
        Infrared,
        Skew,
        OilPainting,
        ApplyChanges,
        Cancel
    };


    static void _goBackToFiltersMenu(){
        std::cout << "\n\t\t\tpress any key to go back to filters menu ...";
        getch();
    }


public:
    FiltersMenuScreen(ImageEditor &ed) : editor(ed){}

    void show(){
        bool running = true;

        while(running){
            clearScreen();
            headerScreen("Filters Menu");

            std::cout << "\t\t\t[1]  Grayscale Filter\n";
            std::cout << "\t\t\t[2]  Black & White Filter\n";
            std::cout << "\t\t\t[3]  Invert Image Colors\n";
            std::cout << "\t\t\t[4]  Add Frame\n";
            std::cout << "\t\t\t[5]  Flip Image (Horizontal/Vertical)\n";
            std::cout << "\t\t\t[6]  Rotate Image\n";
            std::cout << "\t\t\t[7]  Darken\n";
            std::cout << "\t\t\t[8]  Lighten\n";
            std::cout << "\t\t\t[9]  Resize Image\n";
            std::cout << "\t\t\t[10] Merge Images\n";
            std::cout << "\t\t\t[11] Detect Edges\n";
            std::cout << "\t\t\t[12] Crop Image\n";
            std::cout << "\t\t\t[13] Blur Image\n";
            std::cout << "\t\t\t[14] Sunlight Effect\n";
            std::cout << "\t\t\t[15] Old TV Effect\n";
            std::cout << "\t\t\t[16] Purple Effect\n";
            std::cout << "\t\t\t[17] Infrared Effect\n";
            std::cout << "\t\t\t[18] Skew Image\n";
            std::cout << "\t\t\t[19] Oil Painting Effect\n";
            std::cout << "\t\t\t[20] Apply Changes\n";
            std::cout << "\t\t\t[21] Cancel\n";
            std::cout << "\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(
                1, 21, 
                "\t\t\tChoose what do you want to do [1 - 21] : ", 
                "\t\t\tInvalid number, please enter a number in range 1 - 21"
            );

            switch (static_cast<FiltersMenuOption>(choiceNumber)) {

                case FiltersMenuOption::Grayscale:{
                    FilterScreen::simpleScreen(editor , "Grayscale" , FilterType::Grayscale);
                    _goBackToFiltersMenu();
                    break;
                }

                case FiltersMenuOption::BlackAndWhite:
                    FilterScreen::simpleScreen(editor , "Black And White" , FilterType::BlackAndWhite);
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::InvertColors:
                    FilterScreen::simpleScreen(editor , "Invert Colors" , FilterType::InvertImage);
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::AddFrame:
                    
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Flip:
                    
                    _goBackToFiltersMenu();
                    break;
                case FiltersMenuOption::Rotate:
                    FilterScreen::rotateScreen(editor);
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Darken:
                    FilterScreen::simpleScreen(editor , "Darken" , FilterType::Darken);
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Lighten:
                    FilterScreen::simpleScreen(editor , "Lighten" , FilterType::Lighten);
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Resize:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Merge:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::DetectEdges:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Crop:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Blur:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Sunlight:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::OldTV:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::PurpleEffect:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Infrared:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::Skew:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::OilPainting:
                    _goBackToFiltersMenu();
                    break;

                case FiltersMenuOption::ApplyChanges:{
                    bool conf = InputValidation::confirm("\t\t\tApply all changes to the image ?");

                    if(conf){
                        std::cout << "\t\t\tChanges applied.\a\n";
                    }
                    
                    std::cout << "\n\t\t\tpress any key to go back to the main menu ...";
                    getch();

                    running = false;
                    break;
                }

                default:
                    running = false;
                    break;
            }
        }
    }
        
        
};
