// fileloader.h
#include<fstream>
#include<vector>
#include<string>

// class: the read the file
class MyFirstClass {
	public:
		void readFile(const char*);
		void printf() const;
    private:
        std::vector<std::string> filebuffer;

};

// function: read the file according to the input file name
void MyFirstClass::readFile(const char* filename) {
	
    std::vector<std::string> line;

    std::ifstream file(filename, std::ios::in);
    //std::cout << file.is_open() << std::endl;

    if (!file.is_open()) {
        std::cout << "Failed to open file.\n";
    }
    else {
        std::string s;
        while (std::getline(file, s)) {
            filebuffer.push_back(s);
        }

        file.close();
    }
    file.close();

}

// function: print the read file
void MyFirstClass::printf() const{
    for (int i = 0; i < filebuffer.size(); i++) {
        std::cout << filebuffer.at(i) << std::endl;
    }
}