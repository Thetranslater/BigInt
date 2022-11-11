// BigInt.cpp : 定义静态库的函数。
#include<cstring>
#include<type_traits>
#include<string>
#include<limits>
#include<vector>
#include<complex>
#include<string_view>

#include"src/BigInt_impl.h"
#include"src/Util.h"

//friend declear
API BigInt operator+(const BigInt& l, const BigInt& r);
API BigInt operator-(const BigInt& l, const BigInt& r);
API BigInt operator*(const BigInt& l, const BigInt& r);
API BigInt operator/(const BigInt& l, const BigInt& r);
API BigInt operator%(const BigInt& l, const BigInt& r);

API bool operator>(const BigInt& l, const BigInt& r);
API bool operator==(const BigInt& l, const BigInt& r);
API bool operator>=(const BigInt& l, const BigInt& r);
API bool operator<(const BigInt& l, const BigInt& r);
API bool operator<=(const BigInt& l, const BigInt& r);

API std::ostream& operator<<(std::ostream& o, BigInt& bInt);
API std::istream& operator>>(std::istream& i, BigInt& bInt);

//implementation
API BigInt::BigInt(const BigInt& in) {
	size_t sz = in._end - in._digits;
	_digits = new char[sz];
	_end = _digits + sz;
	memcpy((void*)_digits, (const void*)in._digits, sz);
	_sign = in._sign;
}

API BigInt::BigInt(BigInt&& in) {
	_digits = in._digits;
	_end = in._end;
	_sign = in._sign;
	in._digits = nullptr;
	in._end = nullptr;
}

API BigInt::BigInt(const std::string& str_in) {
	if (isValidString(str_in)) {
		size_t alloc_sz = str_in.size();
		if (str_in.front() == '-') {
			--alloc_sz;
			_sign = false;
		}
		_digits = new char[alloc_sz];
		_end = _digits + alloc_sz;
		memcpy((void*)_digits, (const void*)(str_in.data() + !_sign), alloc_sz);
	}
}

API BigInt::BigInt(const std::string_view& str_v) {
	if (isValidString(str_v)) {
		size_t alloc_sz = str_v.size();
		if (str_v[0] == '-') {
			--alloc_sz;
			_sign = false;
		}
		_digits = new char[alloc_sz];
		_end = _digits + alloc_sz;
		memcpy((void*)_digits, (const void*)(str_v.data() + !_sign), alloc_sz);
	}
}

API BigInt::BigInt(const char* cstr_in) {
	if (isValidString(std::string(cstr_in))) {
		size_t len = strlen(cstr_in);
		if (*cstr_in == '-') {
			--len;
			_sign = false;
		}
		_digits = new char[len];
		_end = _digits + len;
		memcpy((void*)_digits, (const void*)(cstr_in + !_sign), len);
	}
}

API BigInt::~BigInt() {
	free();
}

API bool BigInt::isNaN() const {
	return _digits == nullptr || _digits == _end;
}

uint32_t BigInt::digits10() const {
	return static_cast<uint32_t>(_end - _digits);
}

bool BigInt::isValidString(const std::string_view& str) const {
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
	bool mid_sign = in._sign;
	in._digits = _digits;
	in._end = _end;
	in._sign = _sign;
	_digits = mid_digit;
	_end = mid_end;
	_sign = mid_sign;
}

void BigInt::free() {
	if (!isNaN()) delete[] _digits;
	_digits = nullptr;
	_end = nullptr;
	_sign = true;
}

void BigInt::fft(std::vector<std::complex<double>>& ply, bool is_inverse) {
	auto pi = 3.141592653589793238462643383279502884e+00;
	int n = ply.size();
	if (n == 1) {
		return;
	}
	int mid = n / 2;
	std::vector<std::complex<double>> f1(mid, std::complex<double>());
	std::vector<std::complex<double>> f2(mid, std::complex<double>());
	for (int i = 0; i < mid; ++i) {
		f1[i] = ply[2 * i];
		f2[i] = ply[2 * i + 1];
	}
	fft(f1, is_inverse);
	fft(f2, is_inverse);
	std::complex<double> w0(1, 0), wn(std::cos((2 * pi) / n), (is_inverse ? -1 : 1) * std::sin((2 * pi) / n));
	for (int i = 0; i < mid; ++i, w0 *= wn) {
		ply[i] = f1[i] + w0 * f2[i];
		ply[i + mid] = f1[i] - w0 * f2[i];
	}
}

