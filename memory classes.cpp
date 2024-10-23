#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <bitset>
using namespace std;

class Memory {
private:
   cell memory;
public:
    Memory();
    void setMemory(int address, const char& value);
    char getMemory(int address) const;
    void clearMemory();
    void printMemory() const;
};
class cell {
private:
    char A
    char B
public:
    cell();
    void setbit(char value)
    void getcell(int pos)
};
    







Memory::Memory(){
    
}


