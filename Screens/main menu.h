#pragma once

#include<iostream>
#include"..\include\ImageEditor.h"
#include"input_validation_library.h"
#include"string.h"
#include<conio.h>
#include"Screen.h"
#include"FiltersMenu.h"
#include"AdjustMenu.h"

class MainMenuScreen : protected Screen{

    Editor &editor;

    enum enMenuOptions{Filters = 1, Adjust , Undo , Redo , Reset , Exit};

    static void _goBackToMainMenu(){
        std::cout << "\npress any key to go back to the main menu ...";
        getch();
    }

    
public:

    MainMenuScreen(Editor &ed) : editor(ed){}

    void show(){
        bool running = true;

        while(running){
            clearScreen();
            headerScreen("Image Nightmare");
            std::cout << "\t\t\t[1] Apply Filters\n";
            std::cout << "\t\t\t[2] Adjust\n";
            std::cout << "\t\t\t[3] Undo\n";
            std::cout << "\t\t\t[4] Redo\n";
            std::cout << "\t\t\t[5] Reset\n";
            std::cout << "\t\t\t[6] Exit\n";
            std::cout <<"\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(1,6 , "\t\t\tchoose what do you want to do [1 - 6] : " , "\t\t\tinvalid number , please enter a number in range 1 - 6 : ");
            
            switch (static_cast<enMenuOptions> (choiceNumber)){
            
            case enMenuOptions::Filters:{

                FiltersMenuScreen filtersMenu(editor);
                filtersMenu.show();
                
                break;
            }

            case enMenuOptions::Adjust:{

                AdjustMenuScreen adjustMenu(editor);
                adjustMenu.show();

                break;
            }

            case enMenuOptions::Undo:{

                break;
            }
            case enMenuOptions::Redo:{

                break;
            }
            case enMenuOptions::Reset:{

                break;
            }

            case enMenuOptions::Exit:
                running = false;
                break;
            }
        }
        

        
    }


};

   