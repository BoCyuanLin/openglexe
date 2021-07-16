// swap exercise 
// class: swap class
class swap_exe{
	public:
        void swap_int(int &, int &);
        void swap_float(float &, float &);
        void swap_double(double &, double &);
        void swap_char(char &, char &);
        template<class T> void swap_template(T &, T &);
        template<class T> void print_status(T &, T &);
};

// function: swap with type integer
void swap_exe::swap_int(int& a, int& b) {
    int temp;
    temp = a;
    a = b;
    b = temp;
}

// function: swap with type float
void swap_exe::swap_float(float& a, float& b) {
    float temp;
    temp = a;
    a = b;
    b = temp;
}

// function: swap with type double
void swap_exe::swap_double(double& a, double& b) {
    double temp;
    temp = a;
    a = b;
    b = temp;
}

// function: swap with type char
void swap_exe::swap_char(char& a, char& b) {
    char temp;
    temp = a;
    a = b;
    b = temp;
}

// function: swap with all type
template<class T>
void swap_exe::swap_template(T& a, T& b) {
    T temp;
    temp = a;
    a = b;
    b = temp;
}

// function: print the status of the number
template<class T>
void swap_exe::print_status(T &a, T &b) {
    std::cout << a << " <-> " << b << std::endl;
}
