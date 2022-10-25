// BigInt.cpp : 定义静态库的函数。
//

#include "pch.h"

#include"BigInt.h"

API BigInt::BigInt(const BigInt& in) {
	size_t sz = in._end - in._digits;
	_digits = new char[sz];
	_end = _digits + sz;
	memcpy((void*)_digits, (const void*)in._digits, sz);
}

API BigInt::BigInt(BigInt&& in) {
	_digits = in._digits;
	_end = in._end;
	in._digits = nullptr;
	in._end = nullptr;
}

API BigInt::BigInt(const std::string& str_in) {
	if (isValidString(str_in)) {
		_digits = new char[str_in.size()];
		_end = _digits + str_in.size();
		memcpy((void*)_digits, (const void*)str_in.data(), str_in.size());
	}
}

API BigInt::BigInt(const char* cstr_in) {
	if (isValidString(std::string(cstr_in))) {
		auto len = strlen(cstr_in);
		_digits = new char[len];
		_end = _digits + len;
		memcpy((void*)_digits, (const void*)cstr_in, len);
	}
}

API BigInt::~BigInt() {
	if (_digits != nullptr)
		delete[] _digits;
	_digits = nullptr;
	_end = nullptr;
}

API bool BigInt::isNaN() const {
	return _digits == nullptr || _digits == _end;
}

API const std::string BigInt::str() const {
	if (!isNaN()) {
		std::string str(_digits, _end - _digits);
		str.push_back('\0');
		str.resize(str.size() - 1);
		return str;
	}
	return std::string();
}

int BigInt::digits10() const {
	return _end - _digits - (_digits[0] == '-');
}

bool BigInt::isValidString(const std::string& str) const {
	int index{ 0 };
	if (str[index] == '-') ++index;
	while (index < str.size()) {
		if (!(str[index] >= '0' && str[index] <= '9')) {
			return false;
		}
		++index;
	}
	return true;
}

void BigInt::swap(BigInt& in) {
	char* mid_digit = in._digits;
	char* mid_end = in._end;
	in._digits = _digits;
	in._end = _end;
	_digits = mid_digit;
	_end = mid_end;
}

//operators
API BigInt BigInt::operator-() const {
	if (!isNaN()) {
		int redress{ _digits[0] != '-' };
		int digits = digits10() + redress;
		std::string neg;
		neg.resize(digits);
		int index{ 0 };
		if (*_digits != '-') {
			neg[index++] = '-';
		}
		while (index < neg.size()) {
			neg[index] = _digits[index + (redress ? -1 : 1)];
			++index;
		}
		return BigInt(neg);
	}
	return BigInt();
}

API BigInt& BigInt::operator-() {
	if (!isNaN()) {
		if (*_digits == '-') {
			++_digits;
			return *this;
		}
		const BigInt* cbig = this;
		BigInt temp{ cbig->operator-() };
		temp.swap(*this);
		return *this;
	}
	return *this;
}

API BigInt operator+(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		if (l._digits[0] != '-' && r._digits[0] != '-') {
			// all positive
			int l_len = l._end - l._digits;
			int r_len = r._end - r._digits;
			char* res = new char[std::max(l_len, r_len) + 2];
			char* res_p{ res + std::max(l_len, r_len) + 1 };
			*(res_p--) = '\0';
			int carry{ 0 };
			--l_len, --r_len;
			while (l_len >= 0 || r_len >= 0) {
				int l_basic{ l_len < 0 ? 0 : l._digits[l_len] - '0' };
				int r_basic{ r_len < 0 ? 0 : r._digits[r_len] - '0' };
				*res_p = (l_basic + r_basic + carry) % 10 + '0';
				carry = (l_basic + r_basic + carry) / 10;

				--l_len, --r_len, --res_p;
			}
			if (carry) {
				*res_p = carry + '0';
			}
			else ++res_p;
			BigInt ret(res_p);
			res_p = nullptr;
			delete[] res;
			return ret;
		}
		else if (l._digits[0] == '-' && r._digits[0] == '-') {
			// all negtive
			return -((-l) + (-r));
		}
		else {
			// not same sign
			bool sign{ l >= 0 };
			BigInt nature{ sign ? l : r };
			BigInt negtive{ sign ? r : l };
			return nature - (-negtive);
		}
	}
	return BigInt();
}
//TODO:
API BigInt operator-(const BigInt& l, const BigInt& r) {
	return BigInt();
}

API bool operator>(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		bool l_neg{ l._digits[0] == '-' };
		bool r_neg{ r._digits[0] == '-' };
		if (l_neg && r_neg) {
			if (l.digits10() != r.digits10()) {
				return l.digits10() < r.digits10();
			}
			l_neg = false;
			for (int i = 1; l._digits + i != l._end; ++i) {
				if (l._digits[i] > r._digits[i]) {
					return false;
				}
				else if (l._digits[i] < r._digits[i]) {
					l_neg = true;
					break;
				}
			}
		}
		else if (!l_neg && !r_neg) {
			if (l.digits10() != r.digits10()) {
				return l.digits10() > r.digits10();
			}
			l_neg = false;
			for (int i = 0; l._digits + i != l._end; ++i) {
				if (l._digits[i] < r._digits[i]) {
					return false;
				}
				else if (l._digits[i] > r._digits[i]) {
					l_neg = true;
					break;
				}
			}
		}
		else {
			l_neg = !l_neg;
		}
		return l_neg;
	}
	return false;
}

API bool operator==(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		int l_len = l._end - l._digits;
		int r_len = r._end - r._digits;
		int min_len = std::min(l_len, r_len);
		bool condition{ true };
		for (int i = 0; i < min_len; ++i) {
			if (l._digits[i] != r._digits[i]) {
				condition = false;
				break;
			}
		}
		return condition && l_len == r_len;
	}
	return false;
}

API bool operator>=(const BigInt& l, const BigInt& r) {
	return l == r && l > r;
}
