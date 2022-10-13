/*
	Name: hscr.c
	Copyright: All Rights Free.
	Author: AHT
	Created date: 02.10.2022 13.54
	Modify date: 02.10.2022 13.54
	Description: High score functions of 2048
*/

#include "main.h"

unsigned hscr_number = 0; /* highscore number that found in file */ /* get_hscr, is_hscr, show_hscr */
HIGHSCORE **hscr = NULL;

int is_hscr(void)
{
	if(get_hscr() == NULL)
		return 1;
	queue_hscr();
	if((hscr_number < HSCR_NUMBER) || get_biggest() > hscr[HSCR_NUMBER-1]->record)
		return 1;
	return 0;
}

HIGHSCORE **queue_hscr(void) /* sirala nin ingilizcesi ne dayi */
{
	unsigned i = 0, k = 0;
	if(hscr == NULL)
		return NULL;
	if(hscr_number == 1) /* No need to loop */
		return hscr;
	for(i = 0; i < hscr_number-1; i++)
	{
		for(k = 0; k < hscr_number-1; k++)
		{
			if(hscr[k+1]->record > hscr[k]->record)
			{
				HIGHSCORE temp;
				memcpy(&temp, hscr[k+1], sizeof(HIGHSCORE));
				memcpy(hscr[k+1], hscr[k], sizeof(HIGHSCORE));
				memcpy(hscr[k], &temp, sizeof(HIGHSCORE));
			}
		}
	}
	return hscr;
}

HIGHSCORE **get_hscr(void)
{
	FILE *f;
	int i = 0;

	hscr_number = get_line_of_file(HSCR_FILENAME)+1; /* highscore number that found in file */

	if(hscr != NULL)
		DeleteArray((void**) hscr, hscr_number, sizeof(HIGHSCORE));
	hscr = (HIGHSCORE**) CreateArray(hscr_number, sizeof(HIGHSCORE)); /* Create an array element number: line number of file, size of element is size of HIGHSCORE data type */

	if((f = fopen(HSCR_FILENAME, "rb")) == NULL || hscr == NULL) /* If file is not exist then it is highscore */
	{
		return NULL;
	}
	i = 0;
	while(feof(f) == 0)
	{
		fscanf(f, "%c %u %lu %d %d %d %d %d ", &hscr[i]->signature, &hscr[i]->size, &hscr[i]->record, &hscr[i]->time.tm_min, &hscr[i]->time.tm_hour, &hscr[i]->time.tm_mday, &hscr[i]->time.tm_mon, &hscr[i]->time.tm_year);
		if(hscr[i]->signature != HSCR_SIGNATURE)
		{
			break;
		}
		fgets(hscr[i]->name, 16, f);
		hscr[i]->name[strlen(hscr[i]->name)-1] = (hscr[i]->name[strlen(hscr[i]->name)-1] == (char)0x0a) ? '\0' : hscr[i]->name[strlen(hscr[i]->name)-1];
		i++;
	}
	fclose(f);
	return hscr;
}

void put_hscr(void)
{
	#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
		PutHighScore();
	#else
		FILE *f;
		char buff[16];
		printf("\nCongratulations! %lu is your score. Type your name (max 16 char): ", get_biggest());
		#ifndef __unix__
			fflush(stdin);
		#else
			for(char ch = 0; (ch = getchar()) != '\n' && ch != '\0' && ch != EOF; )
				;
		#endif
		fgets(buff, 16, stdin);
		buff[strlen(buff)-1] = '\0';

		f = fopen(HSCR_FILENAME, "ab");
		if(f == NULL)
		{
			printf("\nHigh score file can't open. Press any key to continue . . .\n");
			getch();
			return ;
		}
		set_tinfo();
		fprintf(f, "%c %u %lu %d %d %d %d %d %s%c", HSCR_SIGNATURE, size, get_biggest(), tinfo->tm_min, tinfo->tm_hour, tinfo->tm_mday, tinfo->tm_mon, tinfo->tm_year, buff, 0x0a);
		fclose(f);
	#endif
}

void show_hscr(void)
{
	int i = 0;
	if(get_hscr() == NULL)
		return ;
	queue_hscr();
	printf("\n");
	for(i = 0; i < hscr_number-1; i++)
	{
		printf("size: %u,  score: %lu,  date: %02d.%02d.%04d,  time: %02d.%02d,  name: %s\n", hscr[i]->size, hscr[i]->record, hscr[i]->time.tm_mday, hscr[i]->time.tm_mon, hscr[i]->time.tm_year, hscr[i]->time.tm_hour, hscr[i]->time.tm_min, hscr[i]->name);
	}
	printf("Press any key to continue . . .\n");
	getch();
}
