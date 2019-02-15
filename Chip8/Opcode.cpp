#include "Opcode.h"

namespace chip8 {

	Opcode::Opcode(uint16_t bin) {
		if (bin != 0x0000) {

			// little endian, we need to swap
			int n = 1;
			if (*(char *)&n == 1) {
				bin = _byteswap_ushort(bin);
			}

			switch (bin) {
				case 0x00E0: type = OpcodeType::CLS; break;
				case 0x00EE: type = OpcodeType::RET; break;
				default: {
					uint8_t opcodePrefix = bin >> 12;
					switch (opcodePrefix) {
						case 0x1: DisassembleImm(OpcodeType::JP, bin); break;
						case 0x2: DisassembleImm(OpcodeType::CALL, bin); break;
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
								case 0xe: DisassembleReg(OpcodeType::SHL, bin);  op1.Set((Register)((bin >> 8) | 0xF)); break;
								default: {
									throw std::exception("Invalid opcode");
								} break;
							}
						} break;
						case 0x9: DisassembleRegReg(OpcodeType::SNE, bin); break;
						case 0xa: DisassembleImm(OpcodeType::LD, bin); op2 = op1; op1.Set(Register::I); break;
						case 0xb: DisassembleImm(OpcodeType::JP_I, bin); break;
						case 0xc: DisassembleRegImm(OpcodeType::RND, bin); break;
						case 0xd: DisassembleRegRegImm(OpcodeType::DRW, bin); break;
						case 0xe: {
							uint8_t suffix = bin & 0xFF;
							switch (suffix) {
								case 0x9e: DisassembleReg(OpcodeType::SKP, bin); break;
								case 0xa1: DisassembleReg(OpcodeType::SKNP, bin);  op1.Set((Register)((bin >> 8) | 0xF)); break;
								default: {
									throw std::exception("Invalid opcode");
								}
							}
						} break;
						case 0xf: {
							uint8_t suffix = bin & 0xFF;
							switch (suffix) {
								case 0x07: DisassembleReg(OpcodeType::LD, bin); op2.Set(Register::DT); break;
								case 0x15: DisassembleReg(OpcodeType::LD, bin); op2 = op1; op1.Set(Register::DT); break;
								case 0x18: DisassembleReg(OpcodeType::LD, bin); op2 = op1; op1.Set(Register::ST); break;
								case 0x1e: DisassembleReg(OpcodeType::ADD, bin);  op2 = op1; op1.Set(Register::I); break;
								case 0x29: DisassembleReg(OpcodeType::LD_FONT, bin); break;
								case 0x33: DisassembleReg(OpcodeType::BCD, bin); break;
								case 0x55: DisassembleReg(OpcodeType::LD_MEM_REG, bin); break;
								case 0x65: DisassembleReg(OpcodeType::LD_REG_MEM, bin); break;
								default: {
									throw std::exception("Invalid opcode");
								}
							}
						} break;
					}
				} break;
			}
		}
	}

	uint16_t Opcode::Assemble() {
		switch (type) {
			case OpcodeType::CLS: return 0x00E0;
			case OpcodeType::RET: return 0x00EE;
			case OpcodeType::JP: return AssembleImm(0x1);
			case OpcodeType::CALL: return AssembleImm(0x2);
			case OpcodeType::SE: {
				switch (op2.GetType()) {
					case OperandType::IMMEDIATE: return AssembleRegImm(0x3);
					case OperandType::REGISTER: return AssembleRegReg(0x5, 0x0);
				}
			} break;
			case OpcodeType::SNE: {
				switch (op2.GetType()) {
					case OperandType::IMMEDIATE: return AssembleRegImm(0x4);
					case OperandType::REGISTER: return AssembleRegReg(0x9, 0x0);
				}
			}break;
			case OpcodeType::LD: {
				/* We use the LDe for the counters and index register as well */
				Operand op_temp = op1;
				uint16_t asmm = 0;
				switch (op1.AsRegister()) {
					case Register::DT: op1 = op2; asmm = AssembleReg(0xf, 0x15); op1 = op_temp; return asmm;
					case Register::ST: op1 = op2; asmm = AssembleReg(0xf, 0x18); op1 = op_temp; return asmm;
					case Register::I: op1 = op2; asmm = AssembleImm(0xa); op1 = op_temp; return asmm;
					default: {
						switch (op2.GetType()) {
						case OperandType::IMMEDIATE: return AssembleRegImm(0x6);
							case OperandType::REGISTER: {
								switch (op2.AsRegister()) {
									case Register::DT: return AssembleReg(0xf, 0x7);
									default:
										return AssembleRegReg(0x8, 0x0);
								}
							}
						}
					} break;
				};
			} break;
			case OpcodeType::ADD: {
				switch (op1.AsRegister()) {
					case Register::I: return AssembleReg(0xf, 0x1e);
					default: {
						switch (op2.GetType()) {
							case OperandType::IMMEDIATE: return AssembleRegImm(0x7);
							case OperandType::REGISTER: return AssembleRegReg(0x8, 0x4);
						}
					} break;
				}
			} break;
			case OpcodeType::OR: return AssembleRegReg(0x8, 0x1);
			case OpcodeType::AND: return AssembleRegReg(0x8, 0x2);
			case OpcodeType::XOR: return AssembleRegReg(0x8, 0x3);
			case OpcodeType::SUB: return AssembleRegReg(0x8, 0x5);
			case OpcodeType::SHR: return AssembleReg(0x8, 0x06);
			case OpcodeType::SUBN: return AssembleRegReg(0x8, 0x7);
			case OpcodeType::SHL: return AssembleReg(0x8, 0x0e);
			case OpcodeType::JP_I: return AssembleImm(0xb);
			case OpcodeType::RND: return AssembleRegImm(0xc);
			case OpcodeType::DRW: return AssembleRegRegImm(0xd);
			case OpcodeType::SKP: return AssembleReg(0xe, 0x9e);
			case OpcodeType::SKNP: return AssembleReg(0xe, 0xa1);
			case OpcodeType::LD_FONT: return AssembleReg(0xf, 0x29);
			case OpcodeType::BCD: return AssembleReg(0xf, 0x33);
			case OpcodeType::LD_MEM_REG: return AssembleReg(0xf, 0x55);
			case OpcodeType::LD_REG_MEM: return AssembleReg(0xf, 0x65);
		};

		throw std::exception("Invalid opcode/Operand combination!");
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

	void Opcode::DisassembleImm(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1.Set(opcode & 0x0FFF);
	}

	void Opcode::DisassembleReg(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1.Set((Register)((opcode >> 8) & 0xF));
	}

	void Opcode::DisassembleRegImm(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1.Set((Register)((opcode >> 8) & 0xF));
		this->op2.Set((opcode >> 4) & 0xFF);
	}

	void Opcode::DisassembleRegReg(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1.Set((Register)((opcode >> 8) & 0xF));
		this->op2.Set((Register)((opcode >> 4) & 0xF));
	}

	void Opcode::DisassembleRegRegImm(OpcodeType type, uint16_t opcode) {
		this->type = type;
		this->op1.Set((Register)((opcode >> 8) & 0xF));
		this->op2.Set((Register)((opcode >> 4) & 0xF));
		this->op3.Set(opcode & 0xF);
	}


}
