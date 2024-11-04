#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <bitset>
using namespace std;

class Memory {
private:
    vector<string> memory;

public:
    Memory(int Size) : memory(Size, "00") {}

    void store(int address, const string& value) {
        if (address >= 0 && address < 256) {
            memory[address] = value.substr(0, 2);
            if (value.length() == 4)
            memory[address + 1] = value.substr(2, 2);
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

            instruction = memory[address] + memory[address + 1];
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
        if (text.length() == 0)
            return "";

        size_t start = 0;
        while (start < text.size() && text[start] == ' ') {
            start++;
        }

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

                line = Trim(line);
                line += ' ';
                while (line.length() >= 4)
                {
                    line[0] = toupper(line[0]);
                    Instructions.push_back(line.substr(0, line.find_first_of(' ')));
                    line = line.erase(0, line.find_first_of(' ') + 1);
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
    string hexToBinary(string& hex) {
        string binary;

        if (hex[0] == '0')
            hex = hex.erase(0, 1);

        for (char hexDigit : hex) {
            int decimalValue = 0;

            if (hexDigit >= '0' && hexDigit <= '9') {
                decimalValue = hexDigit - '0';
            }
            else if (hexDigit >= 'A' && hexDigit <= 'F') {
                decimalValue = hexDigit - 'A' + 10;
            }
            else if (hexDigit >= 'a' && hexDigit <= 'f') {
                decimalValue = hexDigit - 'a' + 10;
            }
            binary += std::bitset<4>(decimalValue).to_string();
        }

        return binary;
    }
    string binaryToHex(const string& binaryStr)
    {
        string paddedBinary = binaryStr;
        while (paddedBinary.length() % 4 != 0) {
            paddedBinary = "0" + paddedBinary;
        }

        stringstream hexStream;
        for (size_t i = 0; i < paddedBinary.length(); i += 4) {
            std::string fourBits = paddedBinary.substr(i, 4);
            int decimalValue = std::bitset<4>(fourBits).to_ulong();
            hexStream << std::hex << decimalValue;
        }
        string hexResult = hexStream.str();
        for (char& c : hexResult) c = toupper(c);
        return hexResult;
    }
    void Rotate(const string& instr) {
        int r = instr[1] - '0';
        string no_rotate = instr.substr(2, 2);
        string value = registers.get(r);


        value = hexToBinary(value);
        for (size_t i = 0; i < stoi(no_rotate,nullptr,16); i++)
        {
            char c = value[value.length() - 1];
            value = value.insert(0, 1, c);
            value = value.erase(value.length() - 1, 1);
        }

        value = binaryToHex(value);

        if (value.length() == 1)
            value = value.insert(0, 1, '0');

        registers.set(r, value);
    }
    string IntegerToHexa(int value)
    {
        stringstream result;
        result << hex << value;
        string HexaResult = result.str();

        for (char& c : HexaResult) c = toupper(c);

        return HexaResult;
    }
    void sum_towscomplement(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        if (valueS > 127) valueS -= 256;
        int valueT = stoi(registers.get(t), nullptr, 16);
        if (valueT > 127) valueT -= 256;
        int result = valueS + valueT;
        if (result < -128) result += 256;
        if (result > 127) result -= 256;
        result = (result + 256) % 256;
        string string_result = IntegerToHexa(result);
        registers.set(r, ((string_result.length() == 1) ? "0" : "") + string_result);
    }

    bitset<4> binarydouble(double num) {
        int count = 0;
        //a while loop to extract the binary of the floating point number
        while (num > .025) {
            num -= .025;
            count += 1;
        }
        //covers up for the loss of data due to floating pint
        while (count % 10 != 0) {
            count++;
        }
        count /= 10;
        bitset<4> ret = count;
        return ret;
    }
    string sumdouble(string Dfir, string Dsec) {
        bitset<8> Df = stoi(Dfir, nullptr, 16);
        int sign1 = (Df[7]) ? -1 : 1;
        string moved1 = Df.to_string();
        int move1 = stoi(moved1.substr(1, 3), nullptr, 2) - 4;
        double mantissa1 = 0.0;
        //calculates the mantissa of the first number
        for (int i = 0; i < 4; i++)
            if (moved1[4 + i] == '1')
                mantissa1 += pow(2, -(i + 1));
        double value1 = sign1 * mantissa1 * pow(2, move1);
        bitset<8> Ds = stoul(Dsec, nullptr, 16);
        int sign2 = (Ds[7]) ? -1 : 1;
        string moved2 = Ds.to_string();
        int move2 = stoi(moved2.substr(1, 3), nullptr, 2) - 4;
        double mantissa2 = 0.0;
        //the mantissa of the second number
        for (int i = 0; i < 4; i++)
            if (moved2[4 + i] == '1')
                mantissa2 += pow(2, -(i + 1));
        double value2 = sign2 * mantissa2 * pow(2, move2);
        double result = value1 + value2;
        //after the last result is calculated 
        //the number is returned to being a two hexdecimal digits
        string sign3 = "0";
        string final;
        if (result < 0) {
            sign3 = "1";
            result *= -1;
        }
        if (result < 1) {
            bitset<4> res = binarydouble(result);
            int i = 0;
            while (res[3] != 1) {
                res = res << 1;
                i++;
            }
            bitset<3> idi = i;
            final = sign3 + idi.to_string() + res.to_string();
        }
        else {
            //splites the number into two 
            double fraction = result - floor(result);   //the floating fraction 
            double importnum = result - fraction;       //and the number
            bitset<4> res2 = binarydouble(fraction);    //turn the fraction into binary using binarydouble function
            bitset<4> res1 = importnum;
            int index = 0;
            int j = 3;
            while (res1[j] != 1) {
                index++;
                j--;
            }
            index = -(index - 4); 
            index += 4;                    // to get the exponent
            while (res1[3] != 1)
                res1 = res1 << 1;
            for (int i = 3; i > 0; i--) {
                if (res2[3] == 1)
                    break;                    
                res2 = res2 << 1;            
            }
            int i = 0;
            while (res1[i] != 1) {
                if (res2[3] == 1) {
                    res1[i] = 1;        // putes the two numbers together again but in the mantissa binary form
                }
                res2 = res2 << 1;
                i++;
            }
            bitset<3> idi = index;
            final = sign3 + idi.to_string() + res1.to_string(); //final result number
        }
        int finaly = bitset<32>(final).to_ulong();
        stringstream ss;
        ss << uppercase << hex << finaly;            //finaly returns the number in hexdecimal form
        return ss.str();
    }

    void sum_floating(string instr)
    {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        string ValueS = registers.get(s);
        string ValueT = registers.get(t);
        string result = sumdouble(ValueS, ValueT);
        registers.set(r, result);
    }

    void AND(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_AND = (valueS & valueT);
        if (result_AND > 255) result_AND = 255;
        string string_result = IntegerToHexa(result_AND);
        registers.set(r, ((string_result.length() == 1) ? "0" : "") + string_result);
    }

    void XOR(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_XOR = (valueS ^ valueT);
        if (result_XOR > 255) result_XOR = 255;
        string string_result = IntegerToHexa(result_XOR);
        registers.set(r, ((string_result.length() ^ 1) ? "0" : "") + string_result);
    }

    void OR(const string& instr) {
        int r = instr[1] - '0';
        int s = instr[2] - '0';
        int t = instr[3] - '0';
        int valueS = stoi(registers.get(s), nullptr, 16);
        int valueT = stoi(registers.get(t), nullptr, 16);
        int result_OR = (valueS | valueT);
        if (result_OR > 255) result_OR = 255;
        string string_result = IntegerToHexa(result_OR);
        registers.set(r, ((string_result.length() | 1) ? "0" : "") + string_result);
    }

    void Jump(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        if (registers.get(r) == registers.get(0)) {
            pc = stoi(address, nullptr, 16)-2;
            return;
        }
    }
    void Jump_UP(const string& instr) {
        int r = instr[1] - '0';
        string address = instr.substr(2, 2);
        if (registers.get(r) > registers.get(0)) {
            pc = stoi(address, nullptr, 16)-2;
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
            pc += 2;
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
            cu.sum_towscomplement(instr);
            break;
        case '6':
            cu.sum_floating(instr);
            break;
        case '7':
            cu.OR(instr);
            break;
        case '8':
            cu.AND(instr);
            break;
        case '9':
            cu.XOR(instr);
            break;
        case 'B':
            cu.Jump(instr);
            break;
        case 'D':
            cu.Jump_UP(instr);
            break;
        case 'A':
            cu.Rotate(instr);
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
                memory.load_to_memory(Instructions.GetInstructions());
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
    MachineSimulator simulator(16, 256);
    simulator.run();
    return 0;
}
