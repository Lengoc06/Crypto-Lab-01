#include "BigInt.h"


BigInt::BigInt() {
    // TODO: Khởi tạo giá trị bằng 0
}

BigInt::BigInt(long long value) {
    // TODO: Khởi tạo từ biến giá trị nguyên thông thường
}

BigInt::BigInt(const std::string& str_value) {
    // TODO: Phân tích chuỗi số nguyên tố 512 bit thành cấu trúc mảng
}

void BigInt::remove_leading_zeros() {
    // TODO: Xóa các phần tử mảng bằng 0 dư thừa ở vị trí trọng số cao nhất
}

bool BigInt::operator==(const BigInt& other) const {
    // TODO: Implement kiểm tra bằng nhau
    return false;
}

bool BigInt::operator!=(const BigInt& other) const {
    // TODO: Implement kiểm tra khác nhau
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const {
    // TODO: Implement so sánh nhỏ hơn
    return false;
}

bool BigInt::operator<=(const BigInt& other) const {
    // TODO: Implement so sánh nhỏ hơn hoặc bằng
    return (*this < other) || (*this == other);
}

bool BigInt::operator>(const BigInt& other) const {
    // TODO: Implement so sánh lớn hơn
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    // TODO: Implement so sánh lớn hơn hoặc bằng
    return !(*this < other);
}

BigInt BigInt::operator+(const BigInt& other) const {
    BigInt result;
    // TODO: Cài đặt logic cộng số lớn
    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    BigInt result;
    // TODO: Cài đặt logic trừ số lớn
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result;
    // TODO: Cài đặt logic nhân số lớn
    return result;
}

BigInt BigInt::operator/(const BigInt& other) const {
    BigInt result;
    // TODO: Cài đặt logic chia lấy nguyên số lớn
    return result;
}

BigInt BigInt::operator%(const BigInt& other) const {
    BigInt result;
    // TODO: Cài đặt logic chia lấy phần dư số lớn
    return result;
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
    // TODO: Chuyển đổi cấu trúc mảng nội tại thành định dạng in ra màn hình
    // Cần kiểm tra biến is_negative để in dấu "-" nếu cần
    return os;
}

std::istream& operator>>(std::istream& is, BigInt& num) {
    std::string input;
    is >> input;
    // TODO: Gán chuỗi input vào cấu trúc BigInt
    num = BigInt(input);
    return is;
}