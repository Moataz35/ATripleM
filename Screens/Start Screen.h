#pragma once

#include<iostream>
#include"Screen.h"
#include"ImageEditor.h"
#include<conio.h>
#include"main menu.h"

class StartScreen : Screen{

    ImageEditor &editor;

    
public:

    StartScreen(ImageEditor &ed) : editor(ed){}

    void show(){
        clearScreen();
        headerScreen("Image Nightmare");
        while(true){
            std::string imagePath = InputValidation::readString("\t\t\tEnter image path : ");
            
            try
            {
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