/*
	Name: internal.c
	Copyright: All Rights Free.
	Author: AHT
	Created date: 01.10.2022 18.22
	Modify date: 01.10.2022 18.22
	Description: Internal functions of 2048
*/

#include "main.h"

struct tm *tinfo = NULL;

#ifdef __unix__

char getch(void)
{
	char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
 }

void clrscr(void)
{
	system("clear");
}

#elif defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64

void clrscr(void)
{
	system("cls");
}

#endif

unsigned find_digit_number(unsigned long number)
{
	unsigned digit = 1;
	while(number >= 10)
	{
		number /= 10;
		digit++;
	}
	return digit;
}

void set_tinfo(void)
{
	time_t ti = time(NULL);
	if(tinfo != NULL)
		free(tinfo);
	tinfo = localtime(&ti);
	tinfo->tm_mon++;
	tinfo->tm_year+=1900;
}

void *memrev(void *addr, unsigned size) /* return value is address of addr. */
{
	char ch;
	char *temp = (char*)addr;
	int i = 0;
	for(i = 0; i < size/2; i++)
	{
		ch = temp[size-1-i];
		temp[size-1-i] = temp[i];
		temp[i] = ch;
	}
	return addr;
}

unsigned long get_random(void)
{
	#ifdef __MSDOS__
		static unsigned long numb, i;
		union REGS regs = {0};
		regs.h.ah = 0x2c;
		int86(0x21, &regs, &regs);
		numb += regs.h.ch + regs.h.cl + regs.h.dh + regs.h.dl * ++i * ++i;
		return numb;
	#elif defined __unix__
		#define SEC_TO_NS(sec) ((sec)*1000000000)
		static uint64_t nanoseconds, i = 0;
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
		nanoseconds += (SEC_TO_NS((uint64_t)ts.tv_sec) + (uint64_t)ts.tv_nsec) + i++;
		nanoseconds *= ++i;
		return nanoseconds;
	#elif defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
		static unsigned long i = 0;
		SYSTEMTIME st;
		GetSystemTime(&st);
		st.wMilliseconds += ++i;
		return st.wMilliseconds + st.wSecond;
	#endif
}

unsigned get_byte_of_file(char *path)
{
	unsigned size;
	FILE *f = fopen(path, "rb");
	if(f == NULL)
		return 0;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fclose(f);
	return size;
}

unsigned get_line_of_file(char *path)
{
	unsigned line = 0;
	FILE *f = fopen(path, "rb");
	if(f == NULL)
		return 0;
	while(feof(f) == 0)
	{
		if(fgetc(f) == 0x0a)
			line++;
	}
	fclose(f);
	return line;
}

void **CreateArray(unsigned number_of_element, unsigned size_of_element)
{
	unsigned i = 0;
	char **arr = (char **) calloc(sizeof(char*), number_of_element);
	if(arr == NULL)
		return NULL;
	for(i = 0; i < number_of_element; i++)
	{
		arr[i] = (char*) calloc(sizeof(char), size_of_element);
		if(arr[i] == NULL)
		{
			while(i >= 0)
			{
				i--;
				free(arr[i]);
			}
			free(arr);
			return NULL;
		}
	}
	return (void**) arr;
}

void DeleteArray(void **arr, unsigned number_of_element, unsigned size_of_element)
{
	unsigned i = 0;
	for(i = 0; i < number_of_element; i+=size_of_element)
	{
		free((char*) arr[i]);
	}
	free(arr);
}
