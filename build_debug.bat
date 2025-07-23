@echo off
set name=simple_ide
if exist "%~dp0%name%.exe" del "%~dp0%name%.exe"
clang  %name%.c -O2 -o "%~dp0%name%.exe" -std=c11 -Wall -Wno-missing-braces -Wno-unused-variable -Wno-unused-function -L  ./lib/  -static  -lcore -lraylib -lopengl32 -lgdi32 -pthread -lwinmm
cmd /c if exist "%~dp0%name%.exe" "%~dp0%name%.exe"