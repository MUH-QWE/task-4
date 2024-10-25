#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Memory {
private:
    vector<string> memory;

public:
    Memory(int Size) : memory(Size, "00") {}

    void store(int address, const string& value) {
        if (address >= 0 && address < 256) {
            memory[address] = value.substr(0, 2);
            memory[address + 1] = value.substr(2, 2);
        }
    }

    string load(int address) {
        string instruction;
        if (address >= 0 && address < 256) {
            instruction = memory[address] + memory[address + 1];
            return instruction;
        }
        return "0000";
    }

    void display() {
        int counter = 0;
        for (int i = 0; i < 256; ++i) {
            cout << memory[i] << " ";
            counter++;
            if (counter == 16) {
                cout << endl;
                counter = 0;
            }
        }
    }
};

class Register {
private:
    vector<string> registers;

public:
    Register(int numRegisters) : registers(numRegisters, "00") {}

    void set(int index, const string& value) {
        if (index >= 0 && index < registers.size()) {
            registers[index] = value;
        }
    }

    string get(int index) {
        if (index >= 0 && index < registers.size()) {
            return registers[index];
        }
        return "00";
    }

    void display() {
        for (const auto& reg : registers) {
            cout << reg << endl;
        }
    }
};

class Loader {
private:
    vector<string> program;

public:
    vector<string> loadFromFile(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Error: Unable to open file." << endl;
            return {};
        }
        string line;
        program.clear();
        while (getline(file, line)) {
            if (!line.empty() && line.size() == 4) {
                program.push_back(line);
            }
            else {
                cout << "Error: Invalid instruction in file: " << line << endl;
            }
        }
        if (program.empty()) {
            cout << "Error: Program is empty." << endl;
        }
        return program;
    }

    void loadToMemory(Memory& memory) {
        int address = 0;
        for (const auto& line : program) {
            memory.store(address, line);
            address += 2;
        }
    }

    vector<string> GetInstructions() {
        return program;
    }
};

class MachineSimulator {
private:
    bool halted;
    int pc;
    string instruction;
    Memory memory;
    Register registers;
    Loader Instructions;

public:
    MachineSimulator(int regSize, int memorySize)
        : halted(false), pc(0), instruction("0000"), registers(regSize), memory(memorySize) {}

    void loadProgramFromFile(const string& filePath) {
        Instructions.loadFromFile(filePath);
        Instructions.loadToMemory(memory);
    }

    void execute() {
        while (!halted) {
            instruction = memory.load(pc);
            cout << "PC: " << (pc / 2) << " | Instruction: " << instruction << endl;
            if (instruction.empty()) {
                cout << "Error: No instruction at PC = " << pc << endl;
                break;
            }
            executeInstruction(instruction);
            pc += 2;
        }
    }

    void executeInstruction(const string& instr) {
        char opCode = toupper(instr[0]);
        switch (opCode) {
        case '1':
            Address_registration(instr);
            break;
        case '2':
            Record_it_value(instr);
            break;
        case '3':
            R_in_XY(instr);
            break;
        case '4':
            Move(instr);
            break;
        case '5':
            Add(instr);
            break;
        case '6':
            Sum(instr);
            break;
        case 'B':
            Jump(instr);
            break;
        case 'C':
            halted = true;
            break;
        default:
            cout << "Error: Unknown instruction " << instr << endl;
        }
    }

    void Address_registration(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        string value = memory.load(stoi(address, nullptr, 16));
        registers.set(r, value.substr(2, 2));
    }


    void Record_it_value(const string& instr) {
        int r = instr[1] - '0';
        string value = instr.substr(2, 2);
        registers.set(r, value);
    }

    void R_in_XY(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        string value = registers.get(r);
        memory.store(stoi(address, nullptr, 16), value);
    }

    void Move(const string& instr) {
        int r = instr[2] - '0';
        int s = instr[3] - '0';
        string value = registers.get(r);
        registers.set(s, value);
    }

    void Add(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        string valueS = registers.get(s);
        string valueT = registers.get(t);
        int result = stoi(valueS, nullptr, 16) + stoi(valueT, nullptr, 16);
        if (result > 255) {
            result = 255;
        }
        registers.set(r, (result < 16 ? "0" : "") + to_string(result));
    }

    void Sum(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        string valueS = registers.get(s);
        string valueT = registers.get(t);
        int result = stoi(valueS, nullptr, 16) + stoi(valueT, nullptr, 16);
        if (result > 255) {
            result = 255;
        }
        registers.set(r, (result < 16 ? "0" : "") + to_string(result));
    }

    void Jump(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        if (registers.get(r) == registers.get(0)) {
            pc = stoi(address, nullptr, 16);
            return;
        }
    }

    void display_state() {
        cout << "Registers:" << endl;
        registers.display();
        cout << "PC: " << (pc / 2) << endl;
        cout << "IR: " << instruction << endl;
        cout << "Memory:" << endl;
        memory.display();
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
                cout << "Enter the file name: ";
                cin >> file_path;
                loadProgramFromFile(file_path);
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
    MachineSimulator simulator(16, 256);
    simulator.run();
    return 0;
}
