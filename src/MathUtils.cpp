#include "MathUtils.h"
#include "fips202/prng.h"
#include <cstdint>
#include <stdexcept>
#include <array>

constexpr int MAX_PRIME = 2039;
constexpr int PRIME_COUNT = 308; // Số lượng số nguyên tố lẻ từ 3 đến 2039
//bỏ qua số 2 vì hàm miller_rabin_test có ktra số chẵn hay ko rồi
static constexpr std::array<uint16_t, PRIME_COUNT> generate_primes() {
    std::array<uint16_t, PRIME_COUNT> primes{};
    int index = 0;
    for (int i = 3; i <= MAX_PRIME; i += 2) {
        bool is_prime = true;
        for (int j = 2; j * j <= i; ++j) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) primes[index++] = static_cast<uint16_t>(i);
    }
    return primes;
}
static constexpr auto small_primes = generate_primes();

static uint64_t mod_u64(const BigInt& num, uint64_t m) {
    __uint128_t rem = 0;
    for (size_t i = num.digits.size(); i-- > 0; ) {
        rem = ((rem << 64) | num.digits[i]) % m;
    }
    return static_cast<uint64_t>(rem);
}

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

bool miller_rabin_test(const BigInt& n, int k) {
    if (k <= 0 || n.is_negative || n < BigInt(2)) return false; // k<=0 hoac n<2
    if (n == BigInt(2) || n == BigInt(3)) return true; // bang 2 hoac 3
    if ((n.digits[0] & 1ULL) == 0) return false; // so chan
    //chia cho bang so ngto
    for (uint64_t p : small_primes) {
        if (n.digits.size() == 1 && n.digits[0] == p) {
            return true;
        }
        if (mod_u64(n, p) == 0) {
            return false;
        }
    }

    BigInt n_minus_1 = n - BigInt(1);
    BigInt d = n_minus_1;
    int r = 0;
    while ((d.digits[0] & 1ULL) == 0) {
        d.shift_right_words(d.digits, 1);
        d.remove_leading_zeros();
        r++;
    }

    int bit_len = static_cast<int>(n.digits.size()) * 64;
    BigInt range = n - BigInt(3);

    for (int round = 0; round < k; ++round) {
        BigInt a = (generate_secure_random(bit_len + 64) % range) + BigInt(2);
        BigInt x = modular_exponentiation(a, d, n);
        if (x == BigInt(1) || x == n_minus_1) {
            continue;
        }

        bool is_composite = true;
        for (int i = 1; i < r; ++i) {
            x = (x * x) % n;
            if (x == n_minus_1) {
                is_composite = false;
                break;
            }
            if (x == BigInt(1)) return false;
        }
        if (is_composite) return false;
    }

    return true;
}

BigInt generate_safe_prime(int bit_size) {
    if (bit_size < 3) {
        throw std::invalid_argument("bit_size must be at least 3");
    }

    const int mr_rounds = 40;
    const size_t q_words = static_cast<size_t>((bit_size - 1 + 63) / 64);
    const int q_top_shift = (bit_size - 2) % 64;

    while (true) {
        BigInt q = generate_secure_random(bit_size - 1);
        if (bit_size > 3) q.digits[0] |= 1ULL;
        
        for (int step = 0; step < 2000; ++step, q += BigInt(2)) {
            if (q.digits.size() != q_words || (q.digits.back() >> q_top_shift) != 1ULL) {
                break;
            }

            bool small_div = false;
            if (bit_size >= 16) {
                for (uint64_t l : small_primes) {
                    uint64_t rem = mod_u64(q, l);
                    if (rem == 0 || rem == (l - 1) / 2) {
                        small_div = true;
                        break;
                    }
                }
            }
            if (small_div) {
                continue;
            }

            BigInt p = q + q + BigInt(1);
            if (miller_rabin_test(q, 1) && miller_rabin_test(p, 1) &&
                miller_rabin_test(q, mr_rounds) && miller_rabin_test(p, mr_rounds)) {
                return p;
            }
        }
    }
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