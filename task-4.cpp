#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

class Memory {
private:
    vector<string> memory;

public:
    Memory(int Size) : memory(Size, "00") {}

    void store(int address, const string& value) {
        if (address >= 0 && address < 256) {
            memory[address] = value.substr(0, 2);
            if (value.length() > 2) {
                memory[address + 1] = value.substr(2, 2);
            }
            else {
                memory[address + 1] = "00";
            }
        }
    }

    string load(int address) {
        string instruction;
        if (address >= 0 && address < 256) {
            instruction = memory[address] + memory[address + 1];
            return instruction;
        }
        if (address < 0 && address >= 256) {
            return "00";
        }
        return "0000";
    }

    void display() {
        int counter = 0;
        for (int i = 0; i < 256; ++i) {
            cout << setw(2) << setfill('0') << memory[i] << " ";
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
    vector<string> loadfile(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Unable to open file." << endl;
            return {};
        }
        string line;
        program.clear();
        while (getline(file, line)) {
            istringstream iss(line);
            string instruction;
            while (iss >> instruction) {
                if (instruction.size() == 4 && all_of(instruction.begin(), instruction.end(), ::isxdigit)) {
                    program.push_back(instruction);
                }
                else {
                    cout << "Invalid instruction: " << instruction << endl;
                }
            }
        }
        if (program.empty()) {
            cout << "Program is empty." << endl;
        }
        return program;
    }

    void load_to_memory(Memory& memory) {
        int address = 0;
        for (const auto& instr : program) {
            if (instr.size() == 4 && all_of(instr.begin(), instr.end(), ::isxdigit)) {
                memory.store(address, instr);
                address += 2;
            }
            else {
                cout << "Ignoring: " << instr << endl;
            }
        }
        if (find(program.begin(), program.end(), "C000") == program.end()) {
            program.push_back("C000");
            memory.store(address, "C000");
        }
    }

    void cleanlineloop(const string& file_path) {
        string line;
        ifstream file(file_path);
        while (getline(file, line)) {
            cleanline(line, string("Work.txt"));
        }
    }

    void cleanline(const string& line, string outs1) {
        ofstream outs(outs1);
        vector<char> import;
        int count = 0;
        vector<char> nimport;
        for (char ch : line) {
            if ((!(import.size() == 4)) && count != 0) {
                return;
            }
            if (isspace(ch) && import.size() != 0)
                count++;
            if (!isspace(ch)) {
        import.push_back(ch);
                if (import.size() > 4 && count != 0) {
                    count = 0;
                    for (size_t i = 0; i < import.size() - 1; ++i) {
                        nimport.push_back(toupper(import[i]));
                    }
        import.erase(import.begin(), import.end() - 1);
                }
            }
        }
        if (import.size() == 4)
            for (size_t i = 0; i < import.size(); ++i)
                nimport.push_back(toupper(import[i]));
        for (size_t i = 0; i < nimport.size();) {
            if (nimport[i] == 'C')
                if (nimport[i + 1] == '0' && nimport[i + 2] == '0' && nimport[3] == '0') {
                    for (; i < nimport.size(); i++)
                        outs << nimport[i];
                    return;
                }
                else {
                    return;
                }
            if (!isValidnotFirstChar(nimport[i])) {
                if ((!isvalidnotrestchars(nimport[i + 1])) && (!isvalidnotrestchars(nimport[i + 2])) &&
                    (!isvalidnotrestchars(nimport[i + 3]))) {
                    for (; i < 4; i++)
                        outs << nimport[i];
                    i -= 4;
                    outs << endl;
                }
                else
                    return;
            }
            else
                return;
            nimport.erase(nimport.begin(), nimport.begin() + 4);
        }
    }

    vector<string> getInstructions() {
        return program;
    }

    bool isValidnotFirstChar(char ch) {
        return isxdigit(ch);
    }

    bool isvalidnotrestchars(char ch) {
        return isxdigit(ch);
    }
};


class CPU {
private:
    Memory& memory;
    Register& registers;
    int& pc;

public:
    CPU(Memory& mem, Register& regs, int& programCounter)
        : memory(mem), registers(regs), pc(programCounter) {}

