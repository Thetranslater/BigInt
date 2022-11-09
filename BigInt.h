#pragma once

#define API

#include<vector>
#include<complex>
#include<string>

class __declspec(dllexport) BigInt {
public:
	API BigInt() :_digits{ nullptr }, _end{ nullptr }{}
	API BigInt(const BigInt& in);
	API BigInt(BigInt&& in);
	API BigInt(std::string& str_in) : BigInt(static_cast<const std::string&>(str_in)) {}
	API BigInt(const std::string& str_in);
	API BigInt(std::string_view& str_v) : BigInt(static_cast<const std::string_view&>(str_v)) {}
	API BigInt(const std::string_view& str_v);
	API BigInt(char* cstr_in) : BigInt(static_cast<const char*>(cstr_in)) {}
	API BigInt(const char* cstr_in);
	template<typename Int>
	API BigInt(const Int& in);

	~BigInt();

	API const std::string str() const;
	API bool isNaN() const;

	template<typename Int>
	API BigInt& operator=(const Int& in);
	API BigInt& operator=(const BigInt& in);
	API BigInt& operator=(BigInt&& move);

	API BigInt operator-() const;
	API BigInt& operator-();
	API BigInt& operator++();
	API BigInt operator++(int);
	API BigInt& operator--();
	API BigInt operator--(int);

	API friend BigInt operator+(const BigInt& l, const BigInt& r);
	API friend BigInt operator-(const BigInt& l, const BigInt& r);
	API friend BigInt operator*(const BigInt& l, const BigInt& r);
	API friend BigInt operator/(const BigInt& l, const BigInt& r);
	API friend BigInt operator%(const BigInt& l, const BigInt& r);

	API friend bool operator>(const BigInt& l, const BigInt& r);
	API friend bool operator==(const BigInt& l, const BigInt& r);
	API friend bool operator>=(const BigInt& l, const BigInt& r);
	API friend bool operator<(const BigInt& l, const BigInt& r);
	API friend bool operator<=(const BigInt& l, const BigInt& r);

	API operator std::string() const {
		std::string ret;
		ret.resize(digits10() + !_sign);
		char* start = ret.data() + !_sign;
		memcpy(start, _digits, digits10());
		if (!_sign) ret[0] = '-';
		return ret;
	}
public:
	uint32_t digits10() const;
	bool isValidString(const std::string_view& str) const;
	void swap(BigInt& in);
	inline bool sign() const;
	inline void free();
	static BigInt multiBySingle(const BigInt& l, const char& single);
	static void fft(std::vector<std::complex<double>>& ply, bool is_inverse);
public:
	char* _digits{ nullptr };
	char* _end{ nullptr };
	bool _sign{ true };
};

