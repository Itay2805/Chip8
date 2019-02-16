#include "Opcode.h"

#include "Util.h"

namespace chip8 {

	Opcode::Opcode(uint16_t bin, bool bigEndian) 
		: type(OpcodeType::NONE)
		, op1()
		, op2()
		, op3()
	{
		if (!bigEndian) {
			bin = ToBigEndian(bin);
		}

		if (bin != 0x0000) {
			switch (bin) {
				case 0x00E0: type = OpcodeType::CLS; break;
				case 0x00EE: type = OpcodeType::RET; break;
				default: {
					uint8_t opcodePrefix = bin >> 12;
					switch (opcodePrefix) {
						case 0x1: DisassembleImm(OpcodeType::JP, bin, true); break;
						case 0x2: DisassembleImm(OpcodeType::CALL, bin, true); break;
						case 0x3: DisassembleRegImm(OpcodeType::SE, bin); break;
						case 0x4: DisassembleRegImm(OpcodeType::SNE, bin); break;
						case 0x5: DisassembleRegReg(OpcodeType::SE, bin); break;
						case 0x6: DisassembleRegImm(OpcodeType::LD, bin); break;
						case 0x7: DisassembleRegImm(OpcodeType::ADD, bin); break;
						case 0x8: {
							uint8_t suffix = bin & 0xF;
							switch (suffix) {
								case 0x0: DisassembleRegReg(OpcodeType::LD, bin); break;
								case 0x1: DisassembleRegReg(OpcodeType::OR, bin); break;
								case 0x2: DisassembleRegReg(OpcodeType::AND, bin); break;
								case 0x3: DisassembleRegReg(OpcodeType::XOR, bin); break;
								case 0x4: DisassembleRegReg(OpcodeType::ADD, bin); break;
								case 0x5: DisassembleRegReg(OpcodeType::SUB, bin); break;
								case 0x6: DisassembleReg(OpcodeType::SHR, bin); break;
								case 0x7: DisassembleRegReg(OpcodeType::SUBN, bin); break;
								case 0xe: DisassembleReg(OpcodeType::SHL, bin);  op1 = Operand((Register)((bin >> 8) | 0xF)); break;
								default: {
									throw std::runtime_error("Invalid opcode");
								} break;
							}
						} break;
						case 0x9: DisassembleRegReg(OpcodeType::SNE, bin); break;
						case 0xa: DisassembleImm(OpcodeType::LD, bin); op2 = op1; op1 = Operand(Register::I); break;
						case 0xb: DisassembleImm(OpcodeType::JP_V0, bin); break;
						case 0xc: DisassembleRegImm(OpcodeType::RND, bin); break;
						case 0xd: DisassembleRegRegImm(OpcodeType::DRW, bin); break;
						case 0xe: {
							uint8_t suffix = bin & 0xFF;
							switch (suffix) {
								case 0x9e: DisassembleReg(OpcodeType::SKP, bin); break;
								case 0xa1: DisassembleReg(OpcodeType::SKNP, bin); op1 = Operand((Register)((bin >> 8) | 0xF)); break;
								default: {
									throw std::runtime_error("Invalid opcode");
								}
							}
						} break;
						case 0xf: {
							uint8_t suffix = bin & 0xFF;
							switch (suffix) {
								case 0x07: DisassembleReg(OpcodeType::LD, bin); op2 = Operand(Register::DT); break;
								case 0x15: DisassembleReg(OpcodeType::LD, bin); op2 = op1; op1 = Operand(Register::DT); break;
								case 0x18: DisassembleReg(OpcodeType::LD, bin); op2 = op1; op1 = Operand(Register::ST); break;
								case 0x1e: DisassembleReg(OpcodeType::ADD, bin);  op2 = op1; op1 = Operand(Register::I); break;
								case 0x29: DisassembleReg(OpcodeType::LD_FONT, bin); break;
								case 0x33: DisassembleReg(OpcodeType::LD_BCD, bin); break;
								case 0x55: DisassembleReg(OpcodeType::LD, bin, true); break;
								case 0x65: DisassembleReg(OpcodeType::LD, bin, true); break;
								default: {
									throw std::runtime_error("Invalid opcode");
								}
							}
						} break;
					}
				} break;
			}
		}
	}

