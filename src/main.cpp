#include<iostream>
#include "Start Screen.h";
#include "ImageEditor.h";


int main() {
    ImageEditor editor;

    StartScreen start(editor);
    start.show();
}
