/*
	Name: main.h
	Copyright: All Rights Free.
	Author: AHT
	Created date: 26.09.2022 05.46
	Modify date: 26.09.2022 05.46
	Description: 2048 game header
*/

#ifndef _2048_H_
#define _2048_H_

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#ifdef __unix__
		#include <unistd.h>
		#include <errno.h>
		#include <stdint.h>
		#include <termios.h>
#elif defined __MSDOS__
	#include <dos.h>
	#include <conio.h>
#elif  defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
	#include <windows.h>
	#include <conio.h>
#endif

/* Key defines */
#ifndef __unix__
	#define UP 72
	#define DOWN 80
	#define RIGHT 77
	#define LEFT 75

#else
	#define CONTROL 27
	#define ARROW2 91
	#define UP 'A'
	#define DOWN 'B'
	#define RIGHT 'C'
	#define LEFT 'D'
#endif

#define CTRLC 3
#define ESC 27
#define HELPKEY 'h'
#define SAVEKEY 's'
#define LOADKEY 'l'
#define HSCRKEY 'c'

#define SAVEGAME_FILENAME "SAVEGAME.AHT"
#define SAVEGAME_FILEHEADER "AHT2048SAVE"
#define HSCR_FILENAME "HSCR.AHT"
#define HSCR_SIGNATURE 'H'

#define HSCR_NUMBER 5

#define FULL 1
#define EMPTY 0

#define DIGITS_OF_BLOCK 7

/* Data types */

typedef struct
{
	unsigned char signature;
	unsigned size;
	unsigned long record;
	struct tm time;
	char name[16];
}HIGHSCORE;

/* game variables */

/* internal.c */
extern struct tm *tinfo;

/* hscr.c */
extern unsigned hscr_number; /* highscore number that found in file */ /* get_hscr, is_hscr, show_hscr */
extern HIGHSCORE **hscr;

/* table.c */
extern unsigned long **game_table; /* game_table[x][y] */
extern unsigned size;

/* Function prototypes */

/* main.c */
void game_exit(char *msg); /* print exit message and exit from game. If msg is NULL don't write anything. */

/* game.c */
void save_game(void); /* Save currently game */
void load_game(void); /* Load game */
unsigned long get_biggest(void); /* biggest number of game_Table[][] */
void is_gameover(void);
void create_random(void); /* Create (spawn) random number (2 or 4) on empty object of game_table. */
int find_first_full_index(unsigned long *arr, int size); /* Find first index of full. */
int find_first_empty_index(unsigned long *arr, int size); /* Find first index of empty. */
void compile(int index); /* Don't use this! index can be: UP, DOWN, RIGHT, LEFT */
void move(int index); /* Use this! index can be: UP, DOWN, RIGHT, LEFT */

/* table.c */
int get_size(void); /* get how many square that player want */
void create_game_table(void); /* Memory allocalation for game_table variable. */
void draw_game_table(void); /* draw allocated game_table values. */
void delete_game_table(void); /* delete game table */

/* internal.c */
#ifdef __unix__
	char getch(void);
#endif
void clrscr();
unsigned find_digit_number(unsigned long number); /* Find how many digits of number (decimal) */
void set_tinfo(void);
unsigned long get_random(void); /* generate random number */
void *memrev(void *addr, unsigned size); /* memory reverse. */
unsigned get_byte_of_file(char *path);
unsigned get_line_of_file(char *path);
void **CreateArray(unsigned number_of_element, unsigned size_of_element);
void DeleteArray(void **arr, unsigned number_of_element, unsigned size_of_element);

/* hscr.c */
int is_hscr(void);
HIGHSCORE **queue_hscr(void); /* sirala nin ingilizcesi ne dayi */
HIGHSCORE **get_hscr(void);
void put_hscr(void);
void show_hscr(void); /* show highscore */

#endif /* _2048_H_ */