	uint16_t Opcode::Assemble(bool binEndian) {
		uint16_t opcode = 0;
		switch (type) {
			case OpcodeType::CLS: opcode = 0x00E0; break;
			case OpcodeType::RET: opcode = 0x00EE; break;
			case OpcodeType::JP: opcode = AssembleImm(0x1); break;
			case OpcodeType::CALL: opcode = AssembleImm(0x2); break;
			case OpcodeType::SE: {
				switch (op2.GetType()) {
					case OperandType::IMMEDIATE: opcode = AssembleRegImm(0x3); break;
					case OperandType::REGISTER: opcode = AssembleRegReg(0x5, 0x0); break;
				}
			} break;
			case OpcodeType::SNE: {
				switch (op2.GetType()) {
					case OperandType::IMMEDIATE: opcode = AssembleRegImm(0x4); break;
					case OperandType::REGISTER: opcode = AssembleRegReg(0x9, 0x0); break;
				}
			}break;
			case OpcodeType::LD: {
				/* We use the LDe for the counters and index register as well */
				Operand op_temp = op1;
				uint16_t asmm = 0;
				switch (op1.AsRegister()) {
					case Register::DT: op1 = op2; opcode = AssembleReg(0xf, 0x15); op1 = op_temp; break;
					case Register::ST: op1 = op2; opcode = AssembleReg(0xf, 0x18); op1 = op_temp; break;
					case Register::I: {
						if (op1.IsMemory()) {
							opcode = AssembleReg(0xf, 0x55);
						}
						else {
							op1 = op2; opcode = AssembleImm(0xa); op1 = op_temp;
						}
					} break;
					default: {
						switch (op2.GetType()) {
							case OperandType::IMMEDIATE: opcode = AssembleRegImm(0x6); break;
							case OperandType::REGISTER: {
								switch (op2.AsRegister()) {
									case Register::DT: opcode = AssembleReg(0xf, 0x7); break;
									case Register::I: {
										if (op2.IsMemory()) {
											opcode = AssembleReg(0xf, 0x65);
										}
										else {
											throw std::runtime_error("Can not load I into register");
										}
									} break;
									default: opcode = AssembleRegReg(0x8, 0x0); break;
								}
							}
						}
					} break;
				};
			} break;
			case OpcodeType::ADD: {
				switch (op1.AsRegister()) {
					case Register::I: opcode = AssembleReg(0xf, 0x1e); break;
					default: {
						switch (op2.GetType()) {
							case OperandType::IMMEDIATE: opcode = AssembleRegImm(0x7); break;
							case OperandType::REGISTER: opcode = AssembleRegReg(0x8, 0x4); break;
						}
					} break;
				}
			} break;
			case OpcodeType::OR: opcode = AssembleRegReg(0x8, 0x1); break;
			case OpcodeType::AND: opcode = AssembleRegReg(0x8, 0x2); break;
			case OpcodeType::XOR: opcode = AssembleRegReg(0x8, 0x3); break;
			case OpcodeType::SUB: opcode = AssembleRegReg(0x8, 0x5); break;
			case OpcodeType::SHR: opcode = AssembleReg(0x8, 0x06); break;
			case OpcodeType::SUBN: opcode = AssembleRegReg(0x8, 0x7); break;
			case OpcodeType::SHL: opcode = AssembleReg(0x8, 0x0e); break;
			case OpcodeType::JP_V0: opcode = AssembleImm(0xb); break;
			case OpcodeType::RND: opcode = AssembleRegImm(0xc); break;
			case OpcodeType::DRW: opcode = AssembleRegRegImm(0xd); break;
			case OpcodeType::SKP: opcode = AssembleReg(0xe, 0x9e); break;
			case OpcodeType::SKNP: opcode = AssembleReg(0xe, 0xa1); break;
			case OpcodeType::LD_FONT: opcode = AssembleReg(0xf, 0x29); break;
			case OpcodeType::LD_BCD: opcode = AssembleReg(0xf, 0x33); break;
		};

		if (binEndian) {
			opcode = ToBigEndian(opcode);
		}

		return opcode;
	}
	
	uint16_t Opcode::AssembleImm(uint8_t opcode) const {
		return ((opcode & 0xF) << 12) | (op1.AsImmediate() & 0xFFF);
	}

	uint16_t Opcode::AssembleReg(uint8_t opcode, uint8_t func) const {
		return ((opcode & 0xF) << 12) | ((uint16_t)(op1.AsRegister(true)) << 8) | (func & 0xFF);
	}

