#pragma once
#include<src/BigInt.h>

class util {
public:
	static bool sign(const BigInt& bInt);
	static uint32_t digits10(const BigInt& bInt);
	static std::string to_string(const BigInt& bInt);
};