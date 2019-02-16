#pragma once

#include "Register.h"

#include <iostream>
#include <exception>

namespace chip8 {

	enum class OperandType {
		NONE,
		REGISTER,
		IMMEDIATE
	};

	std::ostream& operator<<(std::ostream& out, const OperandType& r);

	class Operand {
	private:
		OperandType type;
		bool mem, addr;
		union {
			Register reg;
			uint16_t imm;
		};

	public:
		Operand();
		Operand(Register reg, bool mem = false);
		Operand(uint16_t imm, bool mem = false, bool addr = false);

		/*
		Return the type of the Operand
		*/
		inline OperandType GetType() const { return type; }

		inline bool IsMemory() const { return mem; }

		inline bool IsAddress() const { return addr; }


		/*
		Return the register value of the Operand

		will throw an exception if it is not a register
		*/
		Register AsRegister(bool onlyGeneralPurpose = false) const;

		/*
		Return the immediate value of the Operand

		will throw an exception if it is not an immediate
		*/
		uint16_t AsImmediate() const;

		/*
		Will print the operand
		*/
		friend std::ostream& operator<<(std::ostream& out, const Operand& op);
	};
	

}