    void Address(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        int addr = stoi(address, nullptr, 16);
        if (addr < 0 || addr >= 256) {
            registers.set(r, "00");
            return;
        }
        string value = memory.load(addr);
        if (value.empty()) {
            registers.set(r, "00");
            return;
        }
        registers.set(r, value.substr(2, 2));
    }

    void Record_value(const string& instr) {
        int r = instr[1] - '0';
        string value = instr.substr(2, 2);
        if (value == "00") {
            registers.set(r, "00");
        }
        else {
            registers.set(r, value);
        }
    }

    void cope(const string& instr) {
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
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result = valueS + valueT;
        if (result > 255) result = 255;
        registers.set(r, (result < 16 ? "0" : "") + to_string(result));
    }

    void Sum(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        string valueS = registers.get(s);
        string valueT = registers.get(t);
        int result = stoi(valueS, nullptr, 16) + stoi(valueT, nullptr, 16);
        if (result > 255) result = 255;
        registers.set(r, (result < 16 ? "0" : "") + to_string(result));
    }

    void AND(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_AND = valueS & valueT;
        if (result_AND > 255) result_AND = 255;
        registers.set(r, (result_AND < 16 ? "0" : "") + to_string(result_AND));
    }

    void XOR(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_XOR = (valueS ^ valueT);
        if (result_XOR > 255) result_XOR = 255;
        registers.set(r, (result_XOR < 16 ? "0" : "") + to_string(result_XOR));
    }

    void OR(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_OR = (valueS | valueT);
        if (result_OR > 255) result_OR = 255;
        registers.set(r, (result_OR < 16 ? "0" : "") + to_string(result_OR));
}

    void Jump(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        if (registers.get(r) == registers.get(0)) {
            pc = stoi(address, nullptr, 16);
            return;
        }
    }

    void Greater(const string& instr) {
    int r = instr[1] - '0';
    string address = instr.substr(2, 2);
    if (registers.get(r) > registers.get(0)) {
        pc = stoi(address, nullptr, 16);
        return;
        }
    }
};

class Machine {
private:
    bool halted;
    int pc;
    string instruction;
    Memory memory;
    Register registers;
    Loader loader;
    CPU cpu;

public:
    Machine(int regSize, int memorySize)
        : halted(false), pc(0), instruction("0000"), registers(regSize), memory(memorySize),
        loader(), cpu(memory, registers, pc) {}

    void load_program_file(const string& filePath) {
        loader.loadfile(filePath);
        loader.load_to_memory(memory);
    }

    void execute() {
        while (!halted) {
            instruction = memory.load(pc);
            cout << "PC: " << (pc / 2) << " | Instruction: " << instruction << endl;
            if (instruction.empty()) {
                cout << "No instruction at PC = " << pc << endl;
                break;
            }
            execute_instruction(instruction);
            pc += 2;
        }
    }

    void execute_instruction(const string& instr) {
        char opCode = toupper(instr[0]);
        switch (opCode) {
        case '1':
            cpu.Address(instr);
            break;
        case '2':
            cpu.Record_value(instr);
            break;
        case '3':
            cpu.cope(instr);
            break;
        case '4':
            cpu.Move(instr);
            break;
        case '5':
            cpu.Add(instr);
            break;
        case '6':
            cpu.Sum(instr);
            break;
        case '7':
            cpu.OR(instr);
            break;
        case '8':
            cpu.AND(instr);
            break;
        case '9':
            cpu.XOR(instr);
            break;
        case 'B':
            cpu.Jump(instr);
            break;
        case 'D':
            cpu.Greater(instr);
            break;
        case 'C':
            halted = true;
            break;
        default:
            cout << "error instruction " << instr << endl;
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
            cout << "1-Load file" << endl;
            cout << "2-Execute" << endl;
            cout << "3-Display" << endl;
            cout << "4-Exit" << endl;
            cout << "Choose: ";
            int choice;
            cin >> choice;
            switch (choice) {
            case 1: {
                string file_path;
                cout << "file name: ";
                cin >> file_path;
                load_program_file(file_path);
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
                cout << "Invalid choice." << endl;
            }
        }
    }
};

int main() {
    Machine machine(16, 256);
    machine.run();
    return 0;
}
