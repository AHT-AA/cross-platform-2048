rem if your compiler is not on PATH variable set path for compiler. for example
rem set path=C:\mingw64\bin

gcc main.c game.c  hscr.c internal.c table.c -o 2048.exe -Wall
@PAUSE
