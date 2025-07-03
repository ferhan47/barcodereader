cmake_minimum_required(VERSION 3.10)
project(advancebarcode)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# OpenCV'yi bul
find_package(OpenCV REQUIRED)

# ZXing'in kendi kurduğun dizinine CMake'in bakmasını sağla
set(CMAKE_PREFIX_PATH "$ENV{HOME}/zxing-cpp-install")
find_package(ZXing CONFIG REQUIRED)

add_executable(advancebarcode main.cpp)

target_link_libraries(advancebarcode PRIVATE
        ${OpenCV_LIBS}
        ZXing::ZXing
)
