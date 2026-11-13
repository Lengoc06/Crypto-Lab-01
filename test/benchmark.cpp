#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "BigInt.h"
#include "MathUtils.h"

using namespace std;
using namespace std::chrono;

struct BenchmarkStats {
    double min_ms;
    double max_ms;
    double avg_ms;
    double total_ms;
    size_t iterations;
};

BenchmarkStats calculate_stats(const vector<double>& times_ms) {
    if (times_ms.empty()) return {0.0, 0.0, 0.0, 0.0, 0};
    double total = 0.0;
    double min_val = times_ms[0];
    double max_val = times_ms[0];

    for (double t : times_ms) {
        total += t;
        if (t < min_val) min_val = t;
        if (t > max_val) max_val = t;
    }
    return {min_val, max_val, total / times_ms.size(), total, times_ms.size()};
}

void print_header() {
    cout << string(105, '-') << "\n";
    cout << left << setw(38) << "Thuật toán / Tác vụ"
         << right << setw(10) << "Số lần"
         << setw(14) << "Tối thiểu"
         << setw(14) << "Tối đa"
         << setw(14) << "Trung bình"
         << setw(14) << "Tổng cộng" << "\n";
    cout << left << setw(38) << ""
         << right << setw(10) << "(lần)"
         << setw(14) << "(ms)"
         << setw(14) << "(ms)"
         << setw(14) << "(ms)"
         << setw(14) << "(ms)" << "\n";
    cout << string(105, '-') << "\n";
}

void print_row(const string& name, const BenchmarkStats& s) {
    cout << left << setw(38) << name
         << right << setw(10) << s.iterations
         << setw(14) << fixed << setprecision(3) << s.min_ms
         << setw(14) << fixed << setprecision(3) << s.max_ms
         << setw(14) << fixed << setprecision(3) << s.avg_ms
         << setw(14) << fixed << setprecision(3) << s.total_ms << "\n";
}

int main(int argc, char* argv[]) {
    // Cấu hình số lần lặp mặc định
    int n_safe_prime = 100;   // Sinh số nguyên tố an toàn (khá nặng tính toán)
    int n_priv_key   = 1000;  // Sinh khóa riêng
    int n_mod_exp    = 1000;  // Lũy thừa modulo
    int n_e2e        = 100;   // Quy trình Diffie-Hellman đầy đủ

    cout << "=========================================================================\n";
    cout << "  BENCHMARK THỜI GIAN CHẠY THUẬT TOÁN (CRYPTO LAB 01 - C++14)\n";
    cout << "=========================================================================\n";
    cout << "Kích thước bit chuẩn: 512 bit\n\n";

    // -------------------------------------------------------------
    // 1. Benchmark: Sinh số nguyên tố an toàn 512-bit
    // -------------------------------------------------------------
    cout << "[1/4] Đang đo thời gian sinh số nguyên tố an toàn 512-bit (" << n_safe_prime << " lần)...\n";
    vector<double> safe_prime_times;
    BigInt sample_p;

    for (int i = 0; i < n_safe_prime; ++i) {
        cout << "\r   -> Tiến độ: " << (i + 1) << "/" << n_safe_prime << flush;
        auto start = high_resolution_clock::now();
        BigInt p = generate_safe_prime(512);
        auto end = high_resolution_clock::now();

        double elapsed = duration<double, milli>(end - start).count();
        safe_prime_times.push_back(elapsed);
        sample_p = p; // Lưu lại 1 số p để dùng cho các test sau
    }
    cout << " -> Hoàn tất.\n\n";

    // -------------------------------------------------------------
    // 2. Benchmark: Sinh khóa riêng (Private Key)
    // -------------------------------------------------------------
    cout << "[2/4] Đang đo thời gian sinh khóa riêng (" << n_priv_key << " lần)...\n";
    vector<double> priv_key_times;
    BigInt sample_a, sample_b;

    for (int i = 0; i < n_priv_key; ++i) {
        auto start = high_resolution_clock::now();
        BigInt a = generate_private_key(sample_p);
        auto end = high_resolution_clock::now();

        double elapsed = duration<double, milli>(end - start).count();
        priv_key_times.push_back(elapsed);
        if (i == 0) sample_a = a;
        if (i == 1) sample_b = a;
    }
    cout << "   -> Hoàn tất.\n\n";

    // -------------------------------------------------------------
    // 3. Benchmark: Lũy thừa modulo (Modular Exponentiation: g^a % p)
    // -------------------------------------------------------------
    cout << "[3/4] Đang đo thời gian tính lũy thừa modulo (" << n_mod_exp << " lần)...\n";
    vector<double> mod_exp_times;
    BigInt g(2);

    for (int i = 0; i < n_mod_exp; ++i) {
        auto start = high_resolution_clock::now();
        BigInt A = modular_exponentiation(g, sample_a, sample_p);
        auto end = high_resolution_clock::now();

        double elapsed = duration<double, milli>(end - start).count();
        mod_exp_times.push_back(elapsed);
    }
    cout << "   -> Hoàn tất.\n\n";

    // -------------------------------------------------------------
    // 4. Benchmark: Toàn bộ quy trình Diffie-Hellman End-to-End
    // -------------------------------------------------------------
    cout << "[4/4] Đang đo thời gian quy trình Diffie-Hellman toàn phần (" << n_e2e << " lần)...\n";
    vector<double> e2e_times;
    bool all_verified = true;

    for (int i = 0; i < n_e2e; ++i) {
        cout << "\r   -> Tiến độ: " << (i + 1) << "/" << n_e2e << flush;
        auto start = high_resolution_clock::now();

        // Bước 1: Sinh p và g
        BigInt p = generate_safe_prime(512);
        BigInt gen(2);

        // Bước 2: Sinh private key a, b
        BigInt a = generate_private_key(p);
        BigInt b = generate_private_key(p);

        // Bước 3: Tính public key A, B
        BigInt A = modular_exponentiation(gen, a, p);
        BigInt B = modular_exponentiation(gen, b, p);

        // Bước 4: Tính khóa bí mật chung
        BigInt secret_alice = modular_exponentiation(B, a, p);
        BigInt secret_bob   = modular_exponentiation(A, b, p);

        auto end = high_resolution_clock::now();

        if (secret_alice != secret_bob) {
            all_verified = false;
        }

        double elapsed = duration<double, milli>(end - start).count();
        e2e_times.push_back(elapsed);
    }
    cout << " -> Hoàn tất. (Xác minh bí mật chung: " << (all_verified ? "CHÍNH XÁC" : "SAI") << ")\n\n";

    // -------------------------------------------------------------
    // Hiển thị bảng tổng hợp kết quả
    // -------------------------------------------------------------
    BenchmarkStats stats_prime   = calculate_stats(safe_prime_times);
    BenchmarkStats stats_priv    = calculate_stats(priv_key_times);
    BenchmarkStats stats_modexp  = calculate_stats(mod_exp_times);
    BenchmarkStats stats_e2e     = calculate_stats(e2e_times);

    cout << "\nKẾT QUẢ ĐO LƯỜNG HIỆU NĂNG (Đơn vị tính: mili-giây / ms):\n";
    print_header();
    print_row("1. Safe Prime (512-bit)", stats_prime);
    print_row("2. Private Key Gen [2, p-2]", stats_priv);
    print_row("3. Modular Exponentiation (512b)", stats_modexp);
    print_row("4. Full Diffie-Hellman (E2E)", stats_e2e);
    cout << string(105, '-') << "\n";

    return 0;
}
