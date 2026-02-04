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

## 7. **提取刚才的分支中的可能需要添加的 `.c` 和 `.h` 文件到 `main` 分支**

> 对于stm32hal的学习 只需同步模块的.c .h即可
>
> 为后续开发提供模块接口

- 在 **命令面板** 中输入 `Git: Checkout to...`，然后选择 **刚才的分支** 

- 提取指定的 `.c` 和 `.h` 文件到 `main` 分支：

  - 打开 **终端**（`Ctrl +` 或 `Cmd +`）。

  - 执行以下命令：

    ```
    git checkout 分支名 -- User/
    ```

    这会从 刚才的分支 提取 `新的.h` 和 `新的.c` 文件到 `main` 分支。

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


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Project)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

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
    # Add user sources here
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

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    user
    stm32cubemx

    # Add user defined libraries
)

```



这是User下的

```cmake
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

```



