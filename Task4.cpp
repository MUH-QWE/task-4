#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include"classes.h"

using namespace std;
using namespace CPU;


int main() {
    MachineSimulator simulator(16, 256);
    simulator.run();
    return 0;
}
