#include "BigInt.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

BigInt::BigInt() {
    digits = {0};
    is_negative = false;
}

BigInt::BigInt(long long value) {
    if (value < 0) {
        is_negative = true;
        uint64_t uval = static_cast<uint64_t>(-(value + 1)) + 1;
        digits = {uval};
    } else {
        is_negative = false;
        digits = {static_cast<uint64_t>(value)};
    }
}

BigInt::BigInt(const std::string& str_value) {
    digits = {0};
    is_negative = false;

    size_t i = 0;
    while (i < str_value.size() && std::isspace(static_cast<unsigned char>(str_value[i]))) {
        i++;
    }
    if (i >= str_value.size()) {
        return;
    }

    if (str_value[i] == '-') {
        is_negative = true;
        i++;
    } else if (str_value[i] == '+') {
        i++;
    }

    while (i < str_value.size() && std::isspace(static_cast<unsigned char>(str_value[i]))) {
        i++;
    }

    std::string dec_str;
    while (i < str_value.size() && std::isdigit(static_cast<unsigned char>(str_value[i]))) {
        dec_str.push_back(str_value[i++]);
    }

    if (dec_str.empty()) {
        is_negative = false;
        return;
    }

    static const uint64_t pow10_arr[] = {
        1ULL,
        10ULL,
        100ULL,
        1000ULL,
        10000ULL,
        100000ULL,
        1000000ULL,
        10000000ULL,
        100000000ULL,
        1000000000ULL,
        10000000000ULL,
        100000000000ULL,
        1000000000000ULL,
        10000000000000ULL,
        100000000000000ULL,
        1000000000000000ULL,
        10000000000000000ULL,
        100000000000000000ULL,
        1000000000000000000ULL
    };

    digits = {0};
    size_t pos = 0;
    while (pos < dec_str.length()) {
        size_t chunk_len = std::min(static_cast<size_t>(18), dec_str.length() - pos);
        uint64_t val = std::stoull(dec_str.substr(pos, chunk_len));
        uint64_t mult = pow10_arr[chunk_len];
        pos += chunk_len;

        __uint128_t carry = val;
        for (size_t k = 0; k < digits.size(); ++k) {
            __uint128_t cur = static_cast<__uint128_t>(digits[k]) * mult + carry;
            digits[k] = static_cast<uint64_t>(cur);
            carry = cur >> 64;
        }
        while (carry > 0) {
            digits.push_back(static_cast<uint64_t>(carry));
            carry >>= 64;
        }
    }

    remove_leading_zeros();
}

bool BigInt::operator==(const BigInt& other) const {
    if (is_negative != other.is_negative) return false;
    return compare_magnitude(*this, other) == 0;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    if (is_negative != other.is_negative) {
        return is_negative;
    }
    int cmp = compare_magnitude(*this, other);
    if (is_negative) {
        return cmp > 0;
    } else {
        return cmp < 0;
    }
}

bool BigInt::operator<=(const BigInt& other) const {
    return (*this < other) || (*this == other);
}

bool BigInt::operator>(const BigInt& other) const {
    return other < *this;
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (is_negative == other.is_negative) {
        BigInt res = add_magnitude(*this, other);
        res.is_negative = is_negative;
        res.remove_leading_zeros();
        return res;
    }

    int cmp = compare_magnitude(*this, other);
    if (cmp == 0) {
        return BigInt(0);
    }
    if (cmp > 0) {
        BigInt res = sub_magnitude(*this, other);
        res.is_negative = is_negative;
        res.remove_leading_zeros();
        return res;
    } else {
        BigInt res = sub_magnitude(other, *this);
        res.is_negative = other.is_negative;
        res.remove_leading_zeros();
        return res;
    }
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (is_negative != other.is_negative) {
        BigInt res = add_magnitude(*this, other);
        res.is_negative = is_negative;
        res.remove_leading_zeros();
        return res;
    }

    int cmp = compare_magnitude(*this, other);
    if (cmp == 0) {
        return BigInt(0);
    }
    if (cmp > 0) {
        BigInt res = sub_magnitude(*this, other);
        res.is_negative = is_negative;
        res.remove_leading_zeros();
        return res;
    } else {
        BigInt res = sub_magnitude(other, *this);
        res.is_negative = !is_negative;
        res.remove_leading_zeros();
        return res;
    }
}

