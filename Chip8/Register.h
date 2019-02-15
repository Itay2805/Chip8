#pragma once

#include <cstdint>

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

		/* Program counter (not accessible) */
		PC,

		/* Stack pointer (not accesible) */
		SP,

		/* Sound counter */
		ST,

		/* Delay counter */
		DT,

		/* Register count */
		COUNT
	};

}
