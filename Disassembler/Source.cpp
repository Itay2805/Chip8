#include <string>

#include <algorithm>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <stack>

#include <Opcode.h>
#include <Util.h>

using namespace chip8;

static bool show_address = true;
static bool show_color = false;
static bool show_bytecode = false;

static bool ShouldContinue(const Opcode& prev, const Opcode& opcode) {
	if (prev.Type() == OpcodeType::SE || prev.Type() == OpcodeType::SNE) {
		return true;
	}
	switch (opcode.Type()) {
		case OpcodeType::JP:
		case OpcodeType::JP_V0:
		case OpcodeType::RET:
			return false;
		default:
			return true;
	}
}

static void setColor(uint32_t color) {
	if (!show_color) return;
	std::cout << std::dec << "\x1b[38;2;" << ((color >> 16) & 0xFF) << ";" << ((color >> 8) & 0xFF) << ";" << (color & 0xFF) << "m";
}

static void resetColor() {
	if (!show_color) return;
	std::cout << std::dec << "\x1b[0m";
}

static void PrintRegister(Register reg) {
	if (reg < Register::PC) {
		setColor(0xC65440);
	}
	else {
		setColor(0xFF0000);		
	}
	std::cout << reg;
}

static void PrintOperand(const Operand& operand) {
	if (operand.IsMemory()) {
		resetColor();
		std::cout << "[";
	}
	switch (operand.GetType()) {
		case OperandType::IMMEDIATE: {
			if (operand.IsAddress()) {
				setColor(0xBD8EBD);
				std::cout << std::setfill('0') << std::setw(3) << std::hex << operand.AsImmediate();
			}
			else {
				setColor(0xE5743A);
				std::cout << std::dec << operand.AsImmediate();
			}
		} break;
		case OperandType::REGISTER: {
			PrintRegister(operand.AsRegister());
		} break;
	};
	if (operand.IsMemory()) {
		resetColor();
		std::cout << "]";
	}
}

// TODO: This has alot of code duplication
static void PrintOpcode(const Opcode& opcode) {
	switch (opcode.Type()) {
		case OpcodeType::CLS:
		case OpcodeType::RET:	
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			break;
		case OpcodeType::SYS:
		case OpcodeType::CALL:
		case OpcodeType::JP:
		case OpcodeType::SKP: 
		case OpcodeType::SKNP: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::SE:
		case OpcodeType::SNE:
		case OpcodeType::LD:
		case OpcodeType::ADD:
		case OpcodeType::OR:
		case OpcodeType::AND:
		case OpcodeType::XOR:
		case OpcodeType::SUB: 
		case OpcodeType::SHR: 
		case OpcodeType::SUBN: 
		case OpcodeType::SHL:
		case OpcodeType::RND: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::JP_V0: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			PrintRegister(Register::V0);
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::DRW: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand3());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD_FONT: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			setColor(0xCA9F52);
			std::cout << "F";
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD_BCD: {
			setColor(0x4481B8);
			std::cout << opcode.Type() << " ";
			setColor(0xCA9F52);
			std::cout << "B";
			resetColor();
			std::cout << ",";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		default: {
			setColor(0xFF0000);
			std::cout << "<Missing opcode print for " << opcode.Type() << ">" << std::endl;
		}
	}
}

static void PrintOpcodeBytes(uint16_t opcode) {
	if (!show_bytecode) {
		return;
	}

	setColor(0x99C792);
	std::cout << std::setfill('0') << std::setw(2) << std::hex << (opcode >> 8) << " " << std::setfill('0') << std::setw(2) << std::hex << (opcode & 0xFF) << "\t";
}

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage " << argv[0] << " <input file>" << std::endl;
	}
	else {

		if (argc > 3) {
			for (int i = 2; i < argc; i++) {
				if (strcmp(argv[i], "-color") == 0) {
					show_color = true;
				}
				else if (strcmp(argv[i], "-bytecode") == 0) {
					show_bytecode = true;
				}
				else if (strcmp(argv[i], "-no-address") == 0) {
					show_address = false;
				}
			}
		}

		std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> memory(size);

		if (file.read(memory.data(), size))
		{
			std::vector<int> checked;
			std::stack<int> addresses;
			addresses.push(0x200);
			while (!addresses.empty()) {
				int address = addresses.top();
				addresses.pop();

				if (address < 0x200) continue;
				
				if (std::find(checked.begin(), checked.end(), address) != checked.end()) continue;

				resetColor();
				std::cout << "<";
				setColor(0xBD8EBD);
				std::cout << std::setfill('0') << std::setw(3) << std::hex << address;
				resetColor();
				if (address % 2 != 0) {
					std::cout << " [unaligned]";
				}
				std::cout << ">:" << std::endl;

				Opcode prev(0);
				Opcode opcode(0);
				do {
					checked.push_back(address);
					bool over = false;

					if (show_address) {
						setColor(0xBD8EBD);
						std::cout << std::setfill('0') << std::setw(3) << std::hex << address << "\t";
					}
					
					if (address > (memory.size() - 2) + 0x200) {
						setColor(0xFF0000);
						std::cout << "<Outside of range>";
						break;
					}

					uint16_t opcode_byte = *(uint16_t*)&memory[address - 0x200];
					opcode_byte = ToBigEndian(opcode_byte);
					PrintOpcodeBytes(opcode_byte);

					prev = opcode;
					try {
						opcode = Opcode(opcode_byte, true);
					}
					catch (std::runtime_error err) {
						setColor(0xFF0000);
						std::cout << "<" << err.what() << ">" << std::endl;
						opcode = Opcode(0);
					}
					if (opcode.Type() != OpcodeType::NONE) {
						PrintOpcode(opcode);
					}

					// we want to checkout every place we jump into
					switch (opcode.Type()) {
						case OpcodeType::JP: {
							if (opcode.Operand1().GetType() == OperandType::IMMEDIATE) {
								addresses.push(opcode.Operand1().AsImmediate());
							}
						} break;
						case OpcodeType::CALL: {
							if (opcode.Operand1().GetType() == OperandType::IMMEDIATE) {
								addresses.push(opcode.Operand1().AsImmediate());
							}
						} break;
					}

					address += 2;
				} while (ShouldContinue(prev, opcode));

				std::cout << std::endl;
				std::cout << std::endl;
			}
		}
		else {
			std::cout << "Failed to read input file" << std::endl;
		}
	}
}
