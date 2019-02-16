#include "Operand.h"

#include <sstream>
#include <iomanip>

#include "Formatter.h"

namespace chip8 {

	inline std::ostream& operator<<(std::ostream& out, const OperandType& r) {
		switch (r) {
			case OperandType::REGISTER: out << "REGISTER"; break;
			case OperandType::IMMEDIATE: out << "IMMEDIATE"; break;
			default:
				out << "Unknwon Operand"; break;
		}
		return out;
	}

	Operand::Operand() 
		: type(OperandType::NONE)
		, mem(false)
		, addr(false)
		, imm(0)
	{
	}

	Operand::Operand(Register reg, bool mem) 
		: type(OperandType::REGISTER)
		, reg(reg)
		, mem(mem)
		, addr(false)
	{
	}

	Operand::Operand(uint16_t imm, bool mem, bool addr)
		: type(OperandType::IMMEDIATE)
		, imm(imm)
		, mem(mem)
		, addr(addr)
	{
	}

	Register Operand::AsRegister(bool onlyGeneralPurpose) const {
		if (type != OperandType::REGISTER) {
			throw std::runtime_error(Formatter() << "Attempted to get " << type << " as a reigster");
		}
		if (onlyGeneralPurpose && reg > Register::VF) {
			throw std::runtime_error(Formatter() << "Attmpted to get none-general purpose register " << reg);
		}
		return reg;
	}

	uint16_t Operand::AsImmediate() const {
		if (type != OperandType::IMMEDIATE) {
			throw std::runtime_error(Formatter() << "Attempted to get " << type << " as an immediate");
		}
		return imm;
	}

	std::ostream& operator<<(std::ostream& out, const Operand& op) {
		if (op.mem) {
			out << "[";
		}
		switch (op.type) {
			case OperandType::REGISTER: {
				out << op.reg;
			} break;
			case OperandType::IMMEDIATE: {
				if (op.addr) {
					out << std::setfill('0') << std::setw(3) << std::hex << op.imm;
				}
				else {
					out << op.imm;
				}
			} break;
		}
		if (op.mem) {
			out << "]";
		}

		return out;
	}


}
