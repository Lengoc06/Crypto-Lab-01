Analyzed MathUtils.cpp:L55-145
Analyzed MathUtils.cpp:L30-56
Ran check shift_right_words definition
Analyzed BigInt.cpp:L190-250
Analyzed BigInt.cpp:L420-493

Dưới đây là phân tích chi tiết về kết quả đo lường và các đề xuất tối ưu cho nhóm của bạn:

---

### 1. Thời gian hiện tại có cần phải cải thiện không?

> **Kết luận:** **KHÔNG BẮT BUỘC**. Kết quả hiện tại đã ở mức **rất xuất sắc** cho một đồ án môn học (Crypto Lab).

* **Đánh giá thực tế:** 
  * Thuật toán sinh **Safe Prime 512-bit** ($p = 2q + 1$ với cả $p$ và $q$ đều là số nguyên tố) là một tác vụ rất nặng. Việc nhóm tự xây dựng thư viện BigInt từ đầu (cơ số $2^{64}$, chia Knuth Algorithm D) mà đạt thời gian trung bình **~350ms – 400ms** là rất ấn tượng.
  * Hầu hết các đồ án sinh viên tự viết BigInt (dùng `std::string` hoặc mảng byte cơ số 10) thường mất từ **vài giây đến vài chục giây** cho một số 512-bit.
  * Toàn bộ quy trình Diffie-Hellman hoàn thành trong dưới 1 giây nên khi chạy demo hoặc chấm bài tự động sẽ hoàn toàn mượt mà, không gặp hiện tượng treo hay timeout.

---

### 2. Thuật toán hiện tại có thể tối ưu hóa hơn nữa không?

> **Kết luận:** **HOÀN TOÀN CÓ THỂ**. Nếu nhóm muốn tối ưu thêm để lấy điểm cộng hoặc viết vào phần *"Đánh giá hiệu năng & Hướng phát triển"* trong báo cáo, dưới đây là các điểm nghẽn (bottlenecks) và giải pháp cụ thể:

---

#### Mức 1: Tối ưu tham số & Logic thuật toán *(Dễ làm – Hiệu quả cao nhất)*

1. **Điều chỉnh số vòng kiểm tra Miller-Rabin (`mr_rounds`)**:
   * *Hiện tại:* Đang để cố định `mr_rounds = 40`. Khi tìm được một cặp $(q, p)$, code chạy Miller-Rabin 40 vòng cho $q$ và 40 vòng cho $p$ (tổng cộng 80 phép lũy thừa lớn 512-bit).
   * *Tối ưu:* Theo chuẩn **NIST SP 800-56A / FIPS 186-4**, xác suất sai số của Miller-Rabin sau $k$ vòng với cơ số ngẫu nhiên là $\le 4^{-k} = 2^{-2k}$. Với số 512-bit trong bài tập thực hành, chỉ cần **$k = 5$ đến $8$ vòng** là xác suất sai sót đã nhỏ hơn $2^{-80}$ (đủ an toàn tuyệt đối cho thực nghiệm). Việc giảm từ 40 xuống 8 vòng có thể giúp bước xác thực cuối cùng **nhanh hơn gấp 4 đến 5 lần**.

2. **Tránh kiểm tra trùng lặp số nguyên tố nhỏ**:
   * *Hiện tại:* Trong hàm `generate_safe_prime`, ứng viên $q$ đã được sàng qua mảng `small_primes`. Nhưng khi gọi `miller_rabin_test(q, ...)`, bên trong hàm này lại lặp qua `small_primes` và gọi `mod_u64` một lần nữa.
   * *Tối ưu:* Có thể tách hoặc bỏ qua bước chia thử số nguyên tố nhỏ bên trong `miller_rabin_test` nếu biết trước số đó đã được sàng lọc.

3. **Mở rộng bảng số nguyên tố nhỏ (`small_primes`)**:
   * *Hiện tại:* Bảng `small_primes` lọc đến 2039 (308 số).
   * *Tối ưu:* Mở rộng sàng lên **1000 – 2000 số nguyên tố đầu tiên** (đến khoảng 8000 – 16000). Chi phí chia thử các số nhỏ là cực kỳ rẻ so với phép lũy thừa lớn, nhưng nó giúp loại bỏ đến **> 90%** các hợp số trước khi phải gọi đến Miller-Rabin.

---

#### Mức 2: Tối ưu kỹ thuật Sàng số dư (Sieve Optimization) *(Trung bình – Giảm 40-60% thời gian tìm kiếm)*

* *Hiện tại:* Mỗi khi tăng $q \leftarrow q + 2$ (`q += BigInt(2)`), code lại thực hiện 308 phép chia `mod_u64(q, l)` trên toàn bộ 512-bit của $q$.
* *Tối ưu (Incremental Remainder Sieve):* 
  * Thay vì chia lại toàn bộ số lớn $q$: Ta chỉ tính số dư ban đầu $r_l = q \pmod l$ cho từng số nguyên tố $l$.
  * Mỗi bước tăng $q$ lên 2, số dư mới chỉ đơn giản là:
    $$r_l \leftarrow (r_l + 2) \pmod l$$
    *(Đây là phép toán trên số nguyên 16-bit / 64-bit thông thường, tốc độ tính theo nano-giây, hoàn toàn không cần gọi bất kỳ hàm BigInt nào trong vòng lặp sàng).*

---

#### Mức 3: Tối ưu phép Lũy thừa modulo (`modular_exponentiation`) *(Nâng cao)*

1. **Thay đổi cách duyệt bit (Left-to-Right thay vì Right-to-Left)**:
   * *Hiện tại:* Thuật toán đang dùng `exponent.shift_right_words(exponent.digits, 1)` và `exponent.remove_leading_zeros()` liên tục 512 lần. Việc này làm phát sinh nhiều thao tác sao chép / dịch chuyển mảng vector trong bộ nhớ.
   * *Tối ưu:* Đọc trực tiếp các bit từ trái sang phải (từ bit cao nhất xuống bit 0) thông qua chỉ số mảng `digits` mà **không cần chỉnh sửa hay dịch chuyển vector `exponent`**.
2. **Thuật toán khử Modulo nâng cao**:
   * *Hiện tại:* Mỗi phép nhân lớn đều dùng toán tử `%` (gọi giải thuật chia dài Knuth Algorithm D). Phép chia nhiều từ là phép toán đắt nhất trong số học số lớn.
   * *Tối ưu:* Áp dụng **Montgomery Multiplication** hoặc **Barrett Reduction** để biến phép chia modulo thành các phép nhân và dịch bit, tốc độ tính lũy thừa có thể tăng thêm 2 – 3 lần.

---

### 3. Đề xuất hành động cho nhóm

1. **Về mã nguồn nộp bài**: Giữ nguyên trạng thái hiện tại. Code đã chạy đúng đắn, đáp ứng chuẩn C++14, cấu trúc sạch sẽ và hiệu năng ~400ms là đã rất tốt.
2. **Về bài báo cáo (Report)**: 
   * Nhóm có thể đưa bảng số liệu benchmark từ file `test/benchmark.cpp` vào phần **"Kết quả thực nghiệm"**.
   * Đưa các phân tích về *Số vòng Miller-Rabin, Incremental Sieve, và Montgomery Multiplication* ở trên vào phần **"Hướng phát triển và tối ưu hóa"**. Điều này sẽ giúp báo cáo có chiều sâu học thuật và được đánh giá rất cao.
