cmake_minimum_required(VERSION 3.15)
project(CryptoLab01 C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Thư viện hỗ trợ PRNG (Keccak / SHAKE128)
add_library(prng_lib STATIC
    lib/fips202/fips202.c
    lib/fips202/prng.cpp
)

target_include_directories(prng_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/fips202
)

# Liên kết thư viện mã hóa hệ thống trên Windows nếu cần
if(WIN32)
    target_link_libraries(prng_lib PUBLIC bcrypt)
endif()

# Thư viện BigInt và MathUtils
add_library(crypto_lib STATIC
    src/BigInt.cpp
    src/MathUtils.cpp
)

target_include_directories(crypto_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/fips202
)

target_link_libraries(crypto_lib PUBLIC prng_lib)

# Chương trình chính (main.cpp)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE crypto_lib)

# Chương trình kiểm thử PRNG (src/main_prng.cpp)
add_executable(main_prng src/main_prng.cpp)
target_link_libraries(main_prng PRIVATE prng_lib)

# Chương trình kiểm thử hiệu năng và thời gian chạy thuật toán (test/benchmark.cpp)
add_executable(test_benchmark test/benchmark.cpp)
target_link_libraries(test_benchmark PRIVATE crypto_lib)
