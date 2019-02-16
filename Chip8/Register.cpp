#include "Register.h"

namespace chip8 {

	std::ostream& operator<<(std::ostream& out, const Register& r) {
		switch (r) {
			case Register::V0: out << "V0"; break;
			case Register::V1: out << "V1"; break;
			case Register::V2: out << "V2"; break;
			case Register::V3: out << "V3"; break;
			case Register::V4: out << "V4"; break;
			case Register::V5: out << "V5"; break;
			case Register::V6: out << "V6"; break;
			case Register::V7: out << "V7"; break;
			case Register::V8: out << "V8"; break;
			case Register::V9: out << "V9"; break;
			case Register::VA: out << "VA"; break;
			case Register::VB: out << "VB"; break;
			case Register::VC: out << "VC"; break;
			case Register::VD: out << "VD"; break;
			case Register::VE: out << "VE"; break;
			case Register::VF: out << "VF"; break;
			case Register::I:  out << "I"; break;
			case Register::DT: out << "DT"; break;
			case Register::ST: out << "ST"; break;
			case Register::PC: out << "<PC>"; break;
			case Register::SP: out << "<SP>"; break;
			default: out << "<invalid-reg>"; break;
		}
		return out;
	}

}

