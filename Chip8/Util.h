#pragma once

#include <cstdlib>
#include <cstdint>

namespace chip8 {

	static inline uint16_t ToBigEndian(uint16_t& num) {
		int n = 1;
		if (*(char *)&n == 1) {
			num = _byteswap_ushort(num);
		}
		return num;
	}

	static inline uint32_t ToBigEndian(uint32_t& num) {
		int n = 1;
		if (*(char *)&n == 1) {
			num = _byteswap_ulong(num);
		}
		return num;
	}

	static inline uint64_t ToBigEndian(uint64_t& num) {
		int n = 1;
		if (*(char *)&n == 1) {
			num = _byteswap_uint64(num);
		}
		return num;
	}

}
