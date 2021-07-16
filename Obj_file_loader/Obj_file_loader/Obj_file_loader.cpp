// obj file loader
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "objloader.h"

// main function
int main(int argc, char* argv[]) {
    
    int width = 500, height = 500, is_text = 0;

    std::string filename, screenshot_path, texture_path;

    ReadObjfile t; // declare the Readobj object

    // set windows and initialize glut
    std::cout << "Please input the width and height for window:";
    std::cin >> width >> height;
    t.init_glut(argc, argv, width, height);

    // input filename of .obj
    std::cout << "Please input the Obj file name: ";
    std::cin >> filename;
    while (!t.readfile(filename.c_str())) {
        std::cout << "Please input the Obj file name again: ";
        std::cin >> filename;
    } // read the .obj
        
    // input saving path of screenshot
    std::cout << "Please input the screenshot path (name.png): ";
    std::cin >> screenshot_path;
    t.set_screenshot_path(screenshot_path.c_str()); // set screenshot path

    // using texture or not
    std::cout << "Is use texture? (If yes, please input 1, otherwise 0.): ";
    std::cin >> is_text;
    // input the path of texture with .bmp
    if (is_text) {
        std::cout << "Please input the texture path: ";
        std::cin >> texture_path;
        while (!t.texture_loader(texture_path.c_str())) {
            std::cout << "Please input the texture file name again: ";
            std::cin >> texture_path;
        } // read the .obj
        t.display_texture(1); // use the texture
    }

    t.display_light(1); // turn on the light
    t.draw(); // show the model in windows

    

    return 0;
}