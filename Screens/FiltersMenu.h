#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"string.h"
#include<conio.h>
#include"Screen.h"
#include"ImageFilter.h"


class FiltersMenuScreen : Screen{
    
    ImageEditor &editor;


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
            std::cout << "\t\t\t[7]  Darken & Lighten\n";
            std::cout << "\t\t\t[8]  Resize Image\n";
            std::cout << "\t\t\t[9]  Merge Images\n";
            std::cout << "\t\t\t[10] Detect Edges\n";
            std::cout << "\t\t\t[11] Crop Image\n";
            std::cout << "\t\t\t[12] Blur Image\n";
            std::cout << "\t\t\t[13] Sunlight Effect\n";
            std::cout << "\t\t\t[14] Old TV Effect\n";
            std::cout << "\t\t\t[15] Purple Effect\n";
            std::cout << "\t\t\t[16] Infrared Effect\n";
            std::cout << "\t\t\t[17] Skew Image\n";
            std::cout << "\t\t\t[18] Oil Painting Effect\n";
            std::cout << "\t\t\t[19] Back to Main Menu\n";
            std::cout << "\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(
                1, 19, 
                "\t\t\tChoose what do you want to do [1 - 19] : ", 
                "\t\t\tInvalid number, please enter a number in range 1 - 19 : "
            );

            switch (static_cast<FilterType>(choiceNumber)) {

                case FilterType::Grayscale:
                    
                    _goBackToFiltersMenu();
                    break;

                case FilterType::BlackAndWhite:
                    
                    _goBackToFiltersMenu();
                    break;

                case FilterType::InvertImage:
                   
                    _goBackToFiltersMenu();
                    break;

                case FilterType::AddFrame:
                    
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Flip:
                    
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Resize:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Merge:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::DetectEdges:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Crop:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Blur:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::SunLight:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::OldTV:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::PurpleEffect:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Infrared:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Skew:
                    _goBackToFiltersMenu();
                    break;

                case FilterType::Oil:
                    _goBackToFiltersMenu();
                    break;

                default:
                    running = false;
                    break;
            }
        }
    }
        
        
};
