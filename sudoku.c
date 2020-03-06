#include <stdio.h>

int a[9][9]; //sudoku map
//a[x][y]

void refresh()
{
	for (int i = 0; i < 9; i++)
	{
		for (int t = 0; t < 9; t++)
		{
			a[i][t] = 0;
		}
	}
}

int errorSearch() //return 1 = error
{
	for (int i = 0; i < 9; i++)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int y = x + 1; y < 9; y++)
			{
				if (a[i][x] != 0 && a[i][y] != 0 && (a[i][x] == a[i][y] || a[i][x] + 10 == a[i][y] || a[i][x] == a[i][y] + 10))
				{
					return 1;
				}
			}
		}
	}
	for (int i = 0; i < 9; i++)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int y = x + 1; y < 9; y++)
			{
				if (a[x][i] != 0 && a[y][i] != 0 && (a[x][i] == a[y][i] || a[x][i] + 10 == a[y][i] || a[x][i] == a[y][i] + 10))
				{
					return 1;
				}
			}
		}
	}
	int area[9];
	int t = 0;
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int i2 = 0; i2 < 3; i2++)
				{
					area[t] = a[x * 3 + i][y * 3 + i2];
					t++;
				}
			}
			t = 0;
			for (int p1 = 0; p1 < 8; p1++)
			{
				for (int p2 = p1 + 1; p2 < 9; p2++)
				{
					if (area[p1] != 0 && area[p2] != 0 && (area[p1] == area[p2] || area[p1] + 10 == area[p2] || area[p1] == area[p2] + 10))
					{
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

void print()
{
	int c = 0, t = 0;
	printf("\n | 1 2 3 | 4 5 6 | 7 8 9\n");
	for (int i3 = 0; i3 < 3; i3++)
	{
		printf("-+-------+-------+-------\n");

		for (int i2 = 0; i2 < 3; i2++)
		{
			printf("%c", t + 65);
			for (int i = 0; i < 12; i++)
			{
				if (i % 4 == 0)
				{
					printf("|");
				}
				else
				{
					if (a[c][t] > 10)
					{
						printf("%d", a[c][t] - 10);
					}
					else if (a[c][t] == 0)
					{
						printf(" ");
					}
					else
					{
						printf("%d", a[c][t]);
					}
					c++;
				}
				printf(" ");
			}
			printf("\n");
			c = 0;
			t++;
		}
	}
}

void sudoku()
{
	int p[2] = {0, 0}; //pivot coordinate value
	int key = 0;
	while (a[p[0]][p[1]] != 0)
	{
		p[0]++;
		if (p[0] == 9)
		{
			p[0] = 0;
			p[1]++;
			if (p[1] == 9)
				return;
		}
	}
	while (key != 1)
	{
		a[p[0]][p[1]]++;
		if (a[p[0]][p[1]] == 10)
		{
			a[p[0]][p[1]] = 0;
			while (a[p[0]][p[1]] > 10 || a[p[0]][p[1]] == 0)
			{
				p[0]--;
				if (p[0] < 0)
				{
					p[0] = 8;
					p[1]--;
				}
			}
		}
		else if (errorSearch() == 0)
		{
			while (a[p[0]][p[1]] != 0)
			{
				p[0]++;
				if (p[0] == 9)
				{
					p[0] = 0;
					p[1]++;
					if (p[1] == 9)
					{
						key = 1;
						break;
					}
				}
			}
		}
	}
	return;
}

void op()
{
	char xy[3] = {'0', '0'};
	int x, y, v;
	int exit = 0;
	refresh();
	print();
	printf("\nPlease enter coordinate value you want to put the number.\nex) A1\nIf you are ready to get the solution, enter 0.\n");
	while (exit != 1)
	{
		printf("\n> Location : ");
		scanf("%s", xy);
		if (xy[0] == '0')
		{
			exit = 1;
		}
		else if (xy[0] < 65 || xy[0] > 73 || xy[1] < 49 || xy[1] > 57)
		{
			printf("Erorr. Please try again.\n");
		}
		else
		{
			x = (int)xy[1] - 49;
			y = (int)xy[0] - 65;
			printf("> Value : ");
			scanf("%d", &v);
			if (v == 0)
			{
				a[x][y] = v;
				printf("%c%c is removed.\n", xy[0], xy[1]);
				print();
			}
			else if (v < 1 || v > 9)
			{
				printf("#erorr(Invalid value)\nPlease try again.\n");
			}
			else
			{
				a[x][y] = v + 10;
				if (errorSearch())
				{
					a[x][y] = 0;
					printf("#erorr(Duplicate value)\nPlease try again.\n");
				}
				print();
			}
		}
	}
	sudoku();
	printf("\nSolution :\n");
	print();
}

int main(void)
{
	printf("\nSUDOKU ALGORITHM @ alpha 1.0\n");
	while (1)
	{
		printf("\n--------------------------------------------------\n");
		op();
	}
}