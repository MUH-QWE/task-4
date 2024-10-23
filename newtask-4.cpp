#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <bitset>
using namespace std;

class machine{
private:
  vector<memory> cells;
  vector<Register> reg; 

};



class Memory {
private:
	string memorycell;
public:
	Memory();
	Memory(string x);
	void setMemory(string value);
	string getMemory();
	void clearMemory();
	void printMemory();
};
