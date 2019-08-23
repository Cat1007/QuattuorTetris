#include<Windows.h>
#include"Brick.h"

using namespace std;

bool Brick::setBrick(int(*themap)[50], int xp, int yp, COLORREF thecolor)
{
	x = xp;
	y = yp;
	color = thecolor;
	map = themap;
	// *(*(map + x) + y) = 1;
	return true;
}


/*
void Brick::move(int direction)
{
	switch (direction)
	{
		case 0:
		{
			*(*(map + x) + y) -= 1;
			y++;
			*(*(map + x) + y) += 1;
			break;
		}
		case 1:
		{
			*(*(map + x) + y) -= 1;
			x--;
			*(*(map + x) + y) += 1;
			break;
		}
		case 2:
		{
			*(*(map + x) + y) -= 1;
			y--;
			*(*(map + x) + y) += 1;
			break;
		}
		case 3:
		{
			*(*(map + x) + y) -= 1;
			x++;
			*(*(map + x) + y) += 1;
			break;
		}
	}
}
*/

void Brick::paint(HDC screen)
{
	HBRUSH brush;
	HPEN pen;
	COLORREF gray = RGB(96,125,139);
	brush = CreateSolidBrush(color);
	pen = CreatePen(PS_INSIDEFRAME,1,gray);
	SelectObject(screen, brush);
	SelectObject(screen, pen);
	Rectangle(screen, x * 15, y * 15, (x + 1) * 15, (y + 1) * 15);//Ä¬ÈÏ»ÒÉ«Ãè±ß
	DeleteObject(brush);
	DeleteObject(pen);
}