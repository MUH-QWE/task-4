#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <bitset>

using namespace std;

class Machine_Simulator {
private:
    vector<int> registers;
    vector<int> memory;
    bool halted;
    int pc;
    string ir;
    vector<string> program;
    bool jump_done;

public:
    Machine_Simulator() {
        registers.resize(16, 0);
        memory.resize(256, 0);
        halted = false;
        pc = 0;
        ir = " ";
        jump_done = false;
    }

    void load(const string& program_data) {
        istringstream iss(program_data);
        string instruction;
        while (iss >> instruction) {
            program.push_back(instruction);
        }
        pc = 0;
    }

    void execute() {
        while (pc < program.size() && !halted) {
            string instruction = program[pc];
            ir = instruction;
            cout << "Executing instruction: " << instruction << endl;
            execute_instruction(instruction);
            if (!jump_done) {
                pc += 2;
            }
            else {
                jump_done = false;
            }
        }
        if (halted) {
            cout << "Program halted." << endl;
        }
    }

    void execute_instruction(const string& instruction) {
        if (instruction.length() != 4) {
            cout << "Invalid instruction: " << instruction << endl;
            return;
        }
        int r;
        int address = -1;
        char opcode = toupper(instruction[0]);
        string operand = instruction.substr(1);
        switch (opcode) {
        case '1': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            address = strtol(operand.substr(1).c_str(), NULL, 16);
            registers[r] = memory[address] & 0xFF;
            break;
        }
        case '2': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            int value = strtol(operand.substr(1).c_str(), NULL, 16);
            registers[r] = value & 0xFF;
            break;
        }
        case '3': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            address = strtol(operand.substr(1).c_str(), NULL, 16);
            if (address == 0) {
                cout << "Output: " << (registers[r] & 0xFF) << endl;
            }
            else {
                memory[address] = registers[r] & 0xFF;
            }
            break;
        }
        case '4': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            int s = strtol(operand.substr(1, 1).c_str(), NULL, 16);
            registers[s] = registers[r] & 0xFF;
            break;
        }
        case '5': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            int s = strtol(operand.substr(1, 1).c_str(), NULL, 16);
            int t = strtol(operand.substr(2, 1).c_str(), NULL, 16);
            registers[r] = (registers[s] + registers[t]) & 0xFF;
            break;
        }
        case '6': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            int s = strtol(operand.substr(1, 1).c_str(), NULL, 16);
            int t = strtol(operand.substr(2, 1).c_str(), NULL, 16);
            registers[r] = static_cast<int>(static_cast<float>(registers[s]) + static_cast<float>(registers[t])) & 0xFF;
            break;
        }
        case 'B': {
            r = strtol(operand.substr(0, 1).c_str(), NULL, 16);
            address = strtol(operand.substr(1).c_str(), NULL, 16);
            if (registers[r] == registers[0]) {
                pc = address;
                jump_done = true;
            }
            break;
        }
        case 'C': {
            halted = true;
            break;
        }
        default:
            cout << "Invalid opcode: " << opcode << endl;
            break;
        }
    }

    void display_state() {
        cout << "Registers State (Binary):" << endl;
        for (int i = 0; i < 16; i++) {
            cout << "R" << i << ": " << bitset<8>(registers[i] & 0xFF) << endl;
        }
        cout << "\n(PC): " << pc << endl;
        cout << "(IR): " << ir << endl;
        cout << "\nMemory State:" << endl;
        for (int i = 0; i < 256; i++) {
            cout << i << ": " << memory[i] << endl;
        }
    }

    void run() {
        while (true) {
            cout << "Menu:" << endl;
            cout << "1-Load program from file" << endl;
            cout << "2-Execute program" << endl;
            cout << "3-Display state" << endl;
            cout << "4-Exit" << endl;
            cout << "Choose: ";
            int choice;
            cin >> choice;

            switch (choice) {
            case 1: {
                string file_path;
                cout << "Enter the file path: ";
                cin >> file_path;
                ifstream file(file_path.c_str());
                if (file.is_open()) {
                    stringstream buffer;
                    buffer << file.rdbuf();
                    load(buffer.str());
                }
                else {
                    cout << "Error: File not found." << endl;
                }
                break;
            }
            case 2:
                execute();
                break;
            case 3:
                display_state();
                break;
            case 4:
                return;
            default:
                cout << "Error: Invalid choice." << endl;
            }
        }
    }
};

int main() {
    Machine_Simulator simulator;
    simulator.run();
    return 0;
}
//iam here
// LISTEN
