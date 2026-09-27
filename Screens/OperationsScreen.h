
#pragma once
#include<iostream>
#include"ImageEditor.h"
#include"input_validation_library.h"
#include"Screen.h"

namespace Operation{
    void Undo(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Undo");
        bool conf = InputValidation::confirm("\t\t\tAre you sure you want to undo?");
        if(conf){
            try{
                editor.undo();
                std::cout<<"\t\t\tUndo successfully!\n";
            }
            catch(const std::exception &e){
                std::cout<<"\t\t\t"<<e.what()<<"\n";
            }
        }
    }
    void Redo(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Redo");
        bool conf = InputValidation::confirm("\t\t\tAre you sure you want to redo?");
        if(conf){
            try{
                editor.redo();
                std::cout<<"\t\t\tRedo successfully!\n";
            }
            catch(const std::exception &e){
                std::cout<<"\t\t\t"<<e.what()<<"\n";
            }
        }
    }
    void Reset(ImageEditor &editor){
        Screen::clearScreen();
        Screen::headerScreen("Reset");
        bool conf = InputValidation::confirm("\t\t\tAre you sure you want to reset?");
        if(conf){
            try{
                editor.reset();
                std::cout<<"\t\t\tReset successfully!\n";
            }
            catch(const std::exception &e){
                std::cout<<"\t\t\t"<<e.what()<<"\n";
            }
        }
    }
    
}