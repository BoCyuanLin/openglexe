// exercise01.cpp

#include <iostream>
#include "1_swap_exe.h" // exercise01-1
#include "2_outmessage_exe.h" // exercise01-2
#include "3_2D_distance_exe.h" // exercise01-3
#include "4_3D_distance_exe.h" // exercise01-4

int main(){

    swap_exe s;
    /*exercises01-1*/
    std::cout << "Exercises01-1:\n\n" ;
    // swap integer
    std::cout << "[int]:\n" ;
    int x0 = 5, x1 = 6;
    s.print_status(x0, x1);
    s.swap_int(x0,x1);
    s.print_status(x0, x1);
    std::cout<<std::endl;
    // swap float
    std::cout << "[float]:\n" ;
    float y0 = 1.0f, y1 = 3.0f;
    s.print_status(y0, y1);
    s.swap_float(y0, y1);
    s.print_status(y0, y1);
    std::cout << std::endl;
    // swap double
    std::cout << "[double]:\n" ;
    double z0 = 0.0006, z1 = 0.03;
    s.print_status(z0, z1);
    s.swap_double(z0, z1);
    s.print_status(z0, z1);
    std::cout << std::endl;
    // swap char
    std::cout << "[char]:\n" ;
    char w0 = 'a', w1 = 'b';
    s.print_status(w0, w1);
    s.swap_char(w0, w1);
    s.print_status(w0, w1);
    std::cout << std::endl;
    // swap four types above
    std::cout << "[template version]:\n" ;
    s.swap_template(x0, x1);
    s.swap_template(y0, y1);
    s.swap_template(z0, z1);
    s.swap_template(w0, w1);
    s.print_status(x0, x1);
    s.print_status(y0, y1);
    s.print_status(z0, z1);
    s.print_status(w0, w1);

    /*exercises01-2*/
    std::cout << "\n\n";
    std::cout << "Exercises01-2:\n\n";
    out_message("You are very good!");

    /*exercises01-3*/
    std::cout << "\n\n";
    std::cout << "Exercises01-3:\n\n";
    float p0[] = { 0, 0 };
    float p1[] = { 3, 4 };
    std::wcout << "p0: (" << p0[0] << "," << p0[1] << ")\n";
    std::wcout << "p1: (" << p1[0] << "," << p1[1] << ")\n";
    distance2D dis2;
    float d = dis2.computeDistance(p0, p1);
    std::cout << "Distance: " << d << std::endl;

    /*exercises01-4*/
    std::cout << "\n\n";
    std::cout << "Exercises01-4:\n\n";
    float v0[] = { 0, 0, 0};
    float v1[] = { 3, 4, 5};
    std::wcout << "v0: (" << v0[0] << "," << v0[1] << "," << v0[2] << ")\n";
    std::wcout << "v1: (" << v1[0] << "," << v1[1] << "," << v1[2] << ")\n";
    distance3D dis3;
    float d3 = dis3.computeDistance(v0, v1);
    std::cout << "Distance: " << d3 << std::endl;

    system("pause");
}
