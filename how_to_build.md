### 1. Hướng dẫn biên dịch và chạy thử (`main.cpp`)

Bạn có thể tạo thư mục build và biên dịch bằng các lệnh sau:

```bash
# 1. Tạo thư mục build và cấu hình
cmake -B build -S .

# 2. Biên dịch toàn bộ
cmake --build build

# 3. Chạy chương trình chính
./build/main

# 4. Chạy kiểm tra PRNG (nếu cần)
./build/main_prng
```

---

### 2. Hướng dẫn biên dịch và chạy đo thời gian (`test_benchmark`)

Để có kết quả đo chính xác và phản ánh đúng hiệu năng thực tế của các thuật toán mã hóa, khuyến nghị bạn biên dịch với chế độ **Release** (bật cờ tối ưu hóa của compiler):

```bash
# 1. Cấu hình thư mục build ở chế độ Release
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. Biên dịch riêng target test_benchmark (hoặc chạy cmake --build build để build tất cả)
cmake --build build --target test_benchmark

# 3. Chạy chương trình đo lường
./build/test_benchmark
```

*(Ghi chú: Nếu chỉ chạy `cmake -B build -S .` thông thường, trình biên dịch sẽ ở chế độ Debug nên thời gian chạy sinh số nguyên tố an toàn sẽ lâu hơn đáng kể so với Release).*

---