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
	string memorycell;
public:
	Memory();
	Memory(string x);
	void setMemory(string value);
	string getMemory();
	void clearMemory();
	void printMemory();
};
int main() {
	Memory x;
	Memory y("34");
	x.printMemory();
	y.printMemory();

}
Memory::Memory() {
	memorycell = "00";
}
Memory::Memory(string x) {
	memorycell = x;
}
void Memory::setMemory(string value) {
	memorycell = value;
}
string Memory::getMemory (){
   return memorycell;
}
void Memory::clearMemory() {
	memorycell = "  ";
}
void Memory::printMemory() {
	cout << memorycell;
}
