#ifndef TETRIS_H
#define TETRIS_H

#include<Windows.h>
#include"Brick.h"

class Tetris
{
public:
	Tetris();
	bool setTetris(int(*themap)[50], HDC window, int rand,int direction);
	int move(int mdirection);	//布尔值说明是否移动成功
	int check(int mdirection);				//检查方块的下一位置是否合法
	bool transfer(int time);			//方块变形（次数）
	bool generate();

	Brick* theBricks[4] = {nullptr};
	int(*map)[50];
	COLORREF color;
	int x;
	int y;
	int rex[4] = { 0,0,0,0 };
	int rey[4] = { 0,0,0,0 };	//储存其他方块与基块的相对位置
	int trand;
	int tdirection;
};

#endif // !TETRIS_H