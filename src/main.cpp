#include<iostream>
#include "main menu.h";
#include "ImageEditor.h";


int main() {
    ImageEditor editor;
    MainMenuScreen mainMenu(editor);
    mainMenu.show();
    return 0;
}