BigInt BigInt::multiBySingle(const BigInt& l, const char& single) {
	if (single == '0') return BigInt(0);
	std::string res(l.digits10() + 1, 0);
	auto tail = res.end();
	int carry{ 0 };
	for (auto rbegin{ l._end }; rbegin != l._digits; --rbegin) {
		int l_basic = *(rbegin - 1) - '0';
		int r_basic = single - '0';
		*(--tail) = ((l_basic * r_basic + carry) % 10) + '0';
		carry = (l_basic * r_basic + carry) / 10;
	}
	if (carry > 0) {
		*(--tail) = carry + '0';
	}
	int head = tail - res.begin();
	std::string_view res_v(res.data() + head, res.size() - head);
	BigInt ret(res_v);
	ret._sign = l._sign;
	return ret;
}

bool BigInt::sign() const {
	return !isNaN() && _sign;
}

//operators
API BigInt BigInt::operator-() const {
	if (!isNaN()) {
		BigInt rev(*this);
		rev._sign = !_sign;
		return rev;
	}
	return BigInt();
}

API BigInt& BigInt::operator-() {
	_sign = !_sign;
	return *this;
}

API BigInt& BigInt::operator++() {
	*this = operator+(*this, 1);
	return *this;
}

API BigInt BigInt::operator++(int) {
	BigInt temp(*this);
	*this = operator+(*this, 1);
	return temp;
}

API BigInt& BigInt::operator--() {
	*this = ::operator-(*this, 1);
	return *this;
}

API BigInt BigInt::operator--(int) {
	BigInt temp(*this);
	*this = ::operator-(*this, 1);
	return temp;
}

API BigInt operator+(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		if (l.sign() && r.sign()) {
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
		else if (!l.sign() && !r.sign()) {
			// all negtive
			return -((-l) + (-r));
		}
		else {
			// not same sign
			bool sign{ operator>=(l,0) };
			BigInt nature{ sign ? l : r };
			BigInt negtive{ sign ? r : l };
			return nature - (-negtive);
		}
	}
	return BigInt();
}
//TODO:
API BigInt operator-(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		BigInt copy{ l };
		std::string sres(std::max(l.digits10(), r.digits10()), '\0');
		int index = sres.size() - 1;
		bool l_sign{ l.sign() }, r_sign{ r.sign() };
		if (l_sign && r_sign) {
			if (l > r) {
				int i = l.digits10() - 1, j = r.digits10() - 1;
				while (i >= 0) {
					int lbasic = i < 0 ? 0 : (copy._digits[i] - '0');
					int rbasic = j < 0 ? 0 : (r._digits[j] - '0');
					int res{ lbasic - rbasic };
					if (res < 0) {
						int k{ i - 1 };
						while (k >= 0 && copy._digits[k] == '0') {
							copy._digits[k] = '9';
							--k;
						}
						--copy._digits[k];
						res += 10;
					}
					sres[index--] = res + '0';
					--i, --j;
				}
				auto n0iter{ sres.begin() };
				while (n0iter + 1 != sres.end() && *n0iter == '0') ++n0iter;
				int head = n0iter - sres.begin();
				return BigInt(std::string_view(sres.data() + head, sres.size() - head));
			}
			else return -(r - l);
		}
		else if (l_sign && !r_sign) {
			return l + (-r);
		}
		else if (!l_sign && r_sign) {
			return -((-l) + r);
		}
		return (-r) - (-l);
	}
	return BigInt();
}

API bool operator>(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		bool l_neg{ !l.sign() };
		bool r_neg{ !r.sign() };
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
	return l == r || l > r;
}

