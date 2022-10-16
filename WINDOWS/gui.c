/*
	Name: gui.c
	Copyright: All Rights Free.
	Author: AHT-AA
	Created date: 07.10.2022 19.20
	Modify date: 15.10.2022 21.47
	Description: Windows gui for 2048
*/

#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64

#include "main.h"

#define MidScreenX(X) ((GetSystemMetrics(SM_CXSCREEN) - X) / 2)
#define MidScreenY(Y) ((GetSystemMetrics(SM_CYSCREEN) - Y) / 2)

#define TOTAL_MENU_NUMBER  3

#define GAME_BUTTON_NUMBER 5

unsigned X = 400, Y = 300;

BOOL GameOver = FALSE;

const unsigned BlockSize[7] = {140, 110, 85, 72, 60, 55, 50}; /* BlockSize[size-3] */

const COLORREF BlockColors[20] =
{
	RGB(0x00, 0x96, 0xFF),
	RGB(0xF1, 0x55, 0xa4),
	RGB(0x00, 0x32, 0xc3),
	RGB(0x00, 0xcf, 0xff),
	RGB(0xff, 0xbf, 0x00),
	RGB(0xcd, 0x42, 0xf8),
	RGB(0x00, 0xd4, 0xff),
	RGB(0xfb, 0xe3, 0xff),
	RGB(0x00, 0x65, 0x79),
	RGB(0xff, 0x00, 0x20),
	RGB(0x1a, 0x29, 0x4f),
	RGB(0xdb, 0x00, 0x00),
	RGB(0x85, 0x5a, 0x3c),
	RGB(0x91, 0xc6, 0x9c),
	RGB(0xe7, 0x76, 0x1f),
	RGB(0x10, 0x00, 0x7a),
	RGB(0x00, 0xa5, 0xff),
	RGB(0x73, 0x34, 0xd6),
	RGB(0x83, 0xce, 0x6e),
	RGB(0x0b, 0x68, 0x7a),
};

LPCSTR lpszClassName = "WINDOWS2048GAMECLASSNAME";
HINSTANCE hInstance = NULL;
HWND hWnd = NULL;
HFONT hFont = NULL, hBigFont = NULL;

HWND hChildWnd[MAX_CHILD_WINDOWS] = {NULL};

BOOL ActiveMenu = 0; /* 0 Main menu, 1 Game menu, 2 Ask size menu */

UINT CurX, CurY; /* For play using mouse */

UINT FindCountOfhChild(HWND hWnd)
{
	UINT i = 0;
	for(i = 0; i < MAX_CHILD_WINDOWS; i++)
	{
		if(hWnd == hChildWnd[i])
			return i;
	}
	return -1;
}

VOID ClearChildWindows(VOID)
{
	int i = 0;
	for(i = 0; i < MAX_CHILD_WINDOWS; i++)
	{
		if(hChildWnd[i] != NULL)
		{
			DestroyWindow(hChildWnd[i]);
			hChildWnd[i] = NULL;
		}
	}
	ClearScreen();
}

VOID (*MainMenuButton[3 /* Main menu button number */]) (VOID) = {LoadGame, ShowHighScore, NewGame};

