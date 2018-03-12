#include "unit.h"


//连续的异或运算比依次异或运算速度要快
inline void xor(package *pack, package *pack0, package *pack1, package *pack2, package *pack3, package *pack4, package *pack5, package *pack6, package *pack7)
{
	int i;
	for(i = 0; i < PACKAGE_LENGTH + HEAD_INFO + HEAD_TASK; i++)
		pack->data[i] = pack0->data[i] ^ pack1->data[i] ^ pack2->data[i] ^ pack3->data[i] ^ pack4->data[i] ^ pack5->data[i] ^ pack6->data[i] ^ pack7->data[i];
}

void unit_encode(unit u)
{
	int i, j, k;
	for(i = 0; i < UNIT_LENGTH - 1; i++)
		for(j = 0; j < UNIT_LENGTH - 1; j++)
			package_xor(u[i][j][7], u[i][j][0], u[i][j][1], u[i][j][2], u[i][j][3], u[i][j][4], u[i][j][5], u[i][j][6], u[i][j][7]);

	for(j = 0; j < UNIT_LENGTH - 1; j++)
		for(k = 0; k < UNIT_LENGTH; k++)
			package_xor(u[7][j][k], u[0][j][k], u[1][j][k], u[2][j][k], u[3][j][k], u[4][j][k], u[5][j][k], u[6][j][k], u[7][j][k]);

	for(k = 0; k < UNIT_LENGTH; k++)
		for(i = 0; i < UNIT_LENGTH; i++)
			package_xor(u[i][7][k], u[i][0][k], u[i][1][k], u[i][2][k], u[i][3][k], u[i][4][k], u[i][5][k], u[i][6][k], u[i][7][k]);
}

int unit_decode(unit u)
{
	int x[UNIT_LENGTH] = {0};
	int y[UNIT_LENGTH] = {0};
	int z[UNIT_LENGTH] = {0};
	int i, j, k;

	for(i = 0; i < UNIT_LENGTH; i++)
		for(j = 0; j < UNIT_LENGTH; j++)
			for(k = 0; k < UNIT_LENGTH; k++)
				if(!u[i][j][k]->exist)
					x[i] = y[j] = z[k] = 1;

	int index; //丢弃包的序号
	int num; //丢弃包的数量

	int check, result;
	do
	{
		check = 0;
		result = 0;
		for(i = 0; i < UNIT_LENGTH; i++) if(x[i])
			for(j = 0; j < UNIT_LENGTH; j++) if(y[j])
			{
				num = 0;
				for(k = 0; k < UNIT_LENGTH; k++) if(z[k])
				{
					if(!u[i][j][k]->exist)
					{
						index = k;
						num++;
						if(num > 1) break;
					}
				}
				if(num == 1)
				{
					package_xor(u[i][j][index], u[i][j][0], u[i][j][1], u[i][j][2], u[i][j][3], u[i][j][4], u[i][j][5], u[i][j][6], u[i][j][7]);
					u[i][j][index]->exist = 1;
					check = 1;
				}
				result += num;
			}

		for(j = 0; j < UNIT_LENGTH; j++) if(y[j])
			for(k = 0; k < UNIT_LENGTH; k++) if(z[k])
			{
				num = 0;
				for(i = 0; i < UNIT_LENGTH; i++) if(x[i])
				{
					if(!u[i][j][k]->exist)
					{
						index = i;
						num++;
						if(num > 1) break;
					}
				}
				if(num == 1)
				{
					package_xor(u[index][j][k], u[0][j][k], u[1][j][k], u[2][j][k], u[3][j][k], u[4][j][k], u[5][j][k], u[6][j][k], u[7][j][k]);
					u[index][j][k]->exist = 1;
					check = 1;
				}
				result += num;
			}

		for(k = 0; k < UNIT_LENGTH; k++) if(z[k])
			for(i = 0; i < UNIT_LENGTH; i++) if(x[i])
			{
				num = 0;
				for(j = 0; j < UNIT_LENGTH; j++) if(y[j])
				{
					if(!u[i][j][k]->exist)
					{
						index = j;
						num++;
						if(num > 1) break;
					}
				}
				if(num == 1)
				{
					package_xor(u[i][index][k], u[i][0][k], u[i][1][k], u[i][2][k], u[i][3][k], u[i][4][k], u[i][5][k], u[i][6][k], u[i][7][k]);
					u[i][index][k]->exist = 1;
					check = 1;
				}
				result += num;
			}
	} while(check);

	return result;
}

unit unit_malloc(int side)
{
	unit u;
	int i, j, k;

	u = (package ****)lib_calloc(UNIT_LENGTH * sizeof(package ***));
	for(i = 0; i < UNIT_LENGTH; i++)
	{
		u[i] = (package ***)lib_calloc(UNIT_LENGTH * sizeof(package **));
		for(j = 0; j < UNIT_LENGTH; j++)
		{
			u[i][j] = (package **)lib_calloc(UNIT_LENGTH * sizeof(package *));
			for(k = 0; k < UNIT_LENGTH; k++)
			{
				if(side == SIDE_SEND)
				{
					if(i == UNIT_LENGTH - 1 || j == UNIT_LENGTH - 1 || k == UNIT_LENGTH - 1)
						u[i][j][k] = package_malloc();
				}
				else
					u[i][j][k] = package_malloc();
			}
		}
	}
	return u;
}

void unit_free(unit u, int side)
{
	int i, j, k;
	for(i = 0; i < UNIT_LENGTH; i++)
	{
		for(j = 0; j < UNIT_LENGTH; j++)
		{
			for(k = 0; k < UNIT_LENGTH; k++)
			{
				if(side == SIDE_SEND)
				{
					if(i == UNIT_LENGTH - 1 || j == UNIT_LENGTH - 1 || k == UNIT_LENGTH - 1)
						package_free(u[i][j][k]);
				}
				else
					package_free(u[i][j][k]);
			}
			lib_free(u[i][j]);
		}
		lib_free(u[i]);
	}
	lib_free(u);
}

int unit_reset(unit u)
{
	int i, j, k;
	for(i = 0; i < UNIT_LENGTH; i++)
		for(j = 0; j < UNIT_LENGTH; j++)
			for(k = 0; k < UNIT_LENGTH; k++)
				package_reset(u[i][j][k]);
}