BigInt BigInt::operator*(const BigInt& other) const {
    if ((digits.size() == 1 && digits[0] == 0) || (other.digits.size() == 1 && other.digits[0] == 0)) {
        return BigInt(0);
    }

    size_t size1 = digits.size();
    size_t size2 = other.digits.size();
    std::vector<uint64_t> temp(size1 + size2, 0);

    for (size_t i = 0; i < size1; ++i) {
        if (digits[i] == 0) continue;
        __uint128_t carry = 0;
        for (size_t j = 0; j < size2 || carry; ++j) {
            __uint128_t cur = temp[i + j] + carry;
            if (j < size2) {
                cur += static_cast<__uint128_t>(digits[i]) * other.digits[j];
            }
            temp[i + j] = static_cast<uint64_t>(cur);
            carry = cur >> 64;
        }
    }

    BigInt res;
    res.digits = temp;
    res.is_negative = (is_negative != other.is_negative);
    res.remove_leading_zeros();
    return res;
}

BigInt BigInt::operator/(const BigInt& other) const {
    BigInt Q, R;
    divmod_internal(*this, other, Q, R);
    return Q;
}

BigInt BigInt::operator%(const BigInt& other) const {
    BigInt Q, R;
    divmod_internal(*this, other, Q, R);
    return R;
}

BigInt& BigInt::operator+=(const BigInt& other) {
    *this = *this + other;
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
    *this = *this - other;
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& other) {
    *this = *this * other;
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
    *this = *this / other;
    return *this;
}

BigInt& BigInt::operator%=(const BigInt& other) {
    *this = *this % other;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const BigInt& num) {
    if (num.digits.empty() || (num.digits.size() == 1 && num.digits[0] == 0)) {
        os << '0';
        return os;
    }
    if (num.is_negative) {
        os << '-';
    }

    BigInt temp = num;
    temp.is_negative = false;

    const uint64_t BASE10 = 1000000000000000000ULL; // 10^18
    std::vector<uint64_t> chunks;

    while (!(temp.digits.size() == 1 && temp.digits[0] == 0)) {
        __uint128_t rem = 0;
        for (size_t i = temp.digits.size(); i-- > 0; ) {
            __uint128_t cur = (rem << 64) | temp.digits[i];
            temp.digits[i] = static_cast<uint64_t>(cur / BASE10);
            rem = cur % BASE10;
        }
        temp.remove_leading_zeros();
        chunks.push_back(static_cast<uint64_t>(rem));
    }

    if (chunks.empty()) {
        os << '0';
        return os;
    }

    os << chunks.back();

    for (size_t i = chunks.size() - 1; i-- > 0; ) {
        std::string s = std::to_string(chunks[i]);
        if (s.length() < 18) {
            os << std::string(18 - s.length(), '0');
        }
        os << s;
    }

    return os;
}

std::istream& operator>>(std::istream& is, BigInt& num) {
    std::string input;
    if (is >> input) {
        num = BigInt(input);
    }
    return is;
}

void BigInt::remove_leading_zeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) {
        digits.push_back(0);
    }
    if (digits.size() == 1 && digits[0] == 0) {
        is_negative = false;
    }
}

void BigInt::shift_left_words(std::vector<uint64_t>& a, int s) {
    if (s == 0) return;
    uint64_t carry = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        uint64_t cur = a[i];
        a[i] = (cur << s) | carry;
        carry = cur >> (64 - s);
    }
    if (carry > 0) {
        a.push_back(carry);
    }
}

void BigInt::shift_right_words(std::vector<uint64_t>& a, int s) {
    if (s == 0) return;
    uint64_t carry = 0;
    for (size_t i = a.size(); i-- > 0; ) {
        uint64_t cur = a[i];
        a[i] = (cur >> s) | carry;
        carry = cur << (64 - s);
    }
}

int BigInt::compare_magnitude(const BigInt& a, const BigInt& b) {
    if (a.digits.size() < b.digits.size()) return -1;
    if (a.digits.size() > b.digits.size()) return 1;
    for (size_t i = a.digits.size(); i-- > 0; ) {
        if (a.digits[i] < b.digits[i]) return -1;
        if (a.digits[i] > b.digits[i]) return 1;
    }
    return 0;
}

BigInt BigInt::add_magnitude(const BigInt& a, const BigInt& b) {
    BigInt res;
    res.digits.clear();
    size_t n = std::max(a.digits.size(), b.digits.size());
    __uint128_t carry = 0;
    for (size_t i = 0; i < n || carry; ++i) {
        __uint128_t sum = carry;
        if (i < a.digits.size()) sum += a.digits[i];
        if (i < b.digits.size()) sum += b.digits[i];
        res.digits.push_back(static_cast<uint64_t>(sum));
        carry = sum >> 64;
    }
    res.is_negative = false;
    res.remove_leading_zeros();
    return res;
}