VOID SaveGame(VOID)
{
	int i, k;
	FILE *f = fopen(SAVEGAME_FILENAME, "w");
	if(f == NULL)
	{
		MessageBox(hWnd, "Game can't save", strerror(errno), MB_OK | MB_ICONERROR);
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
	fprintf(f, " ");
	fclose(f);
}

VOID LoadGame(VOID)
{
	int i, k;
	unsigned file_size = get_byte_of_file(SAVEGAME_FILENAME);
	char *buff = (char*) calloc(sizeof(char), file_size);
	FILE *f = fopen(SAVEGAME_FILENAME, "r");

	if(f == NULL || buff == NULL)
	{
		MessageBox(hWnd, "Game can't load", strerror(errno), MB_OK | MB_ICONERROR);
		return ;
	}
	fgets(buff, file_size, f);
	fclose(f);
	if(strncmp(buff, SAVEGAME_FILEHEADER, 11) != 0)
	{
		MessageBox(hWnd, "Save file header is invalid.", "Error", MB_OK | MB_ICONERROR);
		return ;
	}
	if(game_table != NULL)
		delete_game_table(); /* clear memory */
	sscanf(buff+12, "%d", &size);
	create_game_table();
	for(i = 0; i < size; i++)
	{
		for(k = 0; k < size; k++)
		{
			sscanf(buff+14+(k*2)+(i*2*size), "%lu", &game_table[i][k]);
		}
	}
	if(ActiveMenu != 1)
	{
		ActiveMenu = 1;
	}
	ClearScreen();
	SendMessage(hWnd, WM_CREATE, (WPARAM) 0, (LPARAM) 0);
	SendMessage(hWnd, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
}

VOID ShowHighScore(VOID)
{
	if(get_hscr() != NULL)
	{
		if(queue_hscr() != NULL)
		{
			char *buff = calloc(256, hscr_number);
			if(buff == NULL)
				game_exit("Not enought memory.");
			int i = 0;
			for(i = 0; i < hscr_number; i++)
				sprintf( (buff + strlen(buff)), "size: %u,  score: %lu,  date: %02d.%02d.%04d,  time: %02d.%02d,  name: %s\n", hscr[i]->size, hscr[i]->record, hscr[i]->time.tm_mday, hscr[i]->time.tm_mon, hscr[i]->time.tm_year, hscr[i]->time.tm_hour, hscr[i]->time.tm_min, hscr[i]->name);
			MessageBox(hWnd, buff, "High Score", MB_OK | MB_ICONINFORMATION);
			free(buff);
		}
	}
	else
		MessageBox(hWnd, "High score can't show.", strerror(errno), MB_OK | MB_ICONWARNING);
}

VOID Restart(VOID)
{
	delete_game_table();
	create_game_table();
	create_random();
	SendMessage(hWnd, WM_CREATE, (WPARAM) 0, (LPARAM) 0);
}

VOID PutHighScore(VOID)
{
	CHAR name[16];
	MessageBox(hWnd, "Congratulations! You made your new high score. Enter your name lower right box. (Max 16 character)", "High Score", MB_OK | MB_ICONINFORMATION);
	hChildWnd[GAME_BUTTON_NUMBER] = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE, 500, 350, 130, 30, hWnd, (HMENU) 0, NULL, NULL);
	hChildWnd[GAME_BUTTON_NUMBER+1] = CreateWindow("BUTTON", "Ok", WS_CHILD | WS_VISIBLE, 500, 385, 130, 30, hWnd, (HMENU) 0, NULL, NULL);
	SetFocus(hChildWnd[GAME_BUTTON_NUMBER]);
}

VOID ReturnMainMenu(VOID)
{
	ClearChildWindows();
	delete_game_table();
	ActiveMenu = 0;
	SendMessage(hWnd, WM_CREATE, (WPARAM) 0, (LPARAM) 0);
}

VOID NewGame(VOID)
{
	ClearChildWindows();
	ActiveMenu = 2;
	SendMessage(hWnd, WM_CREATE, (WPARAM) 0, (LPARAM) 0);
}

VOID DrawBlocks(VOID)
{
	int x = 0, y = 0, BlckSz = BlockSize[size-3];

	HDC hDc = GetDC(hWnd);

	HPEN hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
	SelectObject(hDc, hPen);

	for(x = 0; x < size; x++)
		for(y = 0; y < size; y++)
		{
			HBRUSH hBrush = game_table[x][y] == 0 ? CreateSolidBrush(RGB(128, 128, 128)) : CreateSolidBrush(BlockColors[(int)log2(game_table[x][y]) % 20]);
			SelectObject(hDc, hBrush);

			Rectangle(hDc, BlckSz*x, BlckSz*y, BlckSz*(x+1), BlckSz*(y+1));

			DeleteObject(hBrush);

			if(game_table[x][y] != 0)
			{
				__putnumb(BlckSz*x + (BlckSz/2 - find_digit_number(game_table[x][y])*4 - 3 ), BlckSz*y + (BlckSz-21)/2, (INT) game_table[x][y], RGB(255, 255, 255), BlockColors[(int)log2(game_table[x][y]) % 20]);
			}
		}

	DeleteObject(hPen);
	ReleaseDC(hWnd, hDc);
}

LRESULT CALLBACK MenuProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const char *ButtonTexts[3] = {"Load Game", "Show High Score", "New Game"};
	int i;
	X = 400, Y = 300;
	switch(msg)
	{
		case WM_CREATE:
			SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			MoveWindow(hWnd, MidScreenX(X), MidScreenY(Y), X, Y, TRUE);
			for(i = 0; i < 3; i++)
			{
				hChildWnd[i] = CreateWindow("BUTTON", ButtonTexts[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 90+i*60, 375, 50, hWnd, (HMENU) 0, NULL, NULL);
			}
			EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
		break;
		case WM_COMMAND:
			MainMenuButton[FindCountOfhChild((HWND) lParam)] ();
		break;
		case WM_PAINT:
			__putstr(100, 20, "Welcome to 2048 game!", RGB(255, 255, 255), BACKGROUND_COLOR);
		break;
		case WM_CLOSE:
			ExitProcess(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK GameProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	VOID (*ButtonProcs[GAME_BUTTON_NUMBER]) (VOID) = {SaveGame, LoadGame, ShowHighScore, Restart, ReturnMainMenu};
	X = 640, Y = 480;
	switch(msg)
	{
		case WM_CREATE:
			GameOver = FALSE;
			SendMessage(hWnd, WM_SETFONT,(WPARAM)hFont, TRUE);
			MoveWindow(hWnd, MidScreenX(X), MidScreenY(Y), X, Y, TRUE);
			ClearChildWindows();

			{
				char *ButtonTexts[GAME_BUTTON_NUMBER] = {"Save Game", "Load Game", "Show High Scores", "Restart", "Return Main Menu"};
				int i = 0;
				for(i = 0; i < GAME_BUTTON_NUMBER; i++)
					hChildWnd[i] = CreateWindow("BUTTON", ButtonTexts[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 40+i*60, 130, 50, hWnd, (HMENU) 0, NULL, NULL);
			}
			if(game_table == NULL)
			{
				create_game_table();
				create_random();
			}
		break;
		case WM_COMMAND:
		{
			UINT uWndCount = FindCountOfhChild((HWND) lParam);
			if(uWndCount < GAME_BUTTON_NUMBER && uWndCount >= 0)
			{
				ButtonProcs[uWndCount] ();
				SetFocus(hWnd);
			}
			else if(hChildWnd[GAME_BUTTON_NUMBER+1] != NULL && (HWND) lParam == hChildWnd[GAME_BUTTON_NUMBER+1]) /* Ok button on highscore */
			{
				CHAR name[16] = {0};
				GetWindowText(hChildWnd[GAME_BUTTON_NUMBER], name, 16);

				if(strlen(name) == 0 || name[0] == ' ') /* Empty name is not allowed */
				{
					MessageBox(hWnd, "Empty name is not allowed.\nFirst character of name can't be space.", "Warning", MB_OK | MB_ICONWARNING);
					break;
				}

				FILE *f = fopen(HSCR_FILENAME, "ab");
				if(f == NULL)
				{
					MessageBox(hWnd, "High score file can't open.", strerror(errno), MB_OK | MB_ICONERROR);
					break;
				}
				set_tinfo();
				fprintf(f, "%c %u %lu %d %d %d %d %d %s%c", HSCR_SIGNATURE, size, get_biggest(), tinfo->tm_min, tinfo->tm_hour, tinfo->tm_mday, tinfo->tm_mon, tinfo->tm_year, name, 0x0a);
				fclose(f);
				DestroyWindow(hChildWnd[GAME_BUTTON_NUMBER]);
				DestroyWindow(hChildWnd[GAME_BUTTON_NUMBER+1]);
				hChildWnd[GAME_BUTTON_NUMBER] = NULL;
				hChildWnd[GAME_BUTTON_NUMBER+1] = NULL;
			}
		}
		break;
		case WM_PAINT:
			DrawBlocks();
		break;
		case WM_LBUTTONDOWN:
			CurX = LOWORD(lParam);
			CurY = HIWORD(lParam);
		break;
		case WM_LBUTTONUP:
		{
			UINT tempX = LOWORD(lParam), tempY = HIWORD(lParam);
			if(GameOver == FALSE)
			{
				if((tempX >= CurX-20 && tempX <= CurX+20) && (tempY >= CurY-20 && tempY <= CurY+20)) /* Nothing */
					break;
				else if(tempX > CurX && (abs(tempX - CurX) >= abs(tempY - CurY)) ) /* Right */
					move(RIGHT);
				else if(tempX < CurX && (abs(tempX - CurX) >= abs(tempY - CurY)) ) /* Left */
					move(LEFT);
				else if(tempY > CurY && (abs(tempY - CurY) >= abs(tempX - CurX)) ) /* Down */
					move(DOWN);
				else if(tempY < CurY && (abs(tempY - CurY) >= abs(tempX - CurX)) ) /* Up */
					move(UP);
				SendMessage(hWnd, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
			}
		}
		break;
		case WM_KEYUP:
			if(GameOver == FALSE)
			{
				switch((int)wParam)
				{
					case VK_UP:
						move(UP);
					break;
					case VK_DOWN:
						move(DOWN);
					break;
					case VK_RIGHT:
						move(RIGHT);
					break;
					case VK_LEFT:
						move(LEFT);
					break;
				}
				SendMessage(hWnd, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
			}
		break;
		case WM_CLOSE:
			ExitProcess(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK AskSizeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	X = 400, Y = 300;
	switch(msg)
	{
		case WM_CREATE:
			SendMessage(hWnd, WM_SETFONT,(WPARAM)hFont, TRUE);

			hChildWnd[0] = CreateWindow("EDIT", "3", WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_BORDER | ES_CENTER, 150, 90, 96, 96, hWnd, (HMENU) 0, NULL, NULL);

			hChildWnd[1] = CreateWindow("BUTTON", "Start Game", WS_CHILD | WS_VISIBLE | BS_CENTER, 150, 190, 100, 30, hWnd, (HMENU) 1, NULL, NULL);

			__putstr(140, 60, "Select Size (3-9)", RGB(255, 255, 255), BACKGROUND_COLOR);

			EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
			SendMessage(hChildWnd[0], WM_SETFONT, (WPARAM) hBigFont, TRUE);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case 1:
				{
					char buff[2] = {'\0'};
					GetWindowText(hChildWnd[0], buff, 2);

					if(buff[0] > '9' || buff[0] < '3')
					{
						MessageBox(hWnd, "Size is invalid.", "Warning", MB_OK | MB_ICONWARNING);
						break;
					}
					size = buff[0] - '0';
					ActiveMenu = 1;
					SendMessage(hWnd, WM_CREATE, (WPARAM) 0, (LPARAM) 0);
				}
				break;
			}
		break;
		case WM_CLOSE:
			ExitProcess(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK (*WndProcs[TOTAL_MENU_NUMBER]) (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = {MenuProc, GameProc, AskSizeProc};

LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return WndProcs[ActiveMenu](hWnd, msg, wParam, lParam);
}

VOID GuiInit(VOID)
{
	MSG msg;
	WNDCLASS wc;

	if(hFont == NULL)
		hFont = CreateFont(21, 0, 0, 0, FW_NORMAL, 0, 0, 0, TURKISH_CHARSET, 0, 0, 0, 0, "Arial Black");
	if(hBigFont == NULL)
		hBigFont = CreateFont(96, 0, 0, 0, FW_NORMAL, 0, 0, 0, TURKISH_CHARSET, 0, 0, 0, 0, "Arial Black");
	if(hInstance == NULL)
		hInstance = GetModuleHandle(NULL);

	wc.hbrBackground = (HBRUSH)CreateSolidBrush(BACKGROUND_COLOR);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = MainProc;
	wc.lpszClassName = lpszClassName;
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if((RegisterClass(&wc)) == 0)
	{
		ExitSys("Registeration Error");
	}
	hWnd = CreateWindow(lpszClassName, "2048", (WS_VISIBLE | WS_OVERLAPPEDWINDOW) & (~WS_THICKFRAME) & (~WS_MAXIMIZEBOX), MidScreenX(X), MidScreenY(Y), X, Y, NULL, 0, hInstance, NULL);
	if(hWnd == NULL)
	{
		ExitSys("Window Creation Error");
	}
	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);
	while(GetMessage(&msg, 0 ,0 ,0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

#else
#error This code can only compile under Windows!
#endif
