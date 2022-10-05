/*
	Name: table.c
	Copyright: All Rights Free.
	Author: AHT
	Created date: 01.10.2022 18.19
	Modify date: 01.10.2022 18.19
	Description: game table functions of 2048
*/

#include "main.h"

unsigned long **game_table = NULL; /* game_table[x][y] */
unsigned size = 0;

int get_size(void)
{
	do
	{
		printf("Type size of game (3-9): ");
		scanf("%d", &size);
		if(size < 3 || size > 9)
		{
			printf("Invalid size!\n");
		}
	} while(size < 3 || size > 9);
	return size;
}

void create_game_table(void)
{
	int i = 0;
	game_table = (unsigned long**)calloc(sizeof(unsigned long*), size);
	for(i = 0; i < size; i++)
	{
		game_table[i] = (unsigned long*)calloc(sizeof(unsigned long), size);
	}
}

void draw_game_table(void)
{
	int i = 0, k = 0;
	for(k = 0; k < size; k++)
	{
		for(i = 0; i < size*(DIGITS_OF_BLOCK+1)+1; i++)
			putchar('-');
		putchar('\n');
		for(i = 0; i < size; i++)
		{
			unsigned j = 0, digit = find_digit_number(game_table[i][k]);
			printf("|");
			if(game_table[i][k] == 0)
				for(j = 0; j < DIGITS_OF_BLOCK; j++)
				{
					printf(" ");
				}
			else switch(digit)
			{
				case 1:
					printf("   %lu   ", game_table[i][k]);
				break;
				case 2:
					printf("  %lu   ", game_table[i][k]);
				break;
				case 3:
					printf("  %lu  ", game_table[i][k]);
				break;
				case 4:
					printf(" %lu  ", game_table[i][k]);
				break;
				case 5:
					printf(" %lu ", game_table[i][k]);
				break;
				case 6:
					printf("%lu  ", game_table[i][k]);
				break;
				case 7:
					printf("%lu", game_table[i][k]);
				break;
				default:
					printf("%07lu", game_table[i][k]);
				break;
			}
		}
		putchar('|');
		putchar('\n');
	}
	for(i = 0; i < size*(DIGITS_OF_BLOCK+1)+1; i++)
		putchar('-');
	putchar('\n');
}

void delete_game_table(void)
{
	int i = 0;
	for(i = 0; i < size; i++)
	{
		free(game_table[i]);
	}
	free(game_table);
	game_table = NULL;
}
