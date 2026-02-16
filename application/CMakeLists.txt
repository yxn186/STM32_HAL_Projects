cmake_minimum_required(VERSION 3.22)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

# 没有源文件也允许生成一个“占位源文件”，避免 add_library 报错
if(APP_SOURCES)
    add_library(application STATIC ${APP_SOURCES})
else()
    set(APP_DUMMY ${CMAKE_CURRENT_BINARY_DIR}/application_dummy.c)
    file(WRITE ${APP_DUMMY}
        "/* Auto-generated placeholder (no sources in application/ yet) */\n"
        "void __application_dummy(void) {}\n"
    )
    add_library(application STATIC ${APP_DUMMY})
endif()

target_link_libraries(application PUBLIC
    project_includes
    stm32cubemx
)
