#ifndef BRICK_H
#define BRICK_H

#include<Windows.h>

class Brick
{
public:

	int x;
	int y;
	COLORREF color;
	int(*map)[50];
	bool setBrick(int(*themap)[50],int xp, int yp, COLORREF thecolor);
	void paint(HDC window);
	void move(int direction);//0�� 1�� 2�� 3��
	Brick* nextBrick= NULL;
};
#endif // !BRICK_H

