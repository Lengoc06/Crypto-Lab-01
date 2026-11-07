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

    // --- CONSTRUCTORS ---
    BigInt();                               // Khởi tạo giá trị 0
    BigInt(long long value);                // Khởi tạo từ số nguyên
    BigInt(const std::string& str_value);   // Khởi tạo từ chuỗi (để nhập số nguyên tố rất lớn)

    // --- TOÁN TỬ SO SÁNH ---
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;

    // --- TOÁN TỬ SỐ HỌC CƠ BẢN ---
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    // --- TOÁN TỬ GÁN VÀ SỐ HỌC (+=, -=, *=, /=, %=) ---
    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);

    // --- TOÁN TỬ NHẬP XUẤT (I/O) ---
    // Khai báo là hàm friend để truy cập vào thuộc tính của lớp BigInt
    friend std::ostream& operator<<(std::ostream& os, const BigInt& num);
    friend std::istream& operator>>(std::istream& is, BigInt& num);
    
private:
    // Hàm phụ trợ chuẩn hóa mảng (loại bỏ các số 0 vô nghĩa ở đầu)
    void remove_leading_zeros();
};

#endif // BIGINT_H