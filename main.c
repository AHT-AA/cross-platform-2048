/*
	Name: main.c
	Copyright: All Rights Free.
	Author: AHT-AA
	Created date: 25.09.2022 20.56
	Modify date: 15.10.2022 21.49
	Description: 2048 game
*/
#include "main.h"

void game_exit(char *msg)
{
	#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
	if(msg != NULL)
		MessageBox(hWnd, msg, "2048", MB_OK | MB_ICONINFORMATION);
	ExitProcess(0);
	#else
	if(msg != NULL)
		printf("\n%s\n", msg);
	exit(0);
	#endif
}

int main(void)
{
	#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
		GuiInit();
		return 0;
	#else
		printf("\nWelcome to 2048 game!\n\n");
		get_size();
		create_game_table();
		create_random();
		do
		{
			char ch;
			clrscr();
			draw_game_table();
			printf("Use arrow key to move numbers '%c' to show keys: ", HELPKEY);
			#ifndef __unix__
			if((ch = getch()) == CONTROL)
			{
				switch(getch())
				{
					case UP:
						move(UP);
					break;
					case DOWN:
						move(DOWN);
					break;
					case RIGHT:
						move(RIGHT);
					break;
					case LEFT:
						move(LEFT);
					break;
				}
			}
			else if(ch == ESC || ch == CTRLC)
			{
				game_exit("ESC or CTRL+C pressed.");
			}
			#else
			if((ch = getch()) == CONTROL)
			{
				if((ch = getch()) == ARROW)
				{
					switch(getch())
					{
						case UP:
							move(UP);
						break;
						case DOWN:
							move(DOWN);
						break;
						case RIGHT:
							move(RIGHT);
						break;
						case LEFT:
							move(LEFT);
						break;
					}
				}
				else
				{
					game_exit("Any control key pressed.");
				}
			}
			#endif
			else if(tolower(ch) == HELPKEY)
			{
				#ifndef __unix__
				printf("\n"
						"  s  for save currently game.\n"
						"  l  for load last game.\n"
						"  c  for show high score.\n"
						"  ESC or CTRL+C exit (without save).\n"
						"Press any key to continue game . . .");
				#else
				printf("\n"
						"  s  for save currently game.\n"
						"  l  for load last game.\n"
						"  c  for show high score.\n"
						"  Any control key exit (i.e. Press ESC two times) (without save).\n"
						"Press any key to continue game . . .");
				#endif
				getch();
			}
			else if(tolower(ch) == SAVEKEY)
			{
				save_game();
			}
			else if(tolower(ch) == LOADKEY)
			{
				load_game();
			}
			else if(tolower(ch) == HSCRKEY)
			{
				show_hscr();
			}
		} while(1);
		return 0;
	#endif
}
