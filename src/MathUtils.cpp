#include "MathUtils.h"

BigInt modular_exponentiation(BigInt base, BigInt exponent, BigInt mod) {
    BigInt result(1);
    // TODO: Cài đặt logic tính (base^exponent) % mod.
    // Lưu ý: Cần xử lý hiệu quả để không bị tràn bộ nhớ hoặc chạy quá chậm với số lớn 512 bit.
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
    // TODO: Cài đặt logic sinh số ngẫu nhiên an toàn trên máy tính.
    // Không sử dụng rand() của thư viện C cũ vì không đủ an toàn cho mật mã.
    return random_num;
}

BigInt generate_private_key(BigInt p) {
    BigInt private_key;
    // TODO: Sử dụng cơ chế sinh số ngẫu nhiên để tạo khóa riêng.
    // Điều kiện bắt buộc: Khóa riêng phải nằm trong khoảng [2, p-2].
    return private_key;
}