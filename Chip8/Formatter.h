#pragma once

#include <string>
#include <sstream>

namespace chip8 {

	class Formatter {
	public:
		Formatter() {}
		Formatter(const Formatter &) = delete;
		Formatter& operator=(Formatter &) = delete;

		template <typename Type>
		Formatter & operator<<(const Type & value) {
			stream << value;
			return *this;
		}

		inline std::string str() const { return stream.str(); }
		inline operator std::string() const { return stream.str(); }

		enum ConvertToString {
			to_str
		};
		std::string operator>> (ConvertToString) { return stream.str(); }

	private:
		std::stringstream stream;

	};

}
