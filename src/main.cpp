#include<iostream>
#include "main menu.h";
#include "ImageEditor.h";


int main() {
    ImageEditor editor;
    MainMenuScreen mianMenu(editor);
    mianMenu.show();
    return 0;
}
