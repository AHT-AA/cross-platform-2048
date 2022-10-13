rem if your compiler is not on PATH variable set path for compiler. for example
rem set path=C:\mingw64\bin

@del 2048.EXE > NUL
gcc main.c game.c  hscr.c internal.c table.c WINDOWS\gui.c -o 2048.exe -w -mwindows -m32 -std=c11 -s -I .
@PAUSE
