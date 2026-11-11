#include "MathUtils.h"
#include "fips202/prng.h"

BigInt modular_exponentiation(BigInt base, BigInt exponent, BigInt mod) {
    if (mod == BigInt(1)) {
        return BigInt(0);
    }

    BigInt result(1);
    base = base % mod;

    while (!(exponent.digits.size() == 1 && exponent.digits[0] == 0)) {
        if (exponent.digits[0] & 1) {
            result = (result * base) % mod;
        }

        exponent.shift_right_words(exponent.digits, 1);
        exponent.remove_leading_zeros();

        base = (base * base) % mod;
    }

    return result;
}

bool miller_rabin_test(BigInt n, int k) {
    // TODO: Cài đặt thuật toán kiểm tra số nguyên tố (Miller-Rabin).
    // Sử dụng hàm modular_exponentiation bên trên để kiểm tra.
    return false;
}

BigInt generate_safe_prime(int bit_size) {
    BigInt prime;
    // TODO: Tự cài đặt logic sinh một số nguyên tố an toàn p.
    // Điều kiện: p là số nguyên tố VÀ (p-1)/2 cũng phải là số nguyên tố.
    // Gợi ý vòng lặp:
    // 1. Dùng generate_secure_random sinh số ngẫu nhiên n bit.
    // 2. Dùng miller_rabin_test để kiểm tra.
    return prime;
}

BigInt generate_secure_random(int bit_size) {
    BigInt random_num;
    if (bit_size <= 0) {
        return random_num;
    }

    uint8_t seed[32];
    randombytes(seed, sizeof(seed));

    prng_ctx ctx;
    prng_seed(&ctx, seed, sizeof(seed));

    int num_words = (bit_size + 63) / 64;
    random_num.digits.resize(num_words);
    random_num.is_negative = false;

    for (int i = 0; i < num_words; ++i) {
        random_num.digits[i] = prng_next_u64(&ctx);
    }

    int rem = bit_size % 64;
    if (rem != 0) {
        random_num.digits.back() &= (1ULL << rem) - 1ULL;
    }

    random_num.digits.back() |= (1ULL << ((bit_size - 1) % 64));

    return random_num;
}

BigInt generate_private_key(BigInt p) {
    int bit_size = p.digits.size() * 64;

    BigInt range = p - BigInt(3);

    BigInt rand_num = generate_secure_random(bit_size);

    BigInt private_key = (rand_num % range) + BigInt(2);

    return private_key;
}