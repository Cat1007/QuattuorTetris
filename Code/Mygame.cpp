#include<Windows.h>
#include<time.h>
#include<cmath>
#include<tchar.h>
#include<stdio.h>
#include<Xinput.h>
#include"Brick.h"
#include"Tetris.h"

#pragma comment(lib, "XInput.lib")

int map[50][50] = {0};
Tetris* usertetris;

Brick* setupbase();
Brick* start = NULL;
Brick* end = NULL;
Brick* begin = NULL;
bool active = false;
int direction = 0;
bool dead = false;
bool realdead = false;
bool startjudge = false;
int goal = 0;
char goalstr[9] = "0";
float bouns = 1;

float power = 1;
bool slow = false;

bool wait = true;

#include<cstdio>
using namespace std;

Brick* basebrickstart = setupbase();

int bagrand();
bool Updatewindow(HWND window);
bool addtetris(Tetris* tetris);
bool checkall(Tetris* tetris);
void paintstart(HDC screen);
void control(XINPUT_STATE gamepadcap, HWND window);

LRESULT CALLBACK
MainWindowCallback(HWND   Window,
	UINT   Message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;

	char s[100]; sprintf_s(s, "%x\n", lParam);
	

	switch (Message)
	{
		
	case WM_DESTROY:
	{
		OutputDebugStringA("WM_DESTROY\n");
	}break;
/*
	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	}break;
*/
	case WM_CLOSE:
	{
		OutputDebugStringA("WM_CLOSE\n");
		PostQuitMessage(0);
	}break;
/*
	case WM_PAINT:
	{
		OutputDebugStringA("WM_PAINT\n");
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		static DWORD Operation = WHITENESS;
		if (Operation == WHITENESS)
		{
			Operation = BLACKNESS;
		}
		else
		{
			Operation = WHITENESS;
		}
		PatBlt(DeviceContext, X, Y, Width, Height, Operation);

		EndPaint(Window, &Paint);
	}break;
*/
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
		{
			if (direction % 4 != 3)
				usertetris->move(1);
			OutputDebugStringA("WM_LEFT\n");
			break;
		}
		case VK_UP:
		{
			if (direction % 4 != 0)
				usertetris->move(2);
			OutputDebugStringA("WM_UP\n");
			break;
		}
		case VK_RIGHT:
		{
			if (direction % 4 != 1)
				usertetris->move(3);
			OutputDebugStringA("WM_RIGHT\n");
			break;
		}
		case VK_DOWN:
		{
			if (direction % 4 != 2)
				usertetris->move(0);
			OutputDebugStringA("WM_DOWN\n");
			break;
		}
		case VK_SPACE:
		{
			usertetris->transfer(1);
			usertetris->generate();
			OutputDebugStringA("WM_SPACE\n");
			OutputDebugString(s);
			break;
		}
		case VK_RETURN:
		{
			if (startjudge == false)
				startjudge = true;

			if (dead == true && realdead == true)
			{
				goal = 0;
				sprintf_s(goalstr, "%d", goal);
				power = 1;
				slow = false;
				wait = true;
				delete usertetris;
				usertetris = NULL;
				active = false;
				direction = 0;
				Brick* next = NULL;
				
				if (start != NULL)
				{
					next = start->nextBrick;
					while (next != NULL)
					{
						delete start;
						start = next;
						next = next->nextBrick;
					}
					start = NULL;
				}

				for (int i = 0; i < 50; i++)
				{
					for (int j = 0; j < 50; j++)
					{
						if (i < 30 && i > 19 && j < 30 && j > 19)
						{
							map[i][j] = 4;
						}
						else
						{
							map[i][j] = 0;
						}
					}
				}
				
				
				dead = false;
				realdead = false;

			}
			break;
		}

		case 0x5A:
		{
			if (slow == false&& power == 1)
			{
				slow = true;
			}
			else
			{
				slow = false;
			}
			OutputDebugStringA("z\n");
			break;
		}
		}
	}

	default:
	{
		/*
		HDC screen = GetDC(Window);
		HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
		SelectObject(screen, brush);
		RECT userec = { 0, 0, 1050, 750 };
		FillRect(screen, &userec, brush);
		userbrick.paint(screen);
		*/
		OutputDebugStringA("default\n");
		result = DefWindowProc(Window, Message, wParam, lParam);
	}break;

	}
	return result;
}

