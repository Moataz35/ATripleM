#pragma once
#include <iostream>
#include "ImageEditor.h"
#include "input_validation_library.h"
#include "Screen.h"

namespace AdjustmentScreen{
    void adjustmentScreen(ImageEditor &editor, const std::string &adjustmentname, AdjustmentType type){
        Screen::clearScreen();
        Screen::headerScreen(adjustmentname);
        int value = InputValidation::readIntInRange(-100, 100, "\t\tEnter value [-100 to 100]: ", "\t\tInvalid value! ");
        bool conf = InputValidation::confirm("\t\t\tApply " + adjustmentname +" ?");
        if(conf){
            editor.applyAdjust(type, value);
            std::cout<<"\t\t\t" <<adjustmentname <<" applied with value " << value <<"\a\n";
        }
    }



};