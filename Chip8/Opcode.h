#pragma once

#include "Operand.h"

#include <exception>
#include <string>

namespace chip8 {

	enum class OpcodeType {
		NONE,

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
		/*
		Maybe combine with JP?
		since in the end it is still defined as JP V0, <offset>
		*/
		JP_V0,
		RND,
		DRW,
		SKP,
		SKNP,

		/* 
		special cases for ld, 
		since font and bcd are not actually registers
		*/
		LD_FONT,
		LD_BCD,
	};

	std::ostream& operator<<(std::ostream& out, OpcodeType op);

	class Opcode {
	private:
		OpcodeType type;
		Operand op1;
		Operand op2;
		Operand op3;

	public:
		/* 
		will disassemble the given instruction
		if 0 it will not do anything
		if bigEndian is true, it will assume the given number is in big endian
		otherwise it will first turn it to big endian
		*/
		Opcode(uint16_t bin = 0, bool bigEndian = false);

		inline OpcodeType& Type() { return type; }
		inline Operand& Operand1() { return op1; }
		inline Operand& Operand2() { return op2; }
		inline Operand& Operand3() { return op3; }

		inline OpcodeType Type() const { return type; }
		inline Operand Operand1() const { return op1; }
		inline Operand Operand2() const { return op2; }
		inline Operand Operand3() const { return op3; }

		/* 
		will assemble this instruction 
		if bigEndian is true, will convert to big endian, otherwise
		will leave at the native endianess
		*/
		uint16_t Assemble(bool bigEndian = true);

	private:
		/* The different layouts that can be assembled */
		uint16_t AssembleImm(uint8_t opcode) const;
		uint16_t AssembleReg(uint8_t opcode, uint8_t func) const;
		uint16_t AssembleRegImm(uint8_t opcode) const;
		uint16_t AssembleRegReg(uint8_t opcode, uint8_t func) const;
		uint16_t AssembleRegRegImm(uint8_t opcode) const;

		/* The different layouts that can be disassembled */
		void DisassembleImm(OpcodeType type, uint16_t opcode, bool addr = false);
		void DisassembleReg(OpcodeType type, uint16_t opcode, bool mem = false);
		void DisassembleRegImm(OpcodeType type, uint16_t opcode);
		void DisassembleRegReg(OpcodeType type, uint16_t opcode);
		void DisassembleRegRegImm(OpcodeType type, uint16_t opcode);

		/*
		Will print the opcode
		*/
		friend std::ostream& operator<<(std::ostream& out, Opcode opcode);

	};

}
