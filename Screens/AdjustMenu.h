#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"string.h"
#include<conio.h>
#include"Screen.h"

class AdjustMenuScreen : Screen{
    
    ImageEditor &editor;

    enum enAdjustMenuOptions{Brightness = 1, Contrast , Saturation , BackToMain};

    static void _goBackToAdjustMenu(){
        std::cout << "\n\t\t\tpress any key to go back to Adjust menu ...";
        getch();
    }

public:
    AdjustMenuScreen(ImageEditor &ed) : editor(ed){}

    void show(){
        bool running = true;

        while(running){
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
                    
                    _goBackToAdjustMenu();
                    break;
                case enAdjustMenuOptions::Contrast:
                    
                    _goBackToAdjustMenu();
                    break;
                case enAdjustMenuOptions::Saturation:
                    
                    _goBackToAdjustMenu();
                    break;

                case enAdjustMenuOptions::BackToMain:
                    running = false;
                    break;
            }
        }
        }
        
        
};

