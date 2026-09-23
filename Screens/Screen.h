#pragma once
#include<iostream>

class Screen{

protected:

    static void headerScreen(std::string title){
        std::cout <<"\t\t\t__________________________________\n";
        std::cout << "\t\t\t\t" << title << '\n';
        std::cout <<"\t\t\t__________________________________\n\n";
        
    }

    static void clearScreen(){
        system("cls");
    }

    virtual void show() = 0;


};