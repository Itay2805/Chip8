#pragma once

#include <cstdint>

#include "Register.h"

#include <exception>

namespace chip8 {

	enum class OperandType {
		NONE,
		REGISTER,
		IMMEDIATE
	};

	class Operand {
	private:
		OperandType type;
		union {
			Register reg;
			uint16_t imm;
		};

	public:
		Operand() 
			: type(OperandType::NONE)
			, imm(0)
		{}

		Operand(Register reg)
			: type(OperandType::REGISTER)
			, reg(reg)
		{}

		Operand(uint16_t imm)
			: type(OperandType::IMMEDIATE)
			, imm(imm)
		{}

		/*
		Create a register Operand
		*/
		inline void Set(Register reg) {
			type = OperandType::REGISTER;
			this->reg = reg;
		}

		/*
		Create an immediate Operand
		*/
		inline void Set(uint16_t imm) {
			type = OperandType::IMMEDIATE;
			this->imm = imm;
		}

		/*
		Return the type of the Operand
		*/
		inline OperandType GetType() const { return type; }

		/*
		Return the register value of the Operand

		will throw an exception if it is not a register
		*/
		inline Register AsRegister(bool onlyGeneralPurpose = false) const {
			if (type != OperandType::REGISTER) {
				throw std::exception("Tried to get Operand as register when it is not one");
			}
			if (onlyGeneralPurpose && reg > Register::VF) {
				throw std::exception("Tried to get Operand as general purpose register when it is not one");
			}
			return reg;
		}

		/*
		Return the immediate value of the Operand

		will throw an exception if it is not an immediate
		*/
		inline uint16_t AsImmediate() const { 
			if (type != OperandType::IMMEDIATE) {
				throw std::exception("Tried to get Operand as immediate when it is not one");
			}
			return imm;
		}

	};

	enum class OpcodeType {
		CLS,
		RET,
		JP,
		CALL,
		SE,
		SNE,
		LD,
		ADD,
		OR,
		AND,
		XOR,
		SUB,
		SHR,
		SUBN,
		SHL,
		JP_I,
		RND,
		DRW,
		SKP,
		SKNP,
		LD_FONT,
		BCD,

		// TODO: Add operand of mem
		LD_MEM_REG,
		LD_REG_MEM,
	};

	class Opcode {
	private:
		OpcodeType type;
		Operand op1;
		Operand op2;

		/* only needed for DRW opcode really */
		Operand op3;

	public:
		/* will disassemble the given instruction (ignores if 0) */
		Opcode(uint16_t bin);

		inline OpcodeType& Type() { return type; }
		inline Operand& Operand1() { return op1; }
		inline Operand& Operand2() { return op2; }
		inline Operand& Operand3() { return op3; }

		inline OpcodeType Type() const { return type; }
		inline Operand Operand1() const { return op1; }
		inline Operand Operand2() const { return op2; }
		inline Operand Operand3() const { return op3; }

		/* will assemble this instruction */
		uint16_t Assemble();

	private:
		/* The different layouts that can be assembled */
		uint16_t AssembleImm(uint8_t opcode) const;
		uint16_t AssembleReg(uint8_t opcode, uint8_t func) const;
		uint16_t AssembleRegImm(uint8_t opcode) const;
		uint16_t AssembleRegReg(uint8_t opcode, uint8_t func) const;
		uint16_t AssembleRegRegImm(uint8_t opcode) const;

		/* The different layouts that can be disassembled */
		void DisassembleImm(OpcodeType type, uint16_t opcode);
		void DisassembleReg(OpcodeType type, uint16_t opcode);
		void DisassembleRegImm(OpcodeType type, uint16_t opcode);
		void DisassembleRegReg(OpcodeType type, uint16_t opcode);
		void DisassembleRegRegImm(OpcodeType type, uint16_t opcode);
	};

}
