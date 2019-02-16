#include <string>

#include <algorithm>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <queue>

#include <Opcode.h>

using namespace chip8;

static bool show_address = true;
static bool show_color = false;
static bool show_bytecode = false;

static bool ShouldContinue(const Opcode& opcode) {
	switch (opcode.Type()) {
		case OpcodeType::JP:
		case OpcodeType::JP_I:
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
	setColor(0xC65440);
	switch (reg) {
		case Register::V0: std::cout << "V0"; break;
		case Register::V1: std::cout << "V1"; break;
		case Register::V2: std::cout << "V2"; break;
		case Register::V3: std::cout << "V3"; break;
		case Register::V4: std::cout << "V4"; break;
		case Register::V5: std::cout << "V5"; break;
		case Register::V6: std::cout << "V6"; break;
		case Register::V7: std::cout << "V7"; break;
		case Register::V8: std::cout << "V8"; break;
		case Register::V9: std::cout << "V9"; break;
		case Register::VA: std::cout << "VA"; break;
		case Register::VB: std::cout << "VB"; break;
		case Register::VC: std::cout << "VC"; break;
		case Register::VD: std::cout << "VD"; break;
		case Register::VE: std::cout << "VE"; break;
		case Register::VF: std::cout << "VF"; break;
		case Register::I:  std::cout << "I"; break;
		case Register::DT: std::cout << "DT"; break;
		case Register::ST: std::cout << "ST"; break;
		case Register::PC: std::cout << "<PC>"; break;
		case Register::SP: std::cout << "<SP>"; break;
		default: setColor(0xFF0000);  std::cout << "<invalid-reg>"; break;
	}
}

static void PrintOperand(const Operand& operand, bool hex = false) {
	switch (operand.GetType()) {
		case OperandType::IMMEDIATE: {
			if (hex) {
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
}

// TODO: This has alot of code duplication
static void PrintOpcode(const Opcode& opcode) {
	setColor(0x4481B8);
	switch (opcode.Type()) {
		case OpcodeType::CLS: std::cout << "CLS" << std::endl; break;
		case OpcodeType::RET: std::cout << "RET" << std::endl; break;
		case OpcodeType::JP: {
			std::cout << "JP ";
			PrintOperand(opcode.Operand1(), true);
			std::cout << std::endl;
		} break;
		case OpcodeType::CALL: {
			std::cout << "CALL ";
			PrintOperand(opcode.Operand1(), true);
			std::cout << std::endl;
		} break;
		case OpcodeType::SE: {
			std::cout << "SE ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::SNE: {
			std::cout << "SNE ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD: {
			std::cout << "LD ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::ADD: {
			std::cout << "ADD ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::OR: {
			std::cout << "OR ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::XOR: {
			std::cout << "XOR ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::SUB: {
			std::cout << "SUB ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::SHR: {
			std::cout << "SHR ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::SUBN: {
			std::cout << "SUBN ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::SHL: {
			std::cout << "SHL ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::JP_I: {
			std::cout << "JP ";
			PrintRegister(Register::V0);
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::RND: {
			std::cout << "RND ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			std::cout << std::endl;
		} break;
		case OpcodeType::DRW: {
			std::cout << "DRW ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand2());
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand3());
			std::cout << std::endl;
		} break;
		case OpcodeType::SKP: {
			std::cout << "SKP ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::SKNP: {
			std::cout << "SKNP ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD_FONT: {
			std::cout << "LD ";
			setColor(0xCA9F52);
			std::cout << "F";
			resetColor();
			std::cout << ", ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD_MEM_REG: {
			std::cout << "LD ";
			resetColor();
			std::cout << "[";
			PrintRegister(Register::I);
			resetColor();
			std::cout << "], ";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		case OpcodeType::LD_REG_MEM: {
			std::cout << "LD ";
			PrintOperand(opcode.Operand1());
			resetColor();
			std::cout << ", [";
			PrintRegister(Register::I);
			resetColor();
			std::cout << "]";
			std::cout << std::endl;
		} break;
		case OpcodeType::BCD: {
			std::cout << "LD ";
			setColor(0xCA9F52);
			std::cout << "B";
			resetColor();
			std::cout << ",";
			PrintOperand(opcode.Operand1());
			std::cout << std::endl;
		} break;
		default: std::cout << "Invalid opcode" << std::endl;
	}
}

static void PrintOpcodeBytes(uint16_t opcode) {
	if (!show_bytecode) {
		return;
	}

	// little endian, we need to swap
	int n = 1;
	if (*(char *)&n == 1) {
		opcode = _byteswap_ushort(opcode);
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
				else if (argv[i], "-bytecode") {
					show_bytecode = true;
				}
				else if (argv[i], "-no-address") {
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
			std::queue<int> addresses;
			addresses.push(0x200);
			while (!addresses.empty()) {
				int address = addresses.front();
				addresses.pop();
				if (address < 0x200) continue;
				if (std::find(checked.begin(), checked.end(), address) != checked.end()) continue;
				resetColor();
				std::cout << "<";
				setColor(0xBD8EBD);
				std::cout << std::setfill('0') << std::setw(3) << std::hex << address;
				resetColor();
				std::cout << ">:" << std::endl;
				checked.push_back(address);
				Opcode opcode(0);
				do {
					if (show_address) {
						setColor(0xBD8EBD);
						std::cout << std::setfill('0') << std::setw(3) << std::hex << address << "\t";
					}
					uint16_t opcode_byte = *(uint16_t*)&memory[address - 0x200];
					PrintOpcodeBytes(opcode_byte);
					try {
						opcode = Opcode(opcode_byte);
					}
					catch (...) {
						opcode = Opcode(0);
					}

					PrintOpcode(opcode);

					// catch up on flow control operators
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
						case OpcodeType::SNE: {
							addresses.push(address + 2);
						} break;
						case OpcodeType::SE: {
							addresses.push(address + 2);
						} break;
					}

					address += 2;
				} while (ShouldContinue(opcode));

				std::cout << std::endl;
				std::cout << std::endl;
			}
		}
		else {
			std::cout << "Failed to read input file" << std::endl;
		}
	}
}
