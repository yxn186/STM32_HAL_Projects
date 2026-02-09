cmake_minimum_required(VERSION 3.22)

add_library(user STATIC)

file(GLOB_RECURSE USER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/Src/*.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/*.cpp
)

target_sources(user PRIVATE ${USER_SOURCES})

target_include_directories(user PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Inc
)

# 让 user 也能用到 HAL/CMSIS 的 include/宏（可按你需求）
target_link_libraries(user PUBLIC stm32cubemx)
