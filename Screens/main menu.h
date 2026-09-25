#pragma once

#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"string.h"
#include<conio.h>
#include"Screen.h"
#include"FiltersMenu.h"
#include"AdjustMenu.h"

class MainMenuScreen : protected Screen{

    ImageEditor &editor;

    enum enMenuOptions{Filters = 1, Adjust , Undo , Redo , Reset , Save , SaveAs , Exit};


    
public:

    MainMenuScreen(ImageEditor &ed) : editor(ed){}

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
            std::cout << "\t\t\t[6] save\n";
            std::cout << "\t\t\t[7] Save As\n";
            std::cout << "\t\t\t[8] Exit\n";
            std::cout <<"\t\t\t=======================================\n";

            int choiceNumber = InputValidation::readIntInRange(1,8 , "\t\t\tchoose what do you want to do [1 - 8] : " , "\t\t\tinvalid number , please enter a number in range 1 - 8 : ");
            
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

            case enMenuOptions::Save:{
                editor.saveToSameImage();
                std::cout << "\t\t\tImage Saved Successfuly\n";
                std::cout << "\n\t\t\tpress any key to continue ...";
                getch();
                break;
            }

            case enMenuOptions::Exit:
                if(editor.isModified()){
                    bool conf = InputValidation::confirm("\t\t\tAre you sure you want to leave without saving?");

                    if(conf){
                        running = false;
                        break;
                    }
                }
                else{
                    running = false;
                    break;
                }
            }
        }
        

        
    }


};

   