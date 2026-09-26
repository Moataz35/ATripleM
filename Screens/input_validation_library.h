#pragma once

#include<iostream>
#include<limits>



namespace InputValidation{


    int readInt(std::string msg = "enter a number : ",
        std::string errorMSg = "this is not a valid number , please enter a valid number : "){
        int num;
        std::cout << msg;
        while(true){
            
            std::cin >> num;
            if(std::cin.fail()){
                std::cout << errorMSg;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else{
                break;
            }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return num;
    }

    double readDouble(std::string msg = "enter a number : ",
        std::string errorMsg = "this is not a valid number , please enter a valid number : "){
        double num;
        std::cout << msg;
        while(true){
            
            std::cin >> num;
            if(std::cin.fail()){
                std::cout << errorMsg;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else{
                break;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return num;
    }

    std::string readString(std::string msg = "enter a string : " ,
         std::string errorMsg = "invalid string , please enter a non empty string : "){
        std::cout << msg;
        std::string s;
        getline(std::cin , s);
        while(s == ""){
            std::cout << errorMsg;
            getline(std::cin , s);
        }
        return s;
    }

    bool isNumInRange(int n , int start , int end){
        return n <= end && n >= start;
    }

    bool isNumInRange(double n , double start , double end){
        return n <= end && n >= start;
    }

    int readIntInRange(int start , int end , std::string msg = "enter a number : ",
        std::string errorMSg = "this is not a valid number , please enter a valid number : "){
        int num = readInt(msg , errorMSg);
        while(!isNumInRange(num , start , end)){
            std::cout << errorMSg << '\n';
            num = readInt(msg , errorMSg);
        }
        return num;
    }
    
    int readDoubleInRange(double start , double end , std::string msg = "enter a number : ",
        std::string errorMSg = "this is not a valid number , please enter a valid number : "){
        double num = readDouble(msg , errorMSg);
        while(!isNumInRange(num , start , end)){
            std::cout << errorMSg << '\n';
            num = readDouble(msg , errorMSg);
        }
        return num;
    }

    double readPositiveDouble(std::string msg = "enter a number : ",
        std::string errorMsg = "this is not a valid number , please enter a valid number : "){
        double d = InputValidation::readDouble(msg , errorMsg);
        while (d <= 0){
            std::cout << errorMsg;
            d = InputValidation::readDouble(msg , errorMsg);
        }

        return d;
        
        
    }
    int readPositiveInt(std::string msg = "enter a number : ",
        std::string errorMsg = "this is not a valid number , please enter a valid number : "){
        int num = InputValidation::readInt(msg , errorMsg);
        while (num <= 0){
            std::cout << errorMsg;
            num = InputValidation::readInt(msg , errorMsg);
        }

        return num;
        
        
    }

    bool confirm(const std::string &msg = "Are You Sure ?"){
        std::cout << msg << " (Y / N) : ";
        std::string cont; getline(std::cin , cont);
        return (cont == "y" || cont == "Y");
    }

};

