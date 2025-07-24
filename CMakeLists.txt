cmake_minimum_required(VERSION 3.10)
project(PasswordManager C)

set(CMAKE_C_STANDARD 11)

# Find OpenSSL properly
find_package(OpenSSL REQUIRED)

# Your source files
add_executable(PasswordManager
        main.c passManager.c
        banner.c banner.h
        colors.c colors.h
        utility.c utility.h
        security.c security.h
        seed.c seed.h
)

# Link OpenSSL
target_link_libraries(PasswordManager PRIVATE OpenSSL::Crypto)
