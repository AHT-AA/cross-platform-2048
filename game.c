/*
	Name: game.c
	Copyright: All Rights Free.
	Author: AHT-AA
	Created date: 01.10.2022 18.10
	Modify date: 15.10.2022 21.48
	Description: game functions of 2048
*/

#include "main.h"

static int flag = 0;

void save_game(void)
{
	int i, k;
	FILE *f = fopen(SAVEGAME_FILENAME, "w");
	if(f == NULL)
	{
		printf("Game can't save! Press any key to continue game . . .\n");
		getch();
		return ;
	}
	fputs(SAVEGAME_FILEHEADER, f);
	fprintf(f, " %d", size);
	for(i = 0; i < size; i++)
	{
		for(k = 0; k < size; k++)
		{
			fprintf(f, " %lu", game_table[i][k]);
		}
	}
	fclose(f);
}

void load_game(void)
{
	int i, k;
	char buff[64];
	FILE *f = fopen(SAVEGAME_FILENAME, "r");

	if(f == NULL)
	{
		printf("\nGame can't load! Press any key to continue game . . .\n");
		getch();
		return ;
	}

	fgets(buff, strlen(SAVEGAME_FILEHEADER)+1, f);
	if(strncmp(buff, SAVEGAME_FILEHEADER, strlen(SAVEGAME_FILEHEADER)+1) != 0)
	{
		printf("\nSave file header is invalid. Press any key to continue . . .\n");
		getch();
		return ;
	}
	if(game_table != NULL)
		delete_game_table(); /* clear memory */
	fscanf(f, "%d", &size);
	create_game_table();
	for(i = 0; i < size; i++)
	{
		for(k = 0; k < size; k++)
		{
			fscanf(f, "%lu", &game_table[i][k]);
		}
	}
	fclose(f);
}

unsigned long get_biggest(void) /* biggest number of game_table[][] */
{
	int x, y;
	unsigned long numb = 0;
	for(x = 0; x < size; x++)
	{
		for(y = 0; y < size; y++)
		{
			if(numb < game_table[x][y])
				numb = game_table[x][y];
		}
	}
	return numb;
}

void is_gameover(void)
{
	unsigned x, y ,is_find = 0;
	for(x = 0; x < size; x++) /* If all of the index are full then exit from game. */
	{
		for(y = 0; y < size; y++)
		{
			if(game_table[x][y] == 0)
				is_find = 1;
		}
	}
	if(is_find == 0) /* if it full */
	{
		for(x = 0; x < size-1; x++)
		{
			for(y = 0; y < size-1; y++)
			{
				if(game_table[x][y] == game_table[x+1][y] || game_table[x][y] == game_table[x][y+1])
					is_find = 1;
			}
		}
		for(y = 0; y < size-1; y++) /* last column */
		{
			if(game_table[size-1][y] == game_table[size-1][y+1])
				is_find = 1;
		}
		for(x = 0; x < size-1; x++) /* last row */
		{
			if(game_table[x][size-1] == game_table[x+1][size-1])
				is_find = 1;
		}
	}
	if(is_find == 0)
	{
		#if ( __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64)
			SetWindowLong(hChildWnd[0], GWL_STYLE, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED); /* Disable save game because game over! */
			GameOver = TRUE;
		#endif
		if(is_hscr() != 0)
			put_hscr();
		#if !( __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64)
			game_exit("Game over BYE!");
		#else
			else /* If score is not highscore */
			{
				MessageBox(hWnd, "Game over!", "2048", MB_OK | MB_ICONINFORMATION);
			}
		#endif
	}
}

void create_random(void)
{
	unsigned x, y, is_find = 0;

	while(is_find == 0) /* If it is not empty try another */
	{
		x = get_random() % size, y = get_random() / size % size;
		if(game_table[x][y] == 0)
		{
			is_find = 1;
			game_table[x][y] = ((get_random() % 2 == 0) ? 2 : 4); /* If empty make it 2 or 4 */
		}
	}
}

int find_first_full_index(unsigned long *arr, int size)
{
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(arr[i] != EMPTY)
			return i;
	}
	return -1;
}

int find_first_empty_index(unsigned long *arr, int size)
{
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(arr[i] == EMPTY)
			return i;
	}
	return -1;
}