int CALLBACK
WinMain(HINSTANCE hInstance,
	HINSTANCE hPreInstance,
	LPSTR lpCmdline,
	int nCmdShow)
{

	WNDCLASS WindowClass = {};
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = hInstance;
	//	WindowClass.hIcon = ;
	WindowClass.lpszClassName = _T("MyGameClass");//自定义窗口类

	if (RegisterClass(&WindowClass))
	{
		RECT userec = { 0, 0, 750, 750 };
		AdjustWindowRect(&userec, WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX, FALSE);
		HWND WindowHandle =
			CreateWindowEx(
				0,
				WindowClass.lpszClassName,
				_T("QuattuorTetris"),
				WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX,//具有标题栏，最小化和关闭按钮，初始可见
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				userec.right-userec.left,
				userec.bottom-userec.top,
				0,
				0,
				hInstance,
				0);


		SetFocus(WindowHandle);

		XINPUT_STATE gamepadcap;
		ZeroMemory(&gamepadcap, sizeof(gamepadcap));

		paintstart(GetDC(WindowHandle));



		while (!startjudge)
		{
			if (XInputGetState(0, &gamepadcap) == ERROR_SUCCESS)
			{
				control(gamepadcap,WindowHandle);
				OutputDebugString("success\n");
			}
			else
			{
				//OutputDebugString("error\n");
			}

			MSG message;

			if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				if (message.message == WM_QUIT)
					return 0;
				else
				{
					TranslateMessage(&message);
					DispatchMessage(&message);

				}
			}
		}



		if (WindowHandle != NULL)
		{
			int frametrigger = 0;
			int cycletrigger = 0;
			int slowtrigger = 0;
			int recovertrigger = 0;
			int framedelay = 16;
			int cycledelay = 900;
			int slowdelay = 500;
			int tick = 0;
			int recoverdelay = 5000;


			


			MSG message;

			srand(time(NULL));

			while (true)
			{

				


				if (active == false)
				{
					usertetris = NULL;
					usertetris = new Tetris;
					if (!usertetris->setTetris(map, GetDC(WindowHandle), bagrand(), direction % 4))
						realdead = true;
					if (bouns <= 3)
					{
						bouns += 0.03;
					}

					active = true;
				}

				if (power <= 0)
				{
					slow = false;
					power = 0;
				}

				if (power > 1)
				{
					power = 1;
				}


				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) )
				{
					if (message.message == WM_QUIT)
						return 0;
					else
					{
							TranslateMessage(&message);
							DispatchMessage(&message);
					
					}
				}
				else
				{
					tick = GetTickCount();

					if (tick >= slowtrigger)
					{
						if (slow == true)
						{
							power -= 0.1;
							slowtrigger = tick + slowdelay;
						}
						else
						{
							slowtrigger = 0;
						}
					}

					if (tick >= recovertrigger && slow == false)
					{
						if (power < 1)
						{
							power += 0.1;
							recovertrigger = tick + recoverdelay;
						}
					}

					
					
					if (tick >= cycletrigger  && !realdead )
					{
						cycletrigger = tick + cycledelay / bouns * (slow ? 5 : 1);
						int temp = direction % 4;
						int res = usertetris->move(temp);
						if (res == 2)
						{
							if (wait == true)
								wait = false;
							else
							{
								addtetris(usertetris);
								checkall(usertetris);
								wait = true;
							}
							
						}
						else if (res == 0)
						{
							dead = true;
							realdead = true;
						}
					}//游戏循环计时

					if (tick >= frametrigger)
					{
						if (XInputGetState(0, &gamepadcap) == ERROR_SUCCESS)
						{
							control(gamepadcap, WindowHandle);
							OutputDebugString("success\n");
						}
						else
						{
							//OutputDebugString("error\n");
						}
						frametrigger = tick + framedelay;
						Updatewindow(WindowHandle);
					}//画面刷新计时
				}
			}
		}//简单的消息循环
		else
		{
			//logging
		}

	}
	else
	{
		//LOGGING
		
	}

	return 0;
}


