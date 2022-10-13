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

#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64

VOID ExitSys(LPCSTR lpszMsg)
{
	DWORD dwLastError = GetLastError();
	LPTSTR lpszErr;
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpszErr, 0, NULL))
	{
		MessageBox(NULL, lpszErr, lpszMsg, MB_OK | MB_ICONEXCLAMATION);
		LocalFree(lpszErr);
	}
	ExitProcess(dwLastError);
}

VOID __putnumb(UINT x, UINT y, INT numb, COLORREF text_color, COLORREF bk_color)
{
	LPSTR lpszBuff = (LPSTR) calloc(sizeof(CHAR), 16);
	itoa(numb, lpszBuff, 10);
	__putstr(x, y, lpszBuff, text_color, bk_color);
	free(lpszBuff);
}

VOID __putch(UINT x, UINT y, CHAR ch, COLORREF text_color, COLORREF bk_color)
{
	HDC hDc = GetDC(hWnd);

	CHAR lpszTempBuff[2] = {0};
	lpszTempBuff[0] = ch;

	SetTextColor(hDc, text_color);
	SetBkColor(hDc, bk_color);

	TextOut(hDc, x, y, lpszTempBuff, 1);
}

VOID __putstr(UINT x, UINT y, LPCSTR lpszText, COLORREF text_color, COLORREF bk_color)
{
	HDC hDc = GetDC(hWnd);

	SelectObject(hDc, hFont);

	if(text_color != 0)
		SetTextColor(hDc, text_color);
	if(bk_color != 0)
		SetBkColor(hDc, bk_color);
	TextOut(hDc, x, y, lpszText, strlen(lpszText));

	ReleaseDC(hWnd, hDc);
}

BOOL CALLBACK SetFont(HWND child, LPARAM font)
{
  SendMessage(child, WM_SETFONT, font, TRUE);

  return TRUE;
}

VOID ClearScreen(VOID)
{
	HDC hDc = GetDC(hWnd);
	HBRUSH hBrush = CreateSolidBrush(BACKGROUND_COLOR);
	HPEN hPen = CreatePen(PS_SOLID, 0, BACKGROUND_COLOR);

	SelectObject(hDc, hPen);
	SelectObject(hDc, hBrush);

	Rectangle(hDc, 0, 0, X, Y);

	DeleteObject(hBrush);
	DeleteObject(hPen);
}

#endif

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