API bool operator<(const BigInt& l, const BigInt& r) {
	return !(l > r || l == r);
}

API bool operator<=(const BigInt& l, const BigInt& r) {
	return !(l > r);
}
//fft o(nlogn)
API BigInt operator*(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		BigInt ret;
		uint32_t lsz{ l.digits10() }, rsz{ r.digits10() };
		if (lsz == 1) {
			ret = BigInt::multiBySingle(r, (*(l._digits)));
			ret._sign = l._sign == r._sign;
			return ret;
		}
		else if (rsz == 1) {
			ret = BigInt::multiBySingle(l, (*(r._digits)));
			ret._sign = l._sign == r._sign;
			return ret;
		}
		uint32_t pow2sz{ 1u << std::max((uint32_t)std::ceil(std::log2(lsz + rsz)),1u) };

		std::vector<std::complex<double>> lply(pow2sz, std::complex<double>());
		std::vector<std::complex<double>> rply(pow2sz, std::complex<double>());

		for (uint32_t i = 0; i < lsz; ++i) {
			lply[i].real(*(l._end - 1 - i) - '0');
		}
		for (uint32_t i = 0; i < rsz; ++i) {
			rply[i].real(*(r._end - 1 - i) - '0');
		}
		BigInt::fft(lply, false);
		BigInt::fft(rply, false);
		for (int i = 0; i < pow2sz; ++i) {
			lply[i] = lply[i] * rply[i];
		}
		BigInt::fft(lply, true);
		for (int i = 0; i < pow2sz; ++i) {
			lply[i]._Val[0] = (lply[i]._Val[0] / pow2sz) + 1e-8;
		}
		//进位
		int carry{ 0 };
		for (uint32_t i = 0; i < pow2sz; ++i) {
			int basic = (int)lply[i]._Val[0] + carry;
			lply[i]._Val[0] = basic % 10;
			carry = basic / 10;
		}
		//除去前导0
		uint32_t start{ pow2sz - 1 };
		while (start >= 0 && lply[start].real() < 1) {
			--start;
		}
		//符号
		bool sign{ l.sign() == r.sign() };
		//构造结果
		std::string multi_res(start + 1, '\0');
		for (uint32_t i = 0; i < start + 1; ++i) {
			multi_res[i] = lply[start - i].real() + '0';
		}
		ret = multi_res;
		if (!sign) - ret;
		return ret;
	}
	return BigInt();
}

API BigInt operator/(const BigInt& l, const BigInt& r)
{
	//大除法
	if (!l.isNaN() && !r.isNaN()) {
		if (r == 0) throw std::exception("divided by zero!");
		BigInt _r = r < 0 ? -r : r;

		auto dig_r{ r.digits10() };
		auto dig_l{ l.digits10() };
		std::string quotient;
		std::string remainder;
		bool l_sign = l.sign(), r_sign = r.sign();

		uint32_t i{ 0 };
		for (; i < dig_r - 1; ++i) {
			if (i >= dig_l) return BigInt(0);
			remainder.push_back(l._digits[i]);
		}
		while (i < dig_l) {
			remainder.push_back(l._digits[i]);
			//binary search
			int left{ 0 }, right{ 9 };
			while (left < right) {
				int middle{ right - (right - left) / 2 };
				auto multi_res = BigInt::multiBySingle(_r, middle + '0');
				if (multi_res > remainder) {
					right = middle - 1;
				}
				else {
					left = middle;
				}
			}
			quotient.push_back(left + '0');

			auto int_remain = BigInt(remainder) - BigInt::multiBySingle(_r, left + '0');
			remainder = util::to_string(int_remain);
			++i;
		}
		BigInt ret(0);
		if (!quotient.empty()) {
			int head = quotient.size() == 1 ? 0 : quotient.front() == '0';
			std::string_view quo_v(quotient.data() + head, quotient.size() - head);
			ret = BigInt(quo_v);
			if (!(l_sign ^ r_sign)) {
				ret._sign = true;
			}
			else {
				ret._sign = false;
			}
		}
		return ret;
	}
	return BigInt();
}

