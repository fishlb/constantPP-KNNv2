cmake_minimum_required(VERSION 3.0)
project(FSS_Project)

set(dbg "-g")

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -maes -msse4.2 -fno-strict-aliasing -pedantic -Wall -Wextra -Wunreachable-code -Wmissing-declarations -Wunused-function -Wno-overlength-strings -Wno-deprecated-declarations -O3 ${dbg}")

set(LDFLAGS "-L/usr/lib/x86_64-linux-gnu -Wl,-Bstatic -lcrypto -Wl,-Bdynamic -ldl -lssl -lgmpxx -lgmp -lpthread")

set(SOURCES fss-client.cpp fss-server.cpp fss-common.cpp fss-test.cpp)

add_executable(fss-test ${SOURCES})

target_link_libraries(fss-test ${LDFLAGS})

if(AESNI)
    target_compile_definitions(fss-test PRIVATE AESNI)
endif()
