#include <iostream>
#include"machinesimulator.h"

using namespace std;
using namespace MS;


int main() {
    MachineSimulator simulator(16, 256);
    simulator.run();
    return 0;
}