bool Updatewindow(HWND window)
{
	HDC screen = GetDC(window);
	HDC buffer = CreateCompatibleDC(screen);
	HBITMAP bufferbmp = CreateCompatibleBitmap(screen, 750, 750);

	SelectObject(buffer, bufferbmp);

	static HBRUSH brush = CreateSolidBrush(RGB(33, 33, 33));
	SelectObject(buffer, brush);
	RECT userec = { 0, 0, 750, 750 };
	FillRect(buffer, &userec, brush);
	static HFONT gfont = CreateFont(25, 0, 0, 0, 0, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("黑体"));

	if (realdead)
	{
		static HFONT dfont = CreateFont(250, 0, 0, 0, 700, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("黑体"));
		SelectObject(buffer, dfont);

		SetTextColor(buffer, RGB(213, 0, 0));
		SetBkMode(buffer, TRANSPARENT);
		DrawText(buffer, _T("菜"), -1, &userec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		BitBlt(screen, 0, 0, 750, 750, buffer, 0, 0, SRCCOPY);

		DeleteObject(buffer);
		DeleteObject(screen);
		DeleteObject(bufferbmp);

		return true;
	}


	SelectObject(buffer, gfont);
	static Brick* now = basebrickstart;
	now = basebrickstart;
	Brick* user = start;

	while (true)
	{
		now->paint(buffer);
		now = now->nextBrick;
		if (now == NULL)
			break;
	}
	//绘制基底方块

	while (user!=NULL)
	{
		user->paint(buffer);
		user = user->nextBrick;
		if (user == NULL)
			break;
	}
	//绘制用户方块

	RECT goalrec = {315,315,435,345};

	SetTextColor(buffer, RGB(253, 216, 53));
	SetBkMode(buffer, TRANSPARENT);
	FillRect(buffer, &goalrec, brush);
	if (slow)
	{
		DrawText(buffer, "The World", -1, &goalrec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		DrawText(buffer, _T(goalstr), -1, &goalrec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	

	RECT powerrec = { 316,361,316 + 118 * power,374 };
	RECT backrec = { 315,360,435,375 };
	FillRect(buffer, &backrec, brush);
	static HBRUSH pbrush = CreateSolidBrush(RGB(253, 216, 53));
	FillRect(buffer, &powerrec, pbrush);



	for (int i = 0; i < 4; i++)
	{
		usertetris->theBricks[i]->paint(buffer);
	}
	//绘制当前操作方块

	BitBlt(screen, 0, 0, 750, 750,buffer, 0, 0, SRCCOPY);

	DeleteObject(buffer);
	DeleteObject(screen);
	DeleteObject(bufferbmp);

	return true;
}


Brick* setupbase()
{
	Brick* start = NULL;
	Brick* now = NULL;
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (i < 30 && i > 19 && j < 30 && j > 19)
			{
				if (start == NULL)
				{
					start = new Brick;
					start->setBrick(map, i, j, RGB(144, 164, 174));
					map[i][j] = 4;
					now = start;
				}
				else
				{
					now->nextBrick = new Brick;
					now = now->nextBrick;
					now->setBrick(map, i, j, RGB(144, 164, 174));
					map[i][j] = 4;
				}
			}
		}
	}

	return start;

}



bool addtetris(Tetris* tetris)
{
	for (int i = 0; i < 4; i++)
	{
		map[tetris->x + tetris->rex[i]][tetris->y + tetris->rey[i]] = 2;
	}


	if (start == NULL)
	{
		start = tetris->theBricks[0];
		start->nextBrick = tetris->theBricks[1];
		start->nextBrick->nextBrick = tetris->theBricks[2];
		start->nextBrick->nextBrick->nextBrick = tetris->theBricks[3];
		end = tetris->theBricks[3];

	}
	else
	{
		end->nextBrick = tetris->theBricks[0];
		end = end->nextBrick;
		end->nextBrick = tetris->theBricks[1];
		end->nextBrick->nextBrick = tetris->theBricks[2];
		end->nextBrick->nextBrick->nextBrick = tetris->theBricks[3];
		end = tetris->theBricks[3];
	}

	direction = direction + 1;

	return true;
}


int bagrand()
{
	static int bag[7] = { 0 };
	static int cur = 6;

	if (cur == 6)
	{
		int temp = 0;

		for (int i = 0; i < 7; i++)
		{
			if (bag[i] == cur)
			{
				temp = i;
			}
		}

		memset(bag, 0, sizeof(bag));

		while (cur != -1)
		{
			int k = rand() % 7;
			if (bag[k] == 0)
			{
				bag[k] = 6 - cur;
				cur = cur - 1;
			}
		}

		cur = 0;

		return temp;
	}

	for (int i = 0; i < 7; i++)
	{
		if (bag[i] == cur)
		{
			cur = cur + 1;
			return i;
		}
	}
}

bool checkall(Tetris* tetris)
{
	int tempx[4];
	int tempy[4];

	for (int i = 0; i < 4; i++)
	{
		tempx[i] = tetris->theBricks[i]->x;
		tempy[i] = tetris->theBricks[i]->y;
	}

	for (int i = 0; i < 4; i++)
	{
		int min;
		int max;

		min = (abs(tempx[i] - 24.5) >= abs(tempy[i] - 24.5)) ? tempx[i] : tempy[i];

		if (min >= 25)
			min = 49 - min;

		max = 49 - min;

		Brick* bs = start;
		int count = 0;
		int total = (max - min) * 4;

		while (bs != NULL)//?
		{
			if (bs->x == min || bs->y == min || bs->x == max || bs->y == max)
			{
				if (bs->x >= min && bs->x <= max && bs->y >= min && bs->y <= max)
				{
					count = count + 1;
				}
			}

			bs = bs->nextBrick;
		}

		bs = start;	//	复位bs指针

		if (total == count)
		{
			for (int i = 0; i < 50; i++)	
			{
				for (int j = 0; j < 50; j++)	//遍历整个二维数组
				{
					if (i <= min || i >= max || j <= min || j >= max)
					{
							map[i][j] = 0;
					}
				}               
			}

			Brick* before = start;

			while (bs != NULL)
			{
				

				if (bs->x == min || bs->x == max || bs->y == min || bs->y == max)	//删除方块
				{
					goal += 10 * bouns;

					if (bs == start)
					{
						before = bs;
						bs = bs->nextBrick;
						start = bs;
						delete before;
						before = bs;
						end = bs;

						continue;
					}

						before->nextBrick = bs->nextBrick;
						delete bs;
						bs = before->nextBrick;
						end = before;

						continue;

				}

				if ((bs->x < min && bs->y < min) || (bs->x > max && bs->y < min) || (bs->x < min && bs->y > max) || (bs->x > max && bs->y > max))	//删除方块
				{

					if (bs == start)
					{ 
						before = bs;
						bs = bs->nextBrick;
						start = bs;
						delete before;
						before = bs;
						end = bs;


						continue;
					}

					before->nextBrick = bs->nextBrick;
					delete bs;
					bs = before->nextBrick;
					end = before;

					continue;
				}

				if (bs->x > min && bs->x < max && bs->y < min)	//下移方块
				{
					bs->setBrick(map, bs->x, bs->y + 1, bs->color);
					map[bs->x][bs->y] = 2;
					before = bs;
					end = bs;
					bs = bs->nextBrick;

					continue;
				}

				if (bs->x > min && bs->x < max && bs->y > max)	//上移方块
				{
					bs->setBrick(map, bs->x, bs->y - 1, bs->color);
					map[bs->x][bs->y] = 2;
					before = bs;
					end = bs;
					bs = bs->nextBrick;

					continue;
				}

				if (bs->y > min && bs->y < max && bs->x < min)	//右移方块
				{
					bs->setBrick(map, bs->x + 1, bs->y, bs->color);
					map[bs->x][bs->y] = 2;
					before = bs;
					end = bs;
					bs = bs->nextBrick;

					continue;
				}

				if (bs->y > min && bs->y < max && bs->x > max)	//左移方块
				{
					bs->setBrick(map, bs->x - 1, bs->y, bs->color);
					map[bs->x][bs->y] = 2;
					before = bs;
					end = bs;
					bs = bs->nextBrick;

					continue;
				}



				before = bs;
				end = bs;
				bs = bs->nextBrick;
				

			}

			sprintf_s(goalstr, "%d", goal);

			if (bouns >= 2)
			{
				bouns -= 1;
			}

			break;
			
		}

	}

	Brick* ss = start;
	while (ss != NULL)
	{
		map[ss->x][ss->y] = 2;
		ss = ss->nextBrick;
	}

	active = false;
	return true;
}





void paintstart(HDC hdc)
{

	HDC screen = CreateCompatibleDC(hdc);
	HBITMAP bufferbmp = CreateCompatibleBitmap(hdc, 750, 750);

	SelectObject(screen, bufferbmp);

	HBRUSH bbrush = CreateSolidBrush(RGB(33, 33, 33));
	RECT userec = { 0, 0, 750, 750 };
	SelectObject(screen, bbrush);
	FillRect(screen, &userec, bbrush);
	DeleteObject(bbrush);

	Brick* temp = new Brick;
	

	{
		int x = 13; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 19; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 20; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 24; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 25; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 26; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 29; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 30; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 34; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 35; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 36; int y = 13;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	} //line one

	{
		int x = 13; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 16; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 14;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line two

	{
		int x = 13; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 16; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 15;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line three

	{
		int x = 13; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 14; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 15; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 16; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 19; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 20; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 24; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 25; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 26; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 29; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 30; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 34; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 35; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 16;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line four

	{
		int x = 13; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 20; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 17;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line five

	{
		int x = 13; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 20; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 36; int y = 18;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line six

	{
		int x = 13; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 24; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 25; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 26; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 29; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 30; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 34; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 35; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 36; int y = 19;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line seven

	{
		int x = 13; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 14; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 15; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 16; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 23; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 24; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 26; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 29; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 30; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 31; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 34; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 35; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 36; int y = 21;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line nine

	{
		int x = 13; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 18; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 19; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 21; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 25; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 28; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}
	{
		int x = 33; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 22;
		temp->setBrick(map, x, y, RGB(245, 245, 245)); 
		temp->paint(screen);
	}	//line ten

	{
		int x = 13; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 19; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 23;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line eleven

	{
		int x = 13; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 29; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 30; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 34; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 24;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line twelve

	{
		int x = 13; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 25;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line ten three

	{
		int x = 13; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 26;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line forteen

	{
		int x = 13; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 29; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 30; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 27;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line fifteen


	{
		int x = 13; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 18; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 19; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 21; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 24; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 29; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 30; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 33; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 34; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 36; int y = 29;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 17

	{
		int x = 13; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 30;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 18

	{
		int x = 13; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 31;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 19

	{
		int x = 13; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 24; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 25; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 29; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 30; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 32;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 20

	{
		int x = 16; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 30; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 33;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 21

	{
		int x = 16; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 34;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 22
	{
		int x = 13; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 14; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 15; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 16; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 20; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 23; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 26; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 28; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 31; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}
	{
		int x = 35; int y = 35;
		temp->setBrick(map, x, y, RGB(245, 245, 245));
		temp->paint(screen);
	}	//line 23

	SetTextColor(screen, RGB(245, 245, 245));
	SetBkMode(screen, TRANSPARENT);
	RECT rec = { 300,570,450,585 };
	DrawText(screen, "THANKS FOR 1007", -1, &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	BitBlt(hdc, 0, 0, 750, 750, screen, 0, 0, SRCCOPY);

	delete temp;
	DeleteObject(screen);
	DeleteObject(bufferbmp);
	
}


void control(XINPUT_STATE gamepadcap,HWND window)
{
	XINPUT_GAMEPAD gamepad = gamepadcap.Gamepad;

	static int pre = 0;
	int x = 0x390001;

	if (gamepad.sThumbLX <= -15000)
	{
		gamepad.wButtons = XINPUT_GAMEPAD_DPAD_LEFT;
	}

	if (gamepad.sThumbLX >= 15000)
	{
		gamepad.wButtons = XINPUT_GAMEPAD_DPAD_RIGHT;
	}

	if (gamepad.sThumbLY >= 15000)
	{
		gamepad.wButtons = XINPUT_GAMEPAD_DPAD_UP;
	}

	if (gamepad.sThumbLY <= -15000)
	{
		gamepad.wButtons = XINPUT_GAMEPAD_DPAD_DOWN;
	}


	if (gamepad.wButtons == XINPUT_GAMEPAD_B && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_SPACE, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_Y && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, 0x5A, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_A && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_RETURN, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_UP, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_DOWN, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_DPAD_LEFT && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_LEFT, x);
	}

	if (gamepad.wButtons == XINPUT_GAMEPAD_DPAD_RIGHT && gamepad.wButtons != pre)
	{
		PostMessage(window, WM_KEYDOWN, VK_RIGHT, x);
	}

	pre = gamepad.wButtons;


	return;
}