API BigInt operator%(const BigInt& l, const BigInt& r) {
	if (!l.isNaN() && !r.isNaN()) {
		BigInt quotient = l / r;
		BigInt remainder = l - r * quotient;
		return remainder;
	}
	return BigInt();
}

API BigInt& BigInt::operator=(const BigInt& in) {
	free();
	_sign = in._sign;
	size_t sz = in.digits10();
	_digits = new char[sz];
	_end = _digits + sz;
	memcpy((void*)_digits, (const void*)in._digits, sz);
	return *this;
}

API BigInt& BigInt::operator=(BigInt&& move) {
	free();

	_digits = move._digits;
	_end = move._end;
	_sign = move._sign;

	move._digits = nullptr;
	move._end = nullptr;
	move._sign = true;

	return *this;
}

API std::ostream& operator<<(std::ostream& o, BigInt& bInt) {
	o << util::to_string(bInt);
	return o;
}
//TODO:处理浮点缩窄，输入错误的情况
API std::istream& operator>>(std::istream& i, BigInt& bInt) {
	std::string data;
	i >> data;
	size_t size = data.size();
	bInt.free();
	bInt._digits = new char[size];
	bInt._end = bInt._digits + size;
	bInt._sign = true;
	memcpy(bInt._digits, data.data(), size);
	if (*bInt._digits == '-') {
		++bInt._digits;
		bInt._sign = false;
	}
	return i;
}

API BigInt::operator long long() const {
	long long ll;
	try {
		ll = std::stoll(util::to_string(*this));
	}
	catch (std::out_of_range&) {
		if (_sign) return LLONG_MAX;
		else return LLONG_MIN;
	}
	return ll;
}

API BigInt::operator long() const {
	long l;
	try {
		l = std::stol(util::to_string(*this));
	}
	catch (std::out_of_range&) {
		if (_sign) return LONG_MAX;
		else return LONG_MIN;
	}
	return l;
}

API BigInt::operator int() const {
	int i;
	try {
		i = std::stoi(util::to_string(*this));
	}
	catch (std::out_of_range& out) {
		if (_sign) return INT_MAX;
		else return INT_MIN;
	}
	return i;
}

API BigInt::operator short() const {
	if (*this > BigInt(SHRT_MAX)) {
		return SHRT_MAX;
	}
	else if (*this < BigInt(SHRT_MIN)) {
		return SHRT_MIN;
	}
	return short(int(*this));
}

API BigInt::operator char() const {
	if (*this > BigInt(CHAR_MAX)) {
		return CHAR_MAX;
	}
	else if (*this < BigInt(CHAR_MIN)) {
		return CHAR_MIN;
	}
	return char(int(*this));
}

API BigInt::operator unsigned long long() const {
	if (!_sign) return 0;
	unsigned long long ull;
	try {
		ull = std::stoull(util::to_string(*this));
	}
	catch (std::out_of_range& out) {
		return ULLONG_MAX;
	}
	return ull;
}

API BigInt::operator unsigned long() const {
	if (!_sign) return 0;
	unsigned long ul;
	try {
		ul = std::stoul(util::to_string(*this));
	}
	catch (std::out_of_range&) {
		return ULONG_MAX;
	}
	return ul;
}

API BigInt::operator unsigned int() const {
	if (!_sign) return 0;
	if (*this > BigInt(UINT32_MAX)) {
		return UINT32_MAX;
	}
	return unsigned int(unsigned long(*this));
}

API BigInt::operator unsigned short() const {
	if (!_sign) return 0;
	if (*this > BigInt(USHRT_MAX)) {
		return USHRT_MAX;
	}
	return unsigned short(unsigned long(*this));
}

API BigInt::operator unsigned char() const {
	if (!_sign) return 0;
	if (*this > BigInt(UCHAR_MAX)) {
		return UCHAR_MAX;
	}
	return unsigned char(unsigned long(*this));
}

namespace std {
	std::size_t hash<BigInt>::operator()(const BigInt& bInt) const {
		return hash<std::string>{}(util::to_string(bInt));
	}
}
