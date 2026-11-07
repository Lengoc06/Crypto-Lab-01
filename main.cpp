#include <iostream>
#include "BigInt.h"
#include "MathUtils.h"

int main() {
    // 1. Sinh số nguyên tố lớn p và phần tử sinh
    int bit_size = 512; // Kích thước bit tối thiểu theo yêu cầu
    BigInt p = generate_safe_prime(bit_size); 
    BigInt g = BigInt(2); // Phần tử sinh

    // 2. Sinh khóa riêng của Alice và Bob
    BigInt a = generate_private_key(p); 
    BigInt b = generate_private_key(p); 

    // 3. Tính giá trị công khai của Alice và Bob
    BigInt A = modular_exponentiation(g, a, p); // A = g^a % p
    BigInt B = modular_exponentiation(g, b, p); // B = g^b % p

    // 4. Tính bí mật chung[cite: 1]
    BigInt alice_shared_secret = modular_exponentiation(B, a, p); // s = B^a % p
    BigInt bob_shared_secret = modular_exponentiation(A, b, p);   // s = A^b % p

    // 5. Hiển thị kết quả và xác minh
    std::cout << "Bí mật chung Alice nhận được: " << alice_shared_secret << "\n";
    std::cout << "Bí mật chung Bob nhận được: " << bob_shared_secret << "\n";
    std::cout << "Quá trình tính toán đúng không? " << (alice_shared_secret == bob_shared_secret) << "\n";

    return 0;
}