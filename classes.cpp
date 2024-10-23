#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <bitset>
using namespace std;

class MashIne {
public:




private:
    vector<Memory> memory;
    vector<Regester> regester;
};
class Memory {
private:
    char memorycells[2];
public:
    Memory();
    void setMemory(int address, const char& value);
    char getMemory(int address) const;
    void clearMemory();
    void printMemory() const;
};
class Regester {
private:
    char regester[4];
public:
    Regester();
    void stetRegester(int address,const char&);
    char getRegester(int address) const;
    void clearRegester();
    void printRegester() const;
};
