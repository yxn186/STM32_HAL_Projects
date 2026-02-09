@echo off
echo =============^> GeneratorAfter.bat run ^<=============

set "main_c_file=%~dp0Core\Src\main.c"
set "main_cpp_file=%~dp0Core\Src\main.cpp"

echo main_c_file  = "%main_c_file%"
echo main_cpp_file= "%main_cpp_file%"

if exist "%main_c_file%" (
    echo Found main.c

    REM Safety: if main.cpp already exists, do NOT overwrite it
    if exist "%main_cpp_file%" (
        echo [ERR] main.cpp already exists. Rename main.c to main.cpp would fail.
        echo [ERR] Fix: ensure GeneratorBefore ran successfully, or delete/rename the old main.cpp manually.
        exit /b 20
    )

    ren "%main_c_file%" main.cpp
    if errorlevel 1 (
        echo [ERR] Rename failed: main.c to main.cpp
        exit /b 21
    )

    echo Rename OK: main.c to main.cpp
) else (
    echo [ERR] main.c not found
    exit /b 30
)

echo =============^> GeneratorAfter.bat stop ^<=============
