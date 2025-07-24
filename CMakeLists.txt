cmake_minimum_required(VERSION 3.31)
project(PasswordManager C)

set(CMAKE_C_STANDARD 11)
set(OPENSSL_ROOT_DIR "/usr")
set(OPENSSL_INCLUDE_DIR "/usr/include")
set(OPENSSL_LIBRARIES "/usr/lib/x86_64-linux-gnu")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add your executable
add_executable(PasswordManager main.c passManager.c
        banner.h
        banner.c
        colors.h
        colors.c
        utility.h
        utility.c
        security.h
        security.c
        seed.h
        seed.c
        main.c
)

# Link OpenSSL libraries
target_link_libraries(PasswordManager PRIVATE OpenSSL::Crypto)