void compile(int index) /* index can be: UP, DOWN, RIGHT, LEFT */
{
	int x, y;
	if(index == UP)
	{
		for(x = 0; x < size; x++)
		{
			if(find_first_full_index(game_table[x], size) == -1) /*If empty can't compress any number. */
				continue;
			for(y = 0; y < size-1; y++)
			{
				int temp;
				if(game_table[x][y] == game_table[x][y+1]) /* If number is same */
				{
					game_table[x][y] *= 2;
					for(temp = y+1; temp <= size-1; temp++)
					{
						if(temp == size-1)
							game_table[x][temp] = EMPTY;
						else
							game_table[x][temp] = game_table[x][temp+1];
					}
					if(game_table[x][y] != 0)
						flag = 1;
				}
			}
		}
	}
	else if(index == DOWN)
	{
		for(x = 0; x < size; x++)
		{
			if(find_first_full_index(game_table[x], size) == -1) /*If empty can't compress any number. */
				continue;
			memrev(game_table[x], size*sizeof(unsigned long));
			for(y = 0; y < size-1; y++)
			{
				int temp;
				if(game_table[x][y] == game_table[x][y+1]) /* If number is same */
				{
					game_table[x][y] *= 2;
					for(temp = y+1; temp <= size-1; temp++)
					{
						if(temp == size-1)
							game_table[x][temp] = EMPTY;
						else
							game_table[x][temp] = game_table[x][temp+1];
					}
					if(game_table[x][y] != 0)
						flag = 1;
				}
			}
			memrev(game_table[x], size*sizeof(unsigned long));
		}
	}
	else if(index == RIGHT)
	{
		unsigned long temp[9] = {0};
		int i = 0;
		for(y = 0; y < size; y++)
		{
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin gecici degiskene ihtiyacimiz var. */
			{
				temp[i] = game_table[size-1-i][y];
			}
			if(find_first_full_index(temp, size) == -1)
				continue;
			for(x = 0; x < size-1; x++)
			{
				if(temp[x] == temp[x+1]) /* If number is same */
				{
					temp[x] *= 2;
					for(i = x+1; i <= size-1; i++)
					{
						if(i == size-1)
							temp[i] = EMPTY;
						else
							temp[i] = temp[i+1];
					}
					if(temp[x] != 0)
						flag = 1;
				}
				for(i = 0; i < size; i++) /* UP'a uyarlamak icin geri dondurduk. */
				{
					game_table[i][y] = temp[size-1-i];
				}
			}
		}
	}
	else if(index == LEFT)
	{
		unsigned long temp[9] = {0};
		int i = 0;
		for(y = 0; y < size; y++)
		{
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin gecici degiskene ihtiyacimiz var. */
			{
				temp[i] = game_table[i][y];
			}
			if(find_first_full_index(temp, size) == -1)
				continue;
			for(x = 0; x < size-1; x++)
			{
				if(temp[x] == temp[x+1]) /* If number is same */
				{
					temp[x] *= 2;
					for(i = x+1; i <= size-1; i++)
					{
						if(i == size-1)
							temp[i] = EMPTY;
						else
							temp[i] = temp[i+1];
					}
					if(temp[x] != 0)
						flag = 1;
				}
			}
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin geri dondurduk. */
			{
				game_table[i][y] = temp[i];
			}
		}
	}
}

void move(int index) /* index can be: UP, DOWN, RIGHT, LEFT */
{
	int x, y;
	flag = 0;
	if(index == UP)
	{
		for(x = 0; x < size; x++)
		{
			if(find_first_empty_index(game_table[x], size) == -1)
				continue;
			for(y = 0; y < size-1; y++)
			{
				int temp_full = find_first_full_index(&game_table[x][y], size-y);
				int temp_empty = find_first_empty_index(&game_table[x][y], size-y);
				if(temp_empty < temp_full)
				{
					game_table[x][temp_empty+y] = game_table[x][temp_full+y];
					game_table[x][temp_full+y] = EMPTY;
					flag = 1;
				}
			}
		}
	}
	else if(index == DOWN)
	{
		for(x = 0; x < size; x++)
		{
			if(find_first_empty_index(game_table[x], size) == -1)
				continue;
			memrev(game_table[x], size*sizeof(unsigned long));
			for(y = 0; y < size-1; y++)
			{
				int temp_full = find_first_full_index(&game_table[x][y], size-y);
				int temp_empty = find_first_empty_index(&game_table[x][y], size-y);
				if(temp_empty < temp_full)
				{
					game_table[x][temp_empty+y] = game_table[x][temp_full+y];
					game_table[x][temp_full+y] = EMPTY;
					flag = 1;
				}
			}
			memrev(game_table[x], size*sizeof(unsigned long));
		}
	}
	else if(index == RIGHT)
	{
		unsigned long temp[9] = {0};
		int i = 0;
		for(y = 0; y < size; y++)
		{
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin gecici degiskene ihtiyacimiz var. */
			{
				temp[i] = game_table[size-1-i][y];
			}
			if(find_first_empty_index(temp, size) == -1)
				continue;
			for(x = 0; x < size-1; x++)
			{
				int temp_full = find_first_full_index(&temp[x], size-x);
				int temp_empty = find_first_empty_index(&temp[x], size-x);
				if(temp_empty < temp_full)
				{
					temp[temp_empty+x] = temp[temp_full+x];
					temp[temp_full+x]= EMPTY;
					flag = 1;
				}
			}
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin geri dondurduk. */
			{
				game_table[i][y] = temp[size-1-i];
			}
		}
	}
	else if(index == LEFT)
	{
		unsigned long temp[9] = {0};
		int i = 0;
		for(y = 0; y < size; y++)
		{
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin gecici degiskene ihtiyacimiz var. */
			{
				temp[i] = game_table[i][y];
			}
			if(find_first_empty_index(temp, size) == -1)
				continue;
			for(x = 0; x < size-1; x++)
			{
				int temp_full = find_first_full_index(&temp[x], size-x);
				int temp_empty = find_first_empty_index(&temp[x], size-x);
				if(temp_empty < temp_full)
				{
					temp[temp_empty+x] = temp[temp_full+x];
					temp[temp_full+x]= EMPTY;
					flag = 1;
				}
			}
			for(i = 0; i < size; i++) /* UP'a uyarlamak icin geri dondurduk. */
			{
				game_table[i][y] = temp[i];
			}
		}
	}
	compile(index);
	is_gameover();
	if(flag == 1)
	{
		create_random();
	}
}