	uint16_t Opcode::AssembleRegImm(uint8_t opcode) const {
		return ((opcode & 0xF) << 12) | ((uint16_t)(op1.AsRegister(true)) << 8) | (op2.AsImmediate() & 0xF);
	}

	uint16_t Opcode::AssembleRegReg(uint8_t opcode, uint8_t func) const {
		return ((opcode & 0xF) << 12) | ((uint16_t)(op1.AsRegister(true)) << 8) | ((uint16_t)(op2.AsRegister(true)) << 4) | (func & 0xF);
	}

	uint16_t Opcode::AssembleRegRegImm(uint8_t opcode) const {
		return ((opcode & 0xF) << 12) | ((uint16_t)(op1.AsRegister(true)) << 8) | ((uint16_t)(op2.AsRegister(true)) << 4) | (op3.AsImmediate() & 0xF);
	}

	void Opcode::DisassembleImm(OpcodeType type, uint16_t opcode, bool addr) {
		this->type = type;
		this->op1 = Operand(opcode & 0x0FFF, false, addr);
	}

	void Opcode::DisassembleReg(OpcodeType type, uint16_t opcode, bool mem) {
		this->type = type;
		this->op1 = Operand((Register)((opcode >> 8) & 0xF), mem);
	}

	void Opcode::DisassembleRegImm(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1 = Operand((Register)((opcode >> 8) & 0xF));
		this->op2 = Operand((opcode >> 4) & 0xFF);
	}

	void Opcode::DisassembleRegReg(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1 = Operand((Register)((opcode >> 8) & 0xF));
		this->op2 = Operand((Register)((opcode >> 4) & 0xF));
	}

	void Opcode::DisassembleRegRegImm(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1 = Operand((Register)((opcode >> 8) & 0xF));
		this->op2 = Operand((Register)((opcode >> 4) & 0xF));
		this->op3 = Operand(opcode & 0xF);
	}

	std::ostream& operator<<(std::ostream& out, OpcodeType op) {
		switch (op) {
			case OpcodeType::CLS: out << "CLS"; break;
			case OpcodeType::RET: out << "RET"; break;
			case OpcodeType::JP_V0:
			case OpcodeType::JP: out << "JP"; break;
			case OpcodeType::CALL: out << "CALL"; break;
			case OpcodeType::SE: out << "SE"; break;
			case OpcodeType::SNE: out << "SNE"; break;
			case OpcodeType::LD_FONT:
			case OpcodeType::LD_BCD:
			case OpcodeType::LD: out << "LD"; break;
			case OpcodeType::ADD: out << "ADD"; break;
			case OpcodeType::OR: out << "OR"; break;
			case OpcodeType::XOR: out << "XOR"; break;
			case OpcodeType::SUB: out << "SUB"; break;
			case OpcodeType::SHR: out << "SHR"; break;
			case OpcodeType::SUBN: out << "SUBN"; break;
			case OpcodeType::SHL: out << "SHL"; break;
			case OpcodeType::RND: out << "RND"; break;
			case OpcodeType::DRW: out << "DRW"; break;
			case OpcodeType::SKP: out << "SKP"; break;
			case OpcodeType::SKNP: out << "SKNP"; break;
			default:
				out << "<Invalid opcode>"; break;
		}
		return out;
	}

	std::ostream& operator<<(std::ostream& out, Opcode op) {
		out << op.type;
		switch (op.type) {
			case OpcodeType::JP: {
				out << op.op1;
			} break;
			case OpcodeType::CALL: {
				out << op.op1;
			} break;
			case OpcodeType::SE: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::SNE: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::LD: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::ADD: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::OR: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::XOR: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::SUB: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::SHR: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::SUBN: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;

			} break;
			case OpcodeType::SHL: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
			} break;
			case OpcodeType::JP_V0: {
				out << Operand(Register::V0);
				std::cout << ", ";
				out << op.op1;
			} break;
			case OpcodeType::RND: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;

			} break;
			case OpcodeType::DRW: {
				out << op.op1;
				std::cout << ", ";
				out << op.op2;
				std::cout << ", ";
				out << op.op3;
			} break;
			case OpcodeType::SKP: {
				out << op.op1;
			} break;
			case OpcodeType::SKNP: {
				out << op.op1;
			} break;
			case OpcodeType::LD_FONT: {
				out << "F";
				out << ", ";
				out << op.op1;
			} break;
			case OpcodeType::LD_BCD: {
				out << "B";
				out << ",";
				out << op.op1;
			} break;
		}
		return out;
	}

}
