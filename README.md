# STM32_HAL_Projects
Learning the STM32HAL



# 开发流程

## 1. **初始项目**

 `main` 分支作为基础分支 已经拥有支持开发的一切基础配置（OLED配置等）

以及拥有所需要的各种模块的.c/.h 都存放在`User`文件夹内

**所有自己的模块都要在`User`文件夹内！！！**

## 2. **创建/切换新的模块分支**

在每次开发学习新模块/新项目时，你需要 **创建/切换一个新的分支**！

这个分支用于该模块/项目的独立开发。

在**微软大战代码**的界面左下角 有显示当前分支（刚开始是main）

点开 会弹出消息 可以**新建分支或者切换分支**

## 3. **在新分支上开发**

在新创建/切换的分支上进行开发，你可以编辑、添加 `.c` 和 `.h` 文件，或者修改现有文件。

你可以像平常一样编辑代码，**微软大战代码**会自动跟踪你所做的更改。

也可以点开文件夹内的 `.ioc`文件 使用 **CubeMX** 配置stm32

## 4. **提交当前分支的更改**

> 提交前请确保关闭了 **CubeMX** ！！！

- 每次完成开发并确认功能正常后，你需要将代码提交到本地仓库。
  - 打开 **源代码管理** 面板
  - 你会看到 **修改的文件**（例如 `.c` 和 `.h` 文件）列在 **更改**（Changes）列表中
  - 选择需要提交的文件（或直接点击 **全部添加**）
  - 输入提交信息，例如：`更新xxxxxx`
  - 点击 **提交** 按钮来提交更改。

## 5. **推送更改到远程仓库**

- 提交后，你需要将本地的分支推送到远程仓库
  - 点击 **同步更改** 按钮，或者使用命令面板中的 **Git: Push** 操作
  - VSCode 会推送你当前的分支到远程仓库
  - 此时 当下分支就同步完成

## 6. **切换回 `main` 分支**

- 在完成模块开发并推送到远程仓库后，你需要将修改合并到 `main` 分支。
  - 点击 **左下角的分支名称**
  - 选择 **`main` 分支**，切换回 `main` 分支

## 7. **提取需要提取分支中可能需要添加的 `.c` 和 `.h` 文件到 `你需要提取到的` 分支**

> 对于stm32hal的学习 只需同步模块的.c .h即可
>
> 为后续开发提供模块接口

- 在 **命令面板** 中输入 `Git: Checkout to...`，然后选择 **你需要提取到的分支的分支名** 

- 提取指定的 `.c` 和 `.h` 文件到 `你需要提取到的` 分支：

  - 打开 **终端**（`Ctrl +` 或 `Cmd +`）。

  - 执行以下命令：

    ```
    git checkout 需要提取的分支的分支名 -- User/
    ```

    这会从 需要提取的分支 提取 `新的.h` 和 `新的.c` 文件到 `你需要提取到的` 分支。

## 8. **提交合并的更改**

- 返回 **源代码管理** 面板，确保文件已经被提取并出现在待提交的更改中。
- 输入提交信息，例如：`更新模块接口`。
- 点击 **提交** 按钮，提交合并后的更改。

## 9. **推送更新到远程仓库**

- 提交完更改后，点击 **同步更改** 按钮，或者使用命令面板中的 **Git: Push** 操作，将 `main` 分支的更改推送到远程仓库。

## 10. **继续开发其他模块**

- 对每个新的模块/项目，重复从 `main` 分支创建新的分支，进行开发、提交和推送。
- 每次开发完成后，提取相应的 `.c` 和 `.h` 文件到 `main` 分支，并推送到远程仓库。



# 开发注意

## 模块接口

**尽量做成通用性强的！！！**



## C++配置

使用CubeMX+Vscode开发支持C++的

参考https://blog.csdn.net/qq_38961840/article/details/142530594

大概就是使用cubemx的User Actions功能

