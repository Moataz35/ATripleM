#include<iostream>
#include "../Screens/main menu.h";
#include "../include/ImageEditor.h";


int main() {
    ImageEditor editor;
    MainMenuScreen mianMenu(editor);
    mianMenu.show();
    return 0;
}
