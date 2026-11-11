#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "BigInt.h"

// Thành viên 2: Tính lũy thừa mô-đun: (base^exponent) % mod
BigInt modular_exponentiation(BigInt base, BigInt exponent, BigInt mod);

// Thành viên 2: Sinh khóa riêng ngẫu nhiên trong khoảng [2, p-2]
BigInt generate_private_key(BigInt p);

// Thành viên 3: Kiểm tra tính nguyên tố (Miller-Rabin)
bool miller_rabin_test(const BigInt& n, int k);

// Thành viên 3: Sinh số nguyên tố lớn an toàn
BigInt generate_safe_prime(int bit_size);

// Thành viên 4: Sinh số ngẫu nhiên an toàn trên máy tính
BigInt generate_secure_random(int bit_size);

#endif