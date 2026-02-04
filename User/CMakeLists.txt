add_library(user STATIC)

# 自动收集 User/Src 下所有 .c（新增文件后更容易自动感知）
file(GLOB_RECURSE USER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/Src/*.c
)

target_sources(user PRIVATE
    ${USER_SOURCES}
)

# 让别的 target include "oled.h" 这种时能找到 User/Inc
target_include_directories(user PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Inc
)

# 让 user 也能用 HAL / Core 的头文件和编译宏（比如 STM32F103xB）
target_link_libraries(user PUBLIC
    stm32cubemx
)