因为比如main.c

改成cpp之后 再用cubemx生成代码 cubemx只生成.c

所以原理就是 在生成代码之前 把cpp改成c文件 然后生成代码 然后再生成cpp 这样就不会出问题了

AI增强过的代码 使用.bat文件 放入目录中

```bat
@echo off
echo =============^> GeneratorBefore.bat run ^<=============

set "main_c_file=%~dp0Core\Src\main.c"
set "main_cpp_file=%~dp0Core\Src\main.cpp"

echo main_c_file  = "%main_c_file%"
echo main_cpp_file= "%main_cpp_file%"

if exist "%main_cpp_file%" (
    REM If a stale main.c exists, remove it to avoid rename failure
    if exist "%main_c_file%" (
        echo Found existing main.c, deleting it...
        del /f /q "%main_c_file%"
        if errorlevel 1 (
            echo [ERR] Failed to delete main.c
            exit /b 11
        )
        echo Deleted main.c OK
    )

    echo Found main.cpp
    ren "%main_cpp_file%" main.c
    if errorlevel 1 (
        echo [ERR] Rename failed: main.cpp to main.c
        exit /b 12
    )
    echo Rename OK: main.cpp to main.c
) else (
    echo [ERR] main.cpp not found
    exit /b 10
)

echo =============^> GeneratorBefore.bat stop ^<=============

```



```c
@echo off
echo ============= GeneratorAfter.bat run =============

set "main_c_file=%~dp0Core\Src\main.c"
set "main_cpp_file=%~dp0Core\Src\main.cpp"

if exist "%main_c_file%" (
    if exist "%main_cpp_file%" (
        echo [ERR] main.cpp already exists, abort.
        exit /b 20
    )
    ren "%main_c_file%" main.cpp
    if errorlevel 1 (
        echo [ERR] Rename failed.
        exit /b 21
    )
    echo OK renamed main.c to main.cpp
) else (
    echo [ERR] main.c not found
    exit /b 30
)

echo ============= GeneratorAfter.bat stop =============

```



同时也要修改顶层的CMakeLists

因为这个目录下的cmakelists有个

```cmake
# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
```

cubemx生成时 永远都是main.c 需改成cpp才能成功编译 但是每次改很麻烦 使用上面的after好像也不能改

所以就在顶层cmakelists里忽略一下这个这样的操作

**cmakelists代码一同放在下面**



## 文件层级逻辑（知道即可）

Core下的Inc和Src存放由CubeMX自动生成的.h .c

所以记得要打开CubeMX中

Project Manager-Code Generator中的生成.c.h文件选项

然后User下的Inc和Src就存放自己的.h .c

为此有两个CMakelists要修改

这是根目录下的

```cmake
cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# >>> C++ settings (ADD)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# <<<

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Project)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
# >>> REPLACE: enable CXX here
project(${CMAKE_PROJECT_NAME} LANGUAGES C CXX ASM)
# <<<
message("Build type: " ${CMAKE_BUILD_TYPE})

# >>> DELETE: no longer needed because LANGUAGES already includes them
# enable_language(C ASM)
# <<<

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# ---------------------------------------------------------------------------
# Keep only main.cpp: ignore CubeMX-referenced main.c (may not exist)
# MUST be placed before add_subdirectory(cmake/stm32cubemx)
set(MX_MAIN_C ${CMAKE_SOURCE_DIR}/Core/Src/main.c)
set_source_files_properties(${MX_MAIN_C} PROPERTIES
    GENERATED TRUE
    HEADER_FILE_ONLY TRUE
)
# ---------------------------------------------------------------------------

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add user library
add_subdirectory(User)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Src/main.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

target_include_directories(stm32cubemx INTERFACE
    ${CMAKE_SOURCE_DIR}/User/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    user
    stm32cubemx

    # Add user defined libraries
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=soft")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")

```



这是User下的

```cmake
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

```



