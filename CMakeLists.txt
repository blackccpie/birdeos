cmake_minimum_required (VERSION 3.24)
project (birdeos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-D__MACOS__)

set (sources_list
    app/src/main.cpp

    lib/src/gui.cpp
    lib/src/eospp.cpp
)

set (headers_list
    lib/src/gui.h
    lib/src/eospp.h
)

add_executable(birdeos ${sources_list} ${headers_list})

target_compile_options(birdeos PRIVATE "-Wall" "-Wconversion")

target_include_directories(birdeos SYSTEM PRIVATE
    "3rd_parties/EDSDK/Header"
    "3rd_parties/CImg"
    "3rd_parties/darknet"

    "/opt/X11/include"

    "/usr/local/include"
)

target_include_directories(birdeos PRIVATE
    "lib/src"
)

target_link_directories(birdeos PRIVATE
    "3rd_parties/darknet"

    "/opt/X11/lib"

    "/usr/local/lib"
)

find_library(EDSDK_LIBRARY EDSDK "${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/EDSDK/Framework")

target_link_libraries(birdeos PRIVATE
    ${EDSDK_LIBRARY}
    darknet
    X11
    jpeg
)