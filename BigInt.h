#pragma once

#define API

#define API

class __declspec(dllexport) BigInt {
public:
	API BigInt() :_digits{ nullptr }, _end{ nullptr }{}
	API BigInt(const BigInt& in);
	API BigInt(BigInt&& in);
	API BigInt(std::string& str_in) : BigInt(static_cast<const std::string&>(str_in)) {}
	API BigInt(const std::string& str_in);
	API BigInt(char* cstr_in) : BigInt(static_cast<const char*>(cstr_in)) {}
	API BigInt(const char* cstr_in);
	template<typename Int>
	API BigInt(const Int& in);

	~BigInt();

	API const std::string str() const;
	API bool isNaN() const;

	API BigInt operator-() const;
	API BigInt& operator-();
	API friend BigInt operator+(const BigInt& l, const BigInt& r);
	API friend BigInt operator-(const BigInt& l, const BigInt& r);
	API friend BigInt operator*(const BigInt& l, const BigInt& r);
	API friend BigInt operator/(const BigInt& l, const BigInt& r);

	API friend bool operator>(const BigInt& l, const BigInt& r);
	API friend bool operator==(const BigInt& l, const BigInt& r);
	API friend bool operator>=(const BigInt& l, const BigInt& r);

private:
	int digits10() const;
	bool isValidString(const std::string& str) const;
	void swap(BigInt& in);
private:
	char* _digits{ nullptr };
	char* _end{ nullptr };
};

template<typename Int>
API BigInt::BigInt(const Int& in) {
	if constexpr (std::is_integral_v<Int> || std::is_convertible_v<Int, long long> || std::is_convertible_v<Int, unsigned long long>) {
		using IntType = std::conditional_t< std::is_integral_v<Int>,
			Int, std::conditional_t< std::is_convertible_v<Int, long long>,
			long long, unsigned long long >>;
		IntType copy = std::is_unsigned_v<IntType> ? in : (in < 0 ? -in : in);
		if (copy == 0) {
			_digits = new char[1];
			_end = _digits + 1;
			_digits[0] = '0';
		}
		else if (copy == std::numeric_limits<IntType>::min()) {
			std::string min = std::to_string(std::numeric_limits<IntType>::min());
			_digits = new char[min.size() - 1];
			_end = _digits + min.size() - 1;
			memcpy((void*)_digits, (const void*)min.data(), min.size() - 1);
		}
		else {
			size_t sz{ 0 };
			while (copy > 0) {
				copy /= 10;
				++sz;
			}
			sz += (in < 0);
			_digits = new char[sz];
			_end = _digits + sz;
			char* p = _end - 1;
			copy = std::is_unsigned_v<IntType> ? in : (in < 0 ? -in : in);
			while (copy > 0) {
				*p = '0' + (copy % 10);
				copy /= 10;
				--p;
			}
			if (in < 0) *p = '-';
		}
	}
}