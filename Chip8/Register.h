#pragma once

#include <ostream>

namespace chip8 {

	enum class Register : uint8_t {
		/* general purpose */
		V0,
		V1,
		V2,
		V3,
		V4,
		V5,
		V6,
		V7,
		V8,
		V9,
		VA,
		VB,
		VC,
		VD,
		VE,
		VF,

		/* Index register */
		I,

		/* Sound Timer */
		ST,

		/* Delay Timer */
		DT,

		/* Program counter (not accessible) */
		PC,

		/* Stack pointer (not accesible) */
		SP,

		/* Register count */
		COUNT
	};

	std::ostream& operator<<(std::ostream& out, const Register& r);

}
