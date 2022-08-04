cmake_minimum_required(VERSION 3.0.0)
project(cnsoftbei)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(ASIO_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 ")

aux_source_directory(./cpp cppfiles)
aux_source_directory(./head headfiles)

add_executable(cnsoftbei main.cpp ${cppfiles} ${headfiles} cpp/AdvancedCalculator.cpp head/AdvancedCalculator.h)

target_compile_options(cnsoftbei PUBLIC -DASIO_STANDALONE)
target_include_directories(cnsoftbei PUBLIC ${ASIO_INCLUDE})

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(cnsoftbei PUBLIC -lwsock32 -lws2_32)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

find_package(Threads REQUIRED)
target_link_libraries(cnsoftbei PUBLIC Threads::Threads)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
