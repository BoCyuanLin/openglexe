// exercises02.cpp 

#include <iostream>
#include "1_fileloader.h"

int main(){

    /*esercises02-1*/
    MyFirstClass file;
    file.readFile("testfile.txt");
    file.printf();

    system("pause");    
}