BigInt BigInt::sub_magnitude(const BigInt& a, const BigInt& b) {
    BigInt res;
    res.digits.clear();
    __uint128_t borrow = 0;
    for (size_t i = 0; i < a.digits.size(); ++i) {
        uint64_t a_val = a.digits[i];
        uint64_t b_val = (i < b.digits.size()) ? b.digits[i] : 0;
        __uint128_t diff = ((__uint128_t)1 << 64) + a_val - b_val - borrow;
        res.digits.push_back(static_cast<uint64_t>(diff));
        borrow = 1 - (diff >> 64);
    }
    res.is_negative = false;
    res.remove_leading_zeros();
    return res;
}

int BigInt::count_leading_zeros(uint64_t x) {
    if (x == 0) return 64;
    int count = 0;
    while ((x & (1ULL << 63)) == 0) {
        count++;
        x <<= 1;
    }
    return count;
}

void BigInt::divmod_internal(const BigInt& A, const BigInt& B, BigInt& Q, BigInt& R) {
    if (B.digits.empty() || (B.digits.size() == 1 && B.digits[0] == 0)) {
        throw std::invalid_argument("Division by zero");
    }

    int cmp = compare_magnitude(A, B);
    if (cmp < 0) {
        Q = BigInt(0);
        R = A;
        R.is_negative = A.is_negative;
        if (R.digits.size() == 1 && R.digits[0] == 0) {
            R.is_negative = false;
        }
        return;
    }
    if (cmp == 0) {
        Q = BigInt(1);
        Q.is_negative = (A.is_negative != B.is_negative);
        R = BigInt(0);
        return;
    }

    if (B.digits.size() == 1) {
        uint64_t d = B.digits[0];
        Q.digits.resize(A.digits.size());
        __uint128_t rem = 0;
        for (size_t i = A.digits.size(); i-- > 0; ) {
            __uint128_t cur = (rem << 64) | A.digits[i];
            Q.digits[i] = static_cast<uint64_t>(cur / d);
            rem = cur % d;
        }
        R.digits = {static_cast<uint64_t>(rem)};
    } else {
        size_t m = B.digits.size();
        size_t n = A.digits.size() - m;

        int s = count_leading_zeros(B.digits.back());
        std::vector<uint64_t> u = A.digits;
        std::vector<uint64_t> v = B.digits;
        shift_left_words(u, s);
        shift_left_words(v, s);

        while (u.size() <= A.digits.size() + 1) {
            u.push_back(0);
        }

        Q.digits.assign(n + 1, 0);

        for (size_t j = n + 1; j-- > 0; ) {
            __uint128_t num = ((__uint128_t)u[j + m] << 64) | u[j + m - 1];
            __uint128_t q_hat;
            __uint128_t r_hat;
            if (u[j + m] == v[m - 1]) {
                q_hat = UINT64_MAX;
                r_hat = num - (__uint128_t)UINT64_MAX * v[m - 1];
            } else {
                q_hat = num / v[m - 1];
                r_hat = num % v[m - 1];
            }

            while (r_hat <= UINT64_MAX &&
                   ((__uint128_t)(uint64_t)q_hat * v[m - 2] > ((r_hat << 64) | u[j + m - 2]))) {
                q_hat--;
                r_hat += v[m - 1];
            }

            __uint128_t carry = 0;
            for (size_t i = 0; i < m; ++i) {
                __uint128_t prod = (__uint128_t)(uint64_t)q_hat * v[i] + carry;
                uint64_t prod_lo = static_cast<uint64_t>(prod);
                carry = prod >> 64;

                if (u[j + i] < prod_lo) {
                    carry++;
                }
                u[j + i] -= prod_lo;
            }

            if (u[j + m] < carry) {
                u[j + m] -= carry;
                q_hat--;
                __uint128_t add_carry = 0;
                for (size_t i = 0; i < m; ++i) {
                    __uint128_t sum = (__uint128_t)u[j + i] + v[i] + add_carry;
                    u[j + i] = static_cast<uint64_t>(sum);
                    add_carry = sum >> 64;
                }
                u[j + m] += static_cast<uint64_t>(add_carry);
            } else {
                u[j + m] -= carry;
            }

            Q.digits[j] = static_cast<uint64_t>(q_hat);
        }

        shift_right_words(u, s);
        R.digits = u;
    }

    Q.is_negative = (A.is_negative != B.is_negative);
    R.is_negative = A.is_negative;

    Q.remove_leading_zeros();
    R.remove_leading_zeros();
}
