#include<Windows.h>
#include"Tetris.h"

Tetris::Tetris()
{
	;
}


bool Tetris::setTetris(int(*themap)[50], HDC window, int rand, int direction)
{
	rex[0] = 0;
	rey[0] = 0;		//相对坐标
	trand = rand;
	tdirection = direction;

	map = themap;
	switch (direction)//上右下左0123 用户方块生成方向
	{
	case 0: {x = 24; y = 0 ; break; }
	case 1: {x = 49; y = 24; break; }
	case 2: {x = 24; y = 49; break; }
	case 3: {x = 0 ; y = 24; break; }
	} 

	switch (rand)
	{
	case 0:
	{
		color = RGB(173, 20, 87); //pink
		rex[1] = 1; rex[2] = 1; rex[3] = 0;
		rey[1] = 0; rey[2] = 1; rey[3] = 1;
		break;
		/*
			10
			00
		*/
	}
	case 1:
	{
		color = RGB(103, 58, 183); //purple
		rex[1] = 1; rex[2] = 1; rex[3] = -1;
		rey[1] = 0; rey[2] = 1; rey[3] = 0;
		break;
		/*
			010
			  0
		*/
	}
	case 2:
	{
		color = RGB(33, 150, 243); //blue
		rex[1] = 1; rex[2] = -1; rex[3] = -1;
		rey[1] = 0; rey[2] = 1; rey[3] = 0;
		break;
		/*
			010
			0
		*/
	}
	case 3:
	{
		color = RGB(76, 175, 80); //green
		rex[1] = 2; rex[2] = 1; rex[3] = -1;
		rey[1] = 0; rey[2] = 0; rey[3] = 0;
		break;
		/*
			0100
		*/
	}
	case 4:
	{
		color = RGB(198, 40, 40); //red
		rex[1] = 1; rex[2] = 0; rex[3] = -1;
		rey[1] = 1; rey[2] = 1; rey[3] = 0;
		break;
		/*
			01
			 00
		*/
	}
	case 5:
	{
		color = RGB(255, 87, 34); //orange
		rex[1] = 1; rex[2] = 0; rex[3] = -1;
		rey[1] = 0; rey[2] = 1; rey[3] = 0;
		break;
		/*
			010
			 0
		*/
	}
	case 6:
	{
		color = RGB(255, 235, 59); //yellow
		rex[1] = 1; rex[2] = 0; rex[3] = -1;
		rey[1] = 0; rey[2] = 1; rey[3] = 1;
		break;
		/*
			10
		   00
		*/
	}
	}

	if (!transfer(direction))
		return false;
	else
	{
		generate();
		return true;
	}

}

bool Tetris::generate()	//生成brick对象，并设置其属性，且串联成链表
{
	for(int i = 0; i < 4; i++)
	{
		theBricks[i] = new Brick;
		theBricks[i]->setBrick(map, x + rex[i], y + rey[i], color);

		if (i != 0)
		{
			theBricks[i - 1]->nextBrick = theBricks[i];
		}
	}

	return true;
}

