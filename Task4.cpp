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
            memory[address+1] = value.substr(2, 2);
        }
    }
    void load_to_memory(vector <string> Instructions) {
        int address = 0;
        for (const auto& instr : Instructions) {
            if (instr.size() == 4 && all_of(instr.begin(), instr.end(), ::isxdigit)) {
                store(address, instr);
                address += 2;
            }
        }

    }
    
    string load(int address) {
        string instruction;
        if (address >= 0 && address < 256) {

            instruction= memory[address]+ memory[address+1];
            return instruction;
        }
        return "0000";
    }

    void display() {
        int counter = 0;
        for (int i = 0; i < 256; ++i) {
            
            cout << setw(2) << setfill('0') << memory[i] << " ";
                counter++;
                if (counter == 16)
                {
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
     vector<string> Instructions;

public:
    string Trim(string text) {
        // Remove spaces at the beginning
        if (text.length() == 0)
            return "";

        size_t start = 0;
        while (start < text.size() && text[start] == ' ') {
            start++;
        }

        // Remove spaces at the end
        size_t end = text.size() - 1;
        while (end > start && text[end] == ' ') {
            end--;
        }

        return text.substr(start, end - start + 1);
    }

    vector<string> loadFromFile(const string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            cout << "Error: Unable to open file." << endl;
            return {};
        }

        string line;
        Instructions.clear();
        while (getline(file, line)) {
            if (!line.empty()) {

                line= Trim(line);
                line += ' ';
                while (line.length() >=4)
                {
                    line[0] = toupper(line[0]);
                    Instructions.push_back(line.substr(0,line.find_first_of(' ')));
                    line=line.erase(0, line.find_first_of(' ')+1);
                    line = Trim(line);
                    line += ' ';
                }
                }
        }
        if (Instructions[Instructions.size() - 1] != "C000")
            Instructions.push_back("C000");

        if (Instructions.empty()) {
            cout << "Error: Program is empty." << endl;
        }

        return Instructions;
    }

     vector<string> GetInstructions() {
        return Instructions;
    }
};

class CU {
private:
    Memory& memory;
    Register& registers;
    int& pc;

public:
    CU(Memory& mem, Register& regs, int& programCounter)
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
        if (valueS > 127)
            valueS -= 256;
        int valueT = stoi(registers.get(t), nullptr, 16);
        if (valueT > 127)
            valueT -= 256;
        int result = (valueS + valueT);
        if (result < 0)
            valueT += 256;
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

    void Jump(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        if (registers.get(r) == registers.get(0)) {
            pc = stoi(address, nullptr, 16);
            return;
        }
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
    CU cu;
public:
    MachineSimulator(int regSize, int memorySize)
         : halted(false), pc(0), instruction("0000"), registers(regSize), memory(memorySize),
        Instructions(), cu(memory, registers, pc) {}


    void load(const vector<string>& program) {
        int address = 0;
        for (const auto& line : program) {
            memory.store(address, line);
            address += 2;
        }
    }

    void execute() {
        while (!halted) {
            instruction = memory.load(pc);
            if (instruction.empty()) {
                cout << "Error: No instruction at PC = " << pc << endl;
                break;
            }
            executeInstruction(instruction);
            pc+=2;
        }
    }

    void executeInstruction(const string& instr) {
        char opCode = toupper(instr[0]);

        switch (opCode) {
        case '1':
            cu.Address(instr);
            break;
        case '2':
            cu.Record_value(instr);
            break;
        case '3':
            cu.cope(instr);
            break;
        case '4':
            cu.Move(instr);
            break;
        case '5':
            cu.Add(instr);
            break;
        case '6':
            cu.Sum(instr);
            break;

        case 'B':
            cu.Jump(instr);
            break;
        case 'C':
            halted = true;
            break;
        default:
            cout << "Error: Unknown instruction " << instr << endl;
        }
    }

    
    void display_state() {
        cout << "Registers:" << endl;
        registers.display();
        cout << "PC: " << pc << endl;
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
            string choice = "";

            cin >> choice;


            while (true)
            {

                try
                {
                    while (true)
                    {
                        if (choice != "1" && choice != "2" && choice != "3" && choice != "4")
                        {
                            cout << "Enter a valid choice: ";
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            cin >> choice;
                        }
                        else break;
                    }

                }
                catch (const std::exception&)
                {
                    continue;
                }
                break;
            }

            char c = choice[0];
            switch (c) {
            case '1': {
                string file_path;
                cout << "file name: ";
                cin >> file_path;
                Instructions.loadFromFile(file_path);
                memory.load_to_memory(Instructions.GetInstructions());// store all instructions in memory
                break;
            }
            case '2':
                execute();
                break;
            case '3':
                display_state();
                break;
            case '4':
                return;
            default:
                break;
            }
        }
    }
};

int main() {
    MachineSimulator simulator(16,256);
    simulator.run();
    return 0;
}
