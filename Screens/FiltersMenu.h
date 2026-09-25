#include <iostream>
#include <conio.h>
#include "ImageEditor.h"
#include "input_validation_library.h"
#include "string.h"
#include "Screen.h"
#include "ImageFilter.h"
#include "FilterScreen.h"

class FiltersMenuScreen : Screen {

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
        std::cout << "\t\t\t[20] Cancel\n";
        std::cout << "\t\t\t=======================================\n";

        int choiceNumber = InputValidation::readIntInRange(
            1, 20, 
            "\t\t\tChoose what do you want to do [1 - 20] : ", 
            "\t\t\tInvalid number, please enter a number in range 1 - 20"
        );

        switch (static_cast<FiltersMenuOption>(choiceNumber)) {

            case FiltersMenuOption::Grayscale:
                FilterScreen::simpleScreen(editor, "Grayscale", FilterType::Grayscale);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::BlackAndWhite:
                FilterScreen::simpleScreen(editor, "Black And White", FilterType::BlackAndWhite);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::InvertColors:
                FilterScreen::simpleScreen(editor, "Invert Colors", FilterType::InvertImage);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::AddFrame:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Flip:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Rotate:
                FilterScreen::rotateScreen(editor);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Darken:
                FilterScreen::simpleScreen(editor, "Darken", FilterType::Darken);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Lighten:
                FilterScreen::simpleScreen(editor, "Lighten", FilterType::Lighten);
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Resize:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Merge:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::DetectEdges:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Crop:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Blur:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Sunlight:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::OldTV:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::PurpleEffect:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Infrared:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Skew:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::OilPainting:
                _goBackToMainMenu();
                break;

            case FiltersMenuOption::Cancel:
                break;
        }
    }
};