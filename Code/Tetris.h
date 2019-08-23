#ifndef TETRIS_H
#define TETRIS_H

#include<Windows.h>
#include"Brick.h"

class Tetris
{
public:
	Tetris();
	bool setTetris(int(*themap)[50], HDC window, int rand,int direction);
	int move(int mdirection);	//����ֵ˵���Ƿ��ƶ��ɹ�
	int check(int mdirection);				//��鷽�����һλ���Ƿ�Ϸ�
	bool transfer(int time);			//������Σ�������
	bool generate();

	Brick* theBricks[4] = {nullptr};
	int(*map)[50];
	COLORREF color;
	int x;
	int y;
	int rex[4] = { 0,0,0,0 };
	int rey[4] = { 0,0,0,0 };	//���������������������λ��
	int trand;
	int tdirection;
};

#endif // !TETRIS_H