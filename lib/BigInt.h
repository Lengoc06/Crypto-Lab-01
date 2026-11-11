#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

class BigInt {
public:
    std::vector<uint64_t> digits;
    bool is_negative;

    BigInt();
    BigInt(long long value);
    BigInt(const std::string& str_value);

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);

    void remove_leading_zeros();
    static void shift_left_words(std::vector<uint64_t>& a, int s);
    static void shift_right_words(std::vector<uint64_t>& a, int s);

    friend std::ostream& operator<<(std::ostream& os, const BigInt& num);
    friend std::istream& operator>>(std::istream& is, BigInt& num);

private:
    static int count_leading_zeros(uint64_t x);
    static int compare_magnitude(const BigInt& a, const BigInt& b);
    static BigInt add_magnitude(const BigInt& a, const BigInt& b);
    static BigInt sub_magnitude(const BigInt& a, const BigInt& b);
    static void divmod_internal(const BigInt& A, const BigInt& B, BigInt& Q, BigInt& R);
};

#endif // BIGINT_H