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
	void move(int direction);//0об 1вС 2ио 3ср
	Brick* nextBrick= NULL;
};
#endif // !BRICK_H

