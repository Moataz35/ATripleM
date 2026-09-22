#include "Screens.h"

int Screens::MainMenu() {
    cout<<"\n=======================================================\n"
            <<"              BABY PHOTOSHOB - CS213                     \n"
            <<"=======================================================\n";


        cout <<"1. Load Image" << endl
            << "2. Save Image" << endl
            << "3. Show Filters" << endl
            << "4. Adjustments" << endl
            << "5. Undo" << endl
            << "6. Redo" << endl
            << "7. Reset" << endl
            << "8. Exit" << endl;

        int choice;
        cin >> choice;

        return choice;
        this_thread::sleep_for(chrono::seconds(2));
        system("cls");
}

int Screens::FLipFilters() {
    cout<<"\n=======================================================\n"
            <<"              BABY PHOTOSHOB - CS213                     \n"
            <<"=======================================================\n";

        cout <<"1. Flip Horizontally" << endl
            << "2. Flip Vertically" << endl;

        this_thread::sleep_for(chrono::seconds(2));
        system("cls");
}

int Screens::RotateFilters() {
    cout<<"\n=======================================================\n"
            <<"              BABY PHOTOSHOB - CS213                     \n"
            <<"=======================================================\n";

        cout <<"1. Rotate 90 degrees" << endl
            << "2. Rotate 180 degrees" << endl
            << "3. Rotate 270 degrees" << endl;
        
        int choice; cin>> choice;

        return choice;
        this_thread::sleep_for(chrono::seconds(2));
        system("cls");
}

int Screens::ShowFilters() {
    cout<<"\n=======================================================\n"
            <<"              BABY PHOTOSHOB - CS213                     \n"
            <<"=======================================================\n";

        cout <<"1. Grayscale" << endl
            << "2. Black and White" << endl
            << "3. Invert" << endl
            << "4. Adding Frame" << endl
            << "5. Flip" << endl
            << "6. Rotate" << endl
            << "7. Darken and Lighten" << endl
            <<"8. Resize" << endl
            <<"9. Merge Images" << endl
            << "10. Edge Detection" << endl
            << "11. Crop" << endl
            <<"12. Blur" << endl;
    cout<<"=======================================================\n"
        <<"Enter your choice: ";
        int choice; cin>> choice;
        return choice;
        this_thread::sleep_for(chrono::seconds(2));
        system("cls");
}