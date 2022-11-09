#include<src/Util.h>

bool util::sign(const BigInt& bInt) {
	return bInt.sign();
}

uint32_t util::digits10(const BigInt& bInt) {
	return bInt.digits10();
}

std::string util::to_string(const BigInt& bInt) {
	std::string ret;
	ret.resize(bInt.digits10() + !bInt._sign);
	char* start = ret.data() + !bInt._sign;
	memcpy(start, bInt._digits, bInt.digits10());
	if (!bInt._sign) ret[0] = '-';
	return ret;
}