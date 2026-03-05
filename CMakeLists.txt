cmake_minimum_required(VERSION 3.5)
project(KNN_Project)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -maes -msse4.2 -march=native")

if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCES libfss/cpp/fss-client.cpp libfss/cpp/fss-server.cpp libfss/cpp/fss-common.cpp
src/secMul.cpp src/secMax.cpp src/secBGtCom src/secBLtCom
src/secEqTest.cpp src/secKMin.cpp src/secFre.cpp src/typeConvert.cpp
src/secED.cpp src/dataShare.cpp src/genTriple.cpp src/testAccFunction.cpp src/testLatenFunction.cpp 
src/testOfflineCost.cpp src/secShuffle.cpp src/util.cpp
src/aes_bridge.cpp 
)
add_executable(main main.cpp ${SOURCES})

target_compile_definitions(main PRIVATE AESNI)

target_link_libraries(main 
    PRIVATE 
    gmpxx gmp ssl crypto dl pthread 
    OpenMP::OpenMP_CXX
)