template<typename Int>
API BigInt::BigInt(const Int& in) {
	//check
	std::enable_if_t<
		std::disjunction_v<std::is_integral<Int>, std::is_convertible<Int, double>, std::is_convertible<double, Int>>,
		Int> checkres;

	if constexpr (std::is_integral_v<Int>) {
		Int copy = in;
		if (copy == std::numeric_limits<Int>::min()) {
			std::string min = std::to_string(std::numeric_limits<Int>::min());
			int alloc_sz = min.size();
			if (min.front() == '-') {
				--alloc_sz;
				_sign = false;
			}
			_digits = new char[alloc_sz];
			_end = _digits + alloc_sz;
			memcpy((void*)_digits, (const void*)(min.data() + !_sign), alloc_sz);
		}
		else {
			copy = std::is_unsigned_v<Int> ? in : (in < 0 ? -in : in);
			size_t sz{ 0 };
			while (copy > 0) {
				copy /= 10;
				++sz;
			}
			_digits = new char[sz];
			_end = _digits + sz;
			char* p = _end - 1;
			copy = std::is_unsigned_v<Int> ? in : (in < 0 ? -in : in);
			while (copy > 0) {
				*p = '0' + (copy % 10);
				copy /= 10;
				--p;
			}
			if (in < 0) _sign = false;
		}
	}
	else if constexpr (std::is_convertible_v<Int, double> && std::is_convertible_v<double, Int>) {
		if ((double)in <= std::numeric_limits<double>::max() && (double)in >= -std::numeric_limits<double>::max() &&
			in <= Int(std::numeric_limits<double>::max()) && in >= -Int(std::numeric_limits<double>::max())) {
			double fin{ double(in) };
			if (fin >= double(std::numeric_limits<unsigned long long>::max()) || fin <= double(std::numeric_limits<long long>::min())) {
				double din{ std::abs(fin) };
				size_t sz{ 0 };
				while (din >= 1) {
					din /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				din = (double)std::abs(fin);
				while (din >= 1) {
					*p = '0' + (int)std::fmod(din, 10.f);
					din /= 10;
					--p;
				}
			}
			else if (fin >= double(std::numeric_limits<long long>::max())) {
				unsigned long long ullin = (unsigned long long)fin;
				size_t sz{ 0 };
				while (ullin > 0) {
					ullin /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				ullin = (unsigned long long)std::abs(fin);
				while (ullin > 0) {
					*p = '0' + (ullin) % 10;
					ullin /= 10;
					--p;
				}
			}
			else {
				long long llin = (long long)std::abs(fin);
				size_t sz{ 0 };
				while (llin > 0) {
					llin /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				llin = (long long)std::abs(fin);
				while (llin > 0) {
					*p = '0' + (llin) % 10;
					llin /= 10;
					--p;
				}
			}
			if (fin < 0) _sign = false;
		}
	}
}

template<typename Int>
API BigInt& BigInt::operator=(const Int& in) {
	//check
	std::enable_if_t<
		std::disjunction_v<std::is_integral<Int>, std::is_convertible<Int, double>, std::is_convertible<double, Int>>,
		Int> checkres;

	if constexpr (std::is_integral_v<Int>) {
		free();
		Int copy = in;
		if (copy == std::numeric_limits<Int>::min()) {
			std::string min = std::to_string(std::numeric_limits<Int>::min());
			int alloc_sz = min.size();
			if (min.front() == '-') {
				--alloc_sz;
				_sign = false;
			}
			_digits = new char[alloc_sz];
			_end = _digits + alloc_sz;
			memcpy((void*)_digits, (const void*)(min.data() + !_sign), alloc_sz);
		}
		else {
			copy = std::is_unsigned_v<Int> ? in : (in < 0 ? -in : in);
			size_t sz{ 0 };
			while (copy > 0) {
				copy /= 10;
				++sz;
			}
			_digits = new char[sz];
			_end = _digits + sz;
			char* p = _end - 1;
			copy = std::is_unsigned_v<Int> ? in : (in < 0 ? -in : in);
			while (copy > 0) {
				*p = '0' + (copy % 10);
				copy /= 10;
				--p;
			}
			if (in < 0) _sign = false;
		}
	}
	else if constexpr (std::is_convertible_v<Int, double> && std::is_convertible_v<double, Int>) {
		if ((double)in <= std::numeric_limits<double>::max() && (double)in >= -std::numeric_limits<double>::max() &&
			in <= Int(std::numeric_limits<double>::max()) && in >= -Int(std::numeric_limits<double>::max())) {
			free();
			double fin{ double(in) };
			if (fin >= double(std::numeric_limits<unsigned long long>::max()) || fin <= double(std::numeric_limits<long long>::min())) {
				double din{ std::abs(fin) };
				size_t sz{ 0 };
				while (din >= 1) {
					din /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				din = (double)std::abs(fin);
				while (din >= 1) {
					*p = '0' + (int)std::fmod(din, 10.f);
					din /= 10;
					--p;
				}
			}
			else if (fin >= double(std::numeric_limits<long long>::max())) {
				unsigned long long ullin = (unsigned long long)fin;
				size_t sz{ 0 };
				while (ullin > 0) {
					ullin /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				ullin = (unsigned long long)std::abs(fin);
				while (ullin > 0) {
					*p = '0' + (ullin) % 10;
					ullin /= 10;
					--p;
				}
			}
			else {
				long long llin = (long long)std::abs(fin);
				size_t sz{ 0 };
				while (llin > 0) {
					llin /= 10;
					++sz;
				}
				_digits = new char[sz];
				_end = _digits + sz;
				char* p = _end - 1;
				llin = (long long)std::abs(fin);
				while (llin > 0) {
					*p = '0' + (llin) % 10;
					llin /= 10;
					--p;
				}
			}
			if (fin < 0) _sign = false;
		}
	}
}

namespace std {
	template<>
	struct hash<BigInt> {
		std::size_t operator()(const BigInt& bInt) {
			return hash<std::string>{}(std::string(bInt));
		}
	};
}