bool Tetris::transfer(int time)
{
	if (time == 0)
	{
		return true;
	}
	int tempx[4];
	int tempy[4];

	for (int u = 0; u < 4; u++)
	{
		tempx[u] = rex[u];
		tempy[u] = rey[u];
	}

	for (int j = 0; j < time; j++)
	{
		for (int i = 1; i < 4; i++)
		{
			if (tempx[i] == 0 && tempy[i] == -1)
			{
				tempx[i] = 1; tempy[i] = 0;
			}

			else if (tempx[i] == 1 && tempy[i] == 0)
			{
				tempx[i] = 0; tempy[i] = 1;
			}

			else if (tempx[i] == 0 && tempy[i] == 1)
			{
				tempx[i] = -1; tempy[i] = 0;
			}

			else if (tempx[i] == -1 && tempy[i] == 0)
			{
				tempx[i] = 0; tempy[i] = -1;
			}

			else if (tempx[i] == -1 && tempy[i] == -1)
			{
				tempx[i] = 1;tempy[i] = -1;
			}

			else if (tempx[i] == 1 && tempy[i] == -1)
			{
				tempx[i] = 1; tempy[i] = 1;
			}

			else if (tempx[i] == 1 && tempy[i] == 1)
			{
				tempx[i] = -1; tempy[i] = 1;
			}

			else if (tempx[i] == -1 && tempy[i] == 1)
			{
				tempx[i] = -1; tempy[i] = -1;
			}

			else if (tempx[i] == 2 && tempy[i] == 0)
			{
				tempx[i] = 0; tempy[i] = 2;
			}

			else if (tempx[i] == 0 && tempy[i] == 2)
			{
				tempx[i] = -2; tempy[i] = 0;
			}

			else if (tempx[i] == -2 && tempy[i] == 0)
			{
				tempx[i] = 0; tempy[i] = -2;
			}

			else if (tempx[i] == 0 && tempy[i] == -2)
			{
				tempx[i] = 2; tempy[i] = 0;
			}
		}
	}

	for (int k = 0; k < 4; k++)
	{
		if (map[x + tempx[k]][y + tempy[k]] == 2 || map[x + tempx[k]][y + tempy[k]] == 4)
			return false;

		if (x + tempx[k] < 0 || x + tempx[k] > 49 || y + tempy[k] < 0 || y + tempy[k] > 49)
			return false;

	}

	for (int k = 0; k < 4; k++)
	{
		rex[k] = tempx[k];
		rey[k] = tempy[k];
		delete theBricks[k];
	}

	return true;
}


int Tetris::check(int mdirection)
{
	int mtempx[4];
	int mtempy[4];

	switch (mdirection)
	{
	case 0:
	{
		for (int i = 0; i < 4; i++)
		{
			mtempx[i] = x + rex[i];
			mtempy[i] = y + rey[i] + 1;
		}
		break;
	}
	case 1:
	{
		for (int i = 0; i < 4; i++)
		{
			mtempx[i] = x + rex[i] - 1;
			mtempy[i] = y + rey[i];
		}
		break;
	}
	case 2:
	{
		for (int i = 0; i < 4; i++)
		{
			mtempx[i] = x + rex[i];
			mtempy[i] = y + rey[i] - 1;
		}
		break;
	}
	case 3:
	{
		for (int i = 0; i < 4; i++)
		{
			mtempx[i] = x + rex[i] + 1;
			mtempy[i] = y + rey[i];
		}
		break;
	}
	}
	for (int i = 0; i < 4; i++)
	{
		if (mtempx[i] < 0 || mtempx[i] > 49)
			return 0;
		if (mtempy[i] < 0 || mtempy[i] > 49)
			return 0;

		if (*(*(map + mtempx[i]) + mtempy[i]) == 2 || *(*(map + mtempx[i]) + mtempy[i]) == 4)
		{
			return 2;
		}
	}
	return 1;
}

int Tetris::move(int mdirection)
{
	switch (this->check(mdirection))
	{
	case 0:
	{
		return 0;
	}
	case 1:
	{
		for (int i = 0; i < 4; i++)
		{
			*(*(map + x + rex[i]) + y + rey[i]) = 0;
		}

		switch (mdirection)
		{
		case 0: {y++; break; }
		case 1: {x--; break; }
		case 2: {y--; break; }
		case 3: {x++; break; }
		}

		for (int i = 0; i < 4; i++)
		{
			//*(*(map + x + rex[i]) + x + rey[i]) = 1;
			//theBricks[i]->setBrick(map, x + rex[i], y + rey[i], color);
			theBricks[i]->x = x + rex[i];
			theBricks[i]->y = y + rey[i];
		}

		return 1;
	}
	case 2:
	{
		return 2; 
	}
	}
}

