#ifdef ONLINE_JUDGE

/*
https://acm.sjtu.edu.cn/OnlineJudge/problems
gcc oj.c -o oj.exe -O2 -Wall -static -DONLINE_JUDGE -DONLINE_JUDGE_1000
gcc oj.c -o oj.exe -DONLINE_JUDGE -DONLINE_JUDGE_1000
输出千万别加fflush(stdout)，会极大的提高运行时间，着重强调
大型数组（百万级）放在函数中会出错，需要设置为全局变量
正常来说，乘法不一定需要用移位来替换，因为编译器在处理常量乘法时会自动优化成移位操作
*/
#ifdef ONLINE_JUDGE_1000

#include <stdio.h>

int main(int argc, char *args[])
{
	int a, b;
	scanf("%d %d", &a, &b);
	printf("%d", a + b);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1001

#include <stdio.h>

int main(int argc, char *args[])
{
	int h1, h2, h, n, i;
	int tmp;
	int num = 0;
	scanf("%d %d %d", &h1, &h2, &n);
	h = h1 + h2;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &tmp);
		if(h >= tmp)
			num++;
	}
	printf("%d", num);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1002

#include <stdio.h>

int main(int argc, char *args[])
{
	int l, w;
	scanf("%d %d", &l, &w);

	int arr[l][w];
	int i, j;

	for(i = 0; i < l; i++)
		for(j = 0; j < w; j++)
			scanf("%d", &arr[i][j]);

	int a, b;
	scanf("%d %d", &a, &b);

	int m = l - a + 1;
	int n = w - b + 1;
	int tmp[l][w];

	for(i = 0; i < l; i++)
		for(j = 0; j < w; j++)
			tmp[i][j] = 0;

	for(i = 0; i < a; i++)
		for(j = 0; j < w; j++)
			tmp[0][j] += arr[i][j];

	for(i = 1; i < m; i++)
		for(j = 0; j < w; j++)
			tmp[i][j] = tmp[i - 1][j] - arr[i - 1][j] + arr[i + a - 1][j];

	for(i = 0; i < m; i++)
		for(j = 0; j < w; j++)
		{
			arr[i][j] = tmp[i][j];
			tmp[i][j] = 0;
		}

	for(i = 0; i < b; i++)
		for(j = 0; j < m; j++)
			tmp[j][0] += arr[j][i];

	for(i = 1; i < n; i++)
		for(j = 0; j < m; j++)
			tmp[j][i] = tmp[j][i - 1] - arr[j][i - 1] + arr[j][i + b - 1];

	int max = 0;
	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			if(tmp[i][j] > max)
				max = tmp[i][j];

	printf("%d", max);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1003

#include <stdio.h>

int main(int argc, char *args[])
{
	int l;

	scanf("%d", &l);

	int arr[l][l];
	int i, j;

	for(i = 0; i < l; i++)
		for(j = 0; j < l; j++)
			scanf("%d", &arr[i][j]);

	int num = 0;
	int check;
	while(1)
	{
		check = 0;
		for(i = 0; i < l; i++)
			for(j = 0; j < l; j++)
				if(arr[i][j] == 0)
					check = 1;

		if(check == 0)
			break;

		for(i = 0; i < l; i++)
			for(j = 0; j < l; j++)
				if(arr[i][j] == 1)
				{
					if(i > 0 && arr[i - 1][j] == 0)
						arr[i - 1][j] = 3;
					if(j > 0 && arr[i][j - 1] == 0)
						arr[i][j - 1] = 3;
					if(i < l - 1 && arr[i + 1][j] == 0)
						arr[i + 1][j] = 3;
					if(j < l - 1 && arr[i][j + 1] == 0)
						arr[i][j + 1] = 3;
				}
		for(i = 0; i < l; i++)
			for(j = 0; j < l; j++)
				if(arr[i][j] == 3)
					arr[i][j] = 1;

		num++;
	}

	printf("%d", num);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1004

#include <stdio.h>

int main(int argc, char *args[])
{
	int m, t, u, f, d;
	scanf("%d %d %d %d %d", &m, &t, &u, &f, &d);

	int t1, t2;
	t1 = 2 * f;
	t2 = u + d;

	int l = 0;
	char c;
	while(t)
	{
		scanf("%c", &c);
		switch(c)
		{
		case 'u': case 'd':
			m -= t2;
			if(m > 0)
				l++;
			t--;
			break;
		case 'f':
			m -= t1;
			if(m > 0)
				l++;
			t--;
			break;
		}
	}
	printf("%d", l);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1005

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int arr[9][9];
	int tmp[9];
	int check;
	int i, j, k;
	while(n--)
	{
		for(i = 0; i < 9; i++)
			for(j = 0; j < 9; j++)
				scanf("%d", &arr[i][j]);

		for(j = 0; j < 9; j++)
		{
			for(i = 0; i < 9; i++)
				tmp[i] = 0;

			for(i = 0; i < 9; i++)
				tmp[arr[i][j] - 1] = 1;

			check = 1;
			for(i = 0; i < 9; i++)
				if(tmp[i] == 0)
					check = 0;
			if(check == 0)
				break;
		}
		if(check == 0)
		{
			printf("Wrong\n");
			continue;
		}

		for(j = 0; j < 9; j++)
		{
			for(i = 0; i < 9; i++)
				tmp[i] = 0;

			for(i = 0; i < 9; i++)
				tmp[arr[j][i] - 1] = 1;

			check = 1;
			for(i = 0; i < 9; i++)
				if(tmp[i] == 0)
					check = 0;
			if(check == 0)
				break;
		}
		if(check == 0)
		{
			printf("Wrong\n");
			continue;
		}

		for(k = 0; k < 9; k++)
		{
			for(i = 0; i < 9; i++)
				tmp[i] = 0;

			for(i = 0; i < 3; i++)
				for(j = 0; j < 3; j++)
					tmp[arr[(k / 3) * 3 + i][(k % 3) * 3 + j] - 1] = 1;

			check = 1;
			for(i = 0; i < 9; i++)
				if(tmp[i] == 0)
					check = 0;
			if(check == 0)
				break;
		}
		if(check == 0)
		{
			printf("Wrong\n");
			continue;
		}

		printf("Right\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1006

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int arr[n];
	int i;
	for(i = 0; i < n; i++)
		scanf("%d", &arr[i]);

	int max;
	int sum;
	max = sum = arr[0] + arr[1];
	for(i = 2; i < n; i++)
	{
		if(sum > arr[i - 1])
			sum += arr[i];
		else
			sum = arr[i - 1] + arr[i];

		if(max < sum)
			max = sum;
	}

	if(max > 0)
		printf("%d\n", max);
	else
		printf("Game Over\n");
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1007

#include <stdio.h>

int main(int argc, char *args[])
{
	int LEN = 211;
	char a[LEN], b[LEN];
	int i, j;
	scanf("%s%s", a, b);

	for(i = 0; i < LEN; i++)
		if(a[i] == 0)
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		a[j] = a[i];
	for(; j >= 0; j--)
		a[j] = '0';

	for(i = 0; i < LEN; i++)
		if(b[i] == 0)
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		b[j] = b[i];
	for(; j >= 0; j--)
		b[j] = '0';

	for(i = LEN - 2; i >= 0; i--)
		if(a[i] != '.')
		{
			a[i] = a[i] - '0' + b[i];
			if(a[i] > '9')
			{
				a[i] -= 10;
				if(a[i - 1] == '.')
					a[i - 2]++;
				else
					a[i - 1]++;
			}
		}

	for(i = 0; i < LEN; i++)
		if(a[i] != '0')
			break;

	printf("%s\n", a + i);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1008

#include <stdio.h>

int mdlen(int y, int m1, int d1, int m2, int d2)
{
	int mon[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int day = 0;
	int i;

	if((y % 400 == 0) || (y % 100 != 0) && (y % 4 == 0))
		mon[2] = 29;

	for(i = m1; i < m2; i++)
		day += mon[i];

	day -= d1 - 1;
	day += d2;
	return day;
}

int ymdlen(int y1, int m1, int d1, int y2, int m2, int d2)
{
	int day;
	int num = 0;
	int i;
	day = 365 * (y2 - y1);
	day += ((y2 + 399) / 400 - (y1 + 399) / 400) - ((y2 + 99) / 100 - (y1 + 99) / 100) + ((y2 + 3) / 4 - (y1 + 3) / 4);

	day -= mdlen(y1, 1, 1, m1, d1) - 1;
	day += mdlen(y2, 1, 1, m2, d2);
	return day;
}

int getweek(int y, int m, int d)
{
	int week;
	if(y >= 2000)
		week = (ymdlen(2000, 1, 1, y, m, d) + 4) % 7;
	else
		week = (13 - (ymdlen(y, m, d, 2000, 1, 1) % 7)) % 7;
	return week;
}

int checkday(int y, int m, int d, int m1, int d1, int m2, int d2)
{
	int week;
	if((m > m1 || m == m1 && d >= d1) && (m2 > m || m2 == m && d2 >= d))
	{
		week = getweek(y, m, d);
		if(week != 5 && week != 6)
			return 1;
	}
	return 0;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int y1, m1, d1, y2, m2, d2;
	int day;
	int week;
	int i, j;
	while(n--)
	{
		scanf("%04d-%02d-%02d %04d-%02d-%02d", &y1, &m1, &d1, &y2, &m2, &d2);
		day = ymdlen(y1, m1, d1, y2, m2, d2);
		week = getweek(y2, m2, d2);

		int offset = day % 7;
		day -= day / 7 * 2;
		for(i = week; i > week - offset; i--)
		{
			switch(i)
			{
			case 5: case 6: case -1: case -2:
				day--;
			}
		}

		for(i = y1 + 1; i < y2; i++)
		{
			week = getweek(i, 1, 1);
			if(week != 5 && week != 6)
				day--;
			for(j = 1; j < 4; j++)
			{
				week = getweek(i, 5, j);
				if(week != 5 && week != 6)
					day--;
			}
			day -= 5;
		}

		if(y1 != y2)
		{
			day -= checkday(y1, 1, 1, m1, d1, 12, 31);
			day -= checkday(y1, 5, 1, m1, d1, 12, 31);
			day -= checkday(y1, 5, 2, m1, d1, 12, 31);
			day -= checkday(y1, 5, 3, m1, d1, 12, 31);
			day -= checkday(y1, 10, 1, m1, d1, 12, 31);
			day -= checkday(y1, 10, 2, m1, d1, 12, 31);
			day -= checkday(y1, 10, 3, m1, d1, 12, 31);
			day -= checkday(y1, 10, 4, m1, d1, 12, 31);
			day -= checkday(y1, 10, 5, m1, d1, 12, 31);
			day -= checkday(y1, 10, 6, m1, d1, 12, 31);
			day -= checkday(y1, 10, 7, m1, d1, 12, 31);

			day -= checkday(y2, 1, 1, 1, 1, m2, d2);
			day -= checkday(y2, 5, 1, 1, 1, m2, d2);
			day -= checkday(y2, 5, 2, 1, 1, m2, d2);
			day -= checkday(y2, 5, 3, 1, 1, m2, d2);
			day -= checkday(y2, 10, 1, 1, 1, m2, d2);
			day -= checkday(y2, 10, 2, 1, 1, m2, d2);
			day -= checkday(y2, 10, 3, 1, 1, m2, d2);
			day -= checkday(y2, 10, 4, 1, 1, m2, d2);
			day -= checkday(y2, 10, 5, 1, 1, m2, d2);
			day -= checkday(y2, 10, 6, 1, 1, m2, d2);
			day -= checkday(y2, 10, 7, 1, 1, m2, d2);
		}
		else
		{
			day -= checkday(y2, 1, 1, m1, d1, m2, d2);
			day -= checkday(y2, 5, 1, m1, d1, m2, d2);
			day -= checkday(y2, 5, 2, m1, d1, m2, d2);
			day -= checkday(y2, 5, 3, m1, d1, m2, d2);
			day -= checkday(y2, 10, 1, m1, d1, m2, d2);
			day -= checkday(y2, 10, 2, m1, d1, m2, d2);
			day -= checkday(y2, 10, 3, m1, d1, m2, d2);
			day -= checkday(y2, 10, 4, m1, d1, m2, d2);
			day -= checkday(y2, 10, 5, m1, d1, m2, d2);
			day -= checkday(y2, 10, 6, m1, d1, m2, d2);
			day -= checkday(y2, 10, 7, m1, d1, m2, d2);
		}

		printf("%d\n", day);
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1009

#include <stdio.h>

typedef struct st1
{
	int t;
	int a;
	int d;
	int v;
} st1;

typedef struct st2
{
	int t;
	int v;
} st2;

int main(int argc, char *args[])
{
	int m, n;
	scanf("%d", &m);

	st1 arr[m];
	int i;
	for(i = 0; i < m; i++)
		scanf("%d %d %d", &arr[i].t, &arr[i].a, &arr[i].d);

	scanf("%d", &n);
	st2 tmp[n];
	for(i = 0; i < n; i++)
		scanf("%d %d", &tmp[i].t, &tmp[i].v);

	int flag = 0;
	for(i = 0; i < n - 1; i++)
	{
		while(tmp[i].t <= arr[flag].t && tmp[i + 1].t > arr[flag].t)
		{
			arr[flag].v = tmp[i].v;
			flag++;
		}
	}
	for(i = flag; i < m; i++)
		arr[i].v = tmp[n - 1].v;

	double f = 0;
	double t = 0;
	for(i = 0; i < m; i++)
	{
		t = arr[i].a * arr[i].v;
		f -= t * 0.2 < 5.0 ? 5.0 : t * 0.2;

		f -= arr[i].a / 10.0;
		f -= 1;

		if(arr[i].d == 1)
			f -= t * 100;
		else
		{
			f -= t * 0.1;
			f += t * 100;
		}
	}

	printf("%.02f\n", f);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1010

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	int p = 0, q = 0;
	int i;
	for(i = 1; i < 13; i++)
	{
		scanf("%d", &n);
		if(p + 300 < n)
		{
			printf("-%d\n", i);
			break;
		}
		q += (p + 300 - n) / 100 * 100;
		p = (p + 300 - n) % 100;
	}
	if(i == 13)
		printf("%d\n", p + q / 5 * 6);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1011

#include <stdio.h>

int main(int argc, char *args[])
{
	int a, b, c, d;
	scanf("%d %d", &a, &b);
	scanf("%d %d", &c, &d);

	double da = a, db = b, dc = c, dd = d;
	int d1, d2;
	printf("%.2f %.2f\n", da + dc, db + dd);
	printf("%.2f %.2f\n", da - dc, db - dd);
	printf("%.2f %.2f\n", da * dc - db * dd, da * dd + db * dc);
	printf("%.2f %.2f\n", (da * dc + db * dd) / (dc * dc + dd * dd), (db * dc - da * dd) / (dc * dc + dd * dd));

	printf("%.2f %.2f\n", da += dc, db += dd);
	printf("%.2f %.2f\n", da -= dc, db -= dd);
	d1 = da * dc - db * dd;
	d2 = da * dd + db * dc;
	da = d1;
	db = d2;
	printf("%.2f %.2f\n", da, db);
	d1 = (da * dc + db * dd) / (dc * dc + dd * dd);
	d2 = (db * dc - da * dd) / (dc * dc + dd * dd);
	da = d1;
	db = d2;
	printf("%.2f %.2f\n", da, db);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1012

#include <stdio.h>
#include <stdlib.h>

typedef struct next
{
	long long max;
	long long len;
} next;

int main(int argc, char *args[])
{
	int s, t;
	scanf("%d %d", &s, &t);

	next arr[t - s + 1];
	int n;
	int i, j;
	long long max, len;
	for(i = t; i >= s; i--)
	{
		arr[i - s].max = 1;
		arr[i - s].len = 1;
		max = 1;
		for(j = 101; j <= 200; j++)
		{
			if(i * j % 100 == 0)
			{
				n = i * j / 100;
				if(n > t)
					break;
				if(max < arr[n - s].max + 1)
				{
					max = arr[n - s].max + 1;
					arr[i - s].len = arr[n - s].len;
				}
				else if(max == arr[n - s].max + 1)
					arr[i - s].len += arr[n - s].len;
			}
		}
		arr[i - s].max = max;
	}

	max = 0;
	len = 0;
	for(i = s; i <= t; i++)
	{
		if(max < arr[i - s].max)
		{
			max = arr[i - s].max;
			len = arr[i - s].len;
		}
		else if(max == arr[i - s].max)
			len += arr[i - s].len;
	}

	printf("%lld\n%lld\n", max, len);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1013

#include <stdio.h>

int main(int argc, char *args[])
{
	int v, n;
	scanf("%d %d", &v, &n);

	int c[n + 1], w[n + 1], f[v + 1];
	int i, j;
	for(i = 1; i <= n; i++)
		scanf("%d %d", &c[i], &w[i]);

	for(i = 0; i <= v; i++)
		f[i] = 0;

	for(i = 1; i <= n; i++)
		for(j = c[i]; j <= v; j++) //将第i个数据尝试放入所有位置，如果放入后该位置及之前的总价值更大，则放入并更新最大值
			if(f[j] < f[j - c[i]] + w[i])
				f[j] = f[j - c[i]] + w[i];

	printf("%d\n", f[v]);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1014

#include <stdio.h>

#define LEN 100000

int main(int argc, char *args[])
{
	char a[LEN + 1] = {0};
	char b[LEN + 1] = {0};
	scanf("%s", a);
	scanf("%s", b);

	int i, j;
	for(i = LEN - 1; i >= 0; i--)
		if(a[i])
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		a[j] = a[i];
	for(; j >= 0; j--)
		a[j] = 0;

	for(i = LEN - 1; i >= 0; i--)
		if(b[i])
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		b[j] = b[i];
	for(; j >= 0; j--)
		b[j] = 0;

	char c = 0;
	for(i = LEN - 1; i >= 0; i--)
	{
		if(a[i] == 0 && b[i] == 0)
		{
			if(c == 1)
				a[i] = '1';
			break;
		}
		if(a[i] == 0)
			a[i] = b[i] + c;
		else if(b[i] == 0)
			a[i] = a[i] + c;
		else
			a[i] += b[i] - '0' + c;
		c = 0;
		if(a[i] > '9')
		{
			a[i] -= 10;
			c = 1;
		}
	}

	for(i = LEN - 1; i >= 0 && a[i]; i--) ;
	i++;
	printf("%s", a + i);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1015

#include <stdio.h>

#define LEN 1000

int main(int argc, char *args[])
{
	char a[LEN + 1] = {0};
	char b[LEN + 1] = {0};
	scanf("%s", a);
	scanf("%s", b);

	char tmp[LEN * 2 + 1];
	char sum[LEN * 2 + 1] = {0};

		int i, j;
	for(i = LEN - 1; i >= 0; i--)
		if(a[i])
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		a[j] = a[i];
	for(; j >= 0; j--)
		a[j] = 0;

	for(i = LEN - 1; i >= 0; i--)
		if(b[i])
			break;
	for(j = LEN - 1; i >= 0; i--, j--)
		b[j] = b[i];
	for(; j >= 0; j--)
		b[j] = 0;

	int k;
	int c = 0;
	for(i = LEN - 1; a[i]; i--)
	{
		for(j = 0; j <= LEN * 2; j++)
			tmp[j] = 0;

		c = 0;
		for(j = LEN - 1; b[j]; j--)
		{
			k = (a[i] - '0') * (b[j] - '0') + c;
			c = k / 10;
			tmp[j + LEN] = (k % 10) + '0';
		}
		if(c != 0)
			tmp[j + LEN] = '0' + c;

		c = 0;
		for(j = LEN * 2 - 1; j >= 0; j--)
		{
			k = j + i - LEN + 1;
			if(tmp[j] == 0 && sum[k] == 0)
			{
				if(c != 0)
					sum[k] = c + '0';
				break;
			}
			if(tmp[j] == 0)
				sum[k] += c;
			else if(sum[k] == 0)
				sum[k] = tmp[j] + c;
			else
				sum[k] += tmp[j] - '0' + c;
			c = 0;
			if(sum[k] > '9')
			{
				sum[k] -= 10;
				c = 1;
			}
		}
	}

	for(i = LEN * 2 - 1; sum[i]; i--) ;
	printf("%s\n", sum + i + 1);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1016

#include <stdio.h>
#include <string.h>

#define LEN 1000

int sub(char *a, char *b, int l)
{
	int i;
	int c;
	if(strncmp(a, b, l) < 0)
		return 0;

	c = 0;
	for(i = l - 1; i >= 0; i--)
	{
		a[i] = a[i] - b[i] + '0' - c;
		c = 0;
		if(a[i] < '0')
		{
			a[i] += 10;
			c = 1;
		}
	}
	return 1;
}

int main(int argc, char *args[])
{
	char a[LEN + 1] = {0};
	char b[LEN + 1] = {0};
	scanf("%s", a);
	scanf("%s", b);

	int lb = strlen(b);
	char num[LEN + 1] = {0};
	int id = 0;
	int i, j;

	for(i = 0; i <= LEN; i++)
		num[i] = '0';

	j = 0;
	for(i = 0; a[i]; i++)
	{
		if(strnlen(a + i, lb) < strnlen(b, lb))
			break;
		while(sub(a + i, b, lb))
			num[id]++;
		if(j || num[id] != '0')
		{
			j = 1;
			id++;
		}
		if(a[i + 1] != 0)
			a[i + 1] += (a[i] - '0') * 10;
	}
	num[id] = 0;
	printf("%s\n", num);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1017

#include <stdio.h>

int right(char *a, int len)
{
	int i, j;
	for(i = len - 1; i >= 0; i--)
		if(a[i])
			break;
	for(j = len - 1; i >= 0; i--, j--)
		a[j] = a[i];
	for(; j >= 0; j--)
		a[j] = 0;
}

int add(char *a, char *b, int len, char *sum)
{
	int i, j;
	char c = 0;
	for(i = len - 1; i >= 0; i--)
	{
		if(a[i] == 0 && b[i] == 0)
		{
			if(c == 1)
				sum[i] = '1';
			break;
		}
		if(a[i] == 0)
			sum[i] = b[i] + c;
		else if(b[i] == 0)
			sum[i] = a[i] + c;
		else
			sum[i] = a[i] + b[i] - '0' + c;
		c = 0;
		if(sum[i] > '9')
		{
			sum[i] -= 10;
			c = 1;
		}
	}
}

int mul(char *a, char *b, int len, char *sum)
{
	char tmp[len * 2 + 1];

	int i, j, k;
	int c = 0;
	for(i = len - 1; a[i]; i--)
	{
		for(j = 0; j <= len * 2; j++)
			tmp[j] = 0;

		c = 0;
		for(j = len - 1; b[j]; j--)
		{
			k = (a[i] - '0') * (b[j] - '0') + c;
			c = k / 10;
			tmp[j + len] = (k % 10) + '0';
		}
		if(c != 0)
			tmp[j + len] = '0' + c;

		c = 0;
		for(j = len * 2 - 1; j >= 0; j--)
		{
			k = j + i - len + 1;
			if(tmp[j] == 0 && sum[k] == 0)
			{
				if(c != 0)
					sum[k] = c + '0';
				break;
			}
			if(tmp[j] == 0)
				sum[k] += c;
			else if(sum[k] == 0)
				sum[k] = tmp[j] + c;
			else
				sum[k] += tmp[j] - '0' + c;
			c = 0;
			if(sum[k] > '9')
			{
				sum[k] -= 10;
				c = 1;
			}
		}
	}
}

#define LEN 3000

int print(char *a)
{
	int i;
	for(i = LEN - 1; a[i]; i--) ;
	printf("%s\n", a + i + 1);
}

int main(int argc, char *args[])
{
	char a[LEN + 1] = {0}, b[LEN + 1] = {0}, c[LEN + 1] = {0};
	int n;
	scanf("%s %s %s %d", a, b, c, &n);

	right(a, LEN);
	right(b, LEN);
	right(c, LEN);

	char n0[LEN + 1] = {0};
	char n1[LEN + 1] = {0};
	char n2[LEN + 1] = {0};
	char n3[LEN + 1] = {0};
	n0[LEN - 1] = '1';

	int i, j;
	for(i = 0; i < n; i++)
	{
		char sum1[LEN * 2 + 1] = {0};
		char sum2[LEN * 2 + 1] = {0};
		char sum3[LEN * 2 + 1] = {0};
		char sum4[LEN + 1] = {0};
		char sum5[LEN + 1] = {0};
		char sum6[LEN + 1] = {0};
		char sum7[LEN + 1] = {0};

		mul(a, n0, LEN, sum1);
		mul(b, n1, LEN, sum2);
		add(n2, n3, LEN, sum4);
		mul(c, sum4, LEN, sum3);
		add(sum1 + LEN, sum2 + LEN, LEN, sum5);
		add(sum5, sum3 + LEN, LEN, sum6);
		add(n2, n3, LEN, sum7);

		for(j = 0; j < LEN; j++)
		{
			n3[j] = sum7[j];
			n2[j] = n1[j];
			n1[j] = n0[j];
			n0[j] = sum6[j];
		}
	}
	char sum8[LEN + 1] = {0};
	char sum9[LEN + 1] = {0};
	char sum10[LEN + 1] = {0};
	add(n0, n1, LEN, sum8);
	add(sum8, n2, LEN, sum9);
	add(sum9, n3, LEN, sum10);

	print(sum10);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1018

#include <stdio.h>

int main(int argc, char *args[])
{
	int a, b;

	while(scanf("%d %d", &a, &b) != EOF)
		printf("%d\n", a + b);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1019

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	char str[101];
	int num;
	int i;
	while(n--)
	{
		scanf("%s", str);
		num = 0;
		for(i = 0; str[i]; i++)
		{
			if(str[i] == '(')
				num++;
			else if(str[i] == ')')
				num--;
			if(num < 0)
				break;
		}
		if(str[i] || num != 0)
			printf("NO\n");
		else
			printf("YES\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1020

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int id;
	int i;
	printf("%d=", n);
	for(i = 2; i <= 2147483647; i++)
	{
		id = 0;
		while(n % i == 0)
		{
			n /= i;
			id++;
		}
		if(id)
			printf("%d(%d)", i, id);
		if(n <= 1)
			break;
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1021

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int arr[n][n];
	int id = 1;
	int d = 1;
	int i = 0, j = 0;

	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			arr[i][j] = 0;

	i = 0;
	j = 0;
	arr[i][j] = id++;
	while(id <= n * n)
	{
		switch(d)
		{
		case 1:
			if(i + 1 >= n || arr[i + 1][j] != 0)
				d = 2;
			else
				arr[++i][j] = id++;
			break;
		case 2:
			if(j + 1 >= n || arr[i][j + 1] != 0)
				d = 3;
			else
				arr[i][++j] = id++;
			break;
		case 3:
			if(i - 1 < 0 || arr[i - 1][j] != 0)
				d = 4;
			else
				arr[--i][j] = id++;
			break;
		case 4:
			if(j - 1 < 0 || arr[i][j - 1] != 0)
				d = 1;
			else
				arr[i][--j] = id++;
			break;
		}
	}

	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
			printf("%6d", arr[j][i]);
		printf("\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1022

#include <stdio.h>

int main(int argc, char *args[])
{
	long long n;
	scanf("%lld", &n);

	short arr[2010][2010];
	int a = 1, b = 1, c;
	int i, j;

	for(i = 0; i < 2010; i++)
		for(j = 0; j < 2010; j++)
			arr[i][j] = 0;

	for(i = 3; i <= n; i++)
	{
		if(arr[a][b])
		{
			n %= i - arr[a][b];
			break;
		}
		arr[a][b] = i;
		c = (a + b) % 2010;
		a = b;
		b = c;
	}

	a = 1;
	b = 1;
	for(i = 3; i <= n; i++)
	{
		c = (a + b) % 2010;
		a = b;
		b = c;
	}

	if(n < 3)
		c = 1;

	printf("%d\n", c);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1023

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int a, b;
	int tmp;
	int i, j, k;
	for(k = 0; k < n; k++)
	{
		scanf("%d %d", &a, &b);
		int arr[a][a];
		for(i = 0; i < a; i++)
			for(j = 0; j < a; j++)
				scanf("%d", &arr[i][j]);

		switch(b)
		{
		case 0:
			for(i = 0; i < a; i++)
				for(j = 0; j < a / 2; j++)
				{
					tmp = arr[i][j];
					arr[i][j] = arr[i][a - 1 - j];
					arr[i][a - 1 - j] = tmp;
				}
			break;
		case 1:
			for(i = 0; i < a / 2; i++)
				for(j = 0; j < a; j++)
				{
					tmp = arr[i][j];
					arr[i][j] = arr[a - 1 - i][j];
					arr[a - 1 - i][j] = tmp;
				}
			break;
		case 2:
			for(i = 0; i < a; i++)
				for(j = 0; j < i; j++)
				{
					tmp = arr[i][j];
					arr[i][j] = arr[j][i];
					arr[j][i] = tmp;
				}
			break;
		}

		for(i = 0; i < a; i++)
		{
			for(j = 0; j < a; j++)
				printf("%d ", arr[i][j]);
			printf("\n");
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1024

#include <stdio.h>
#include <stdlib.h>

int sort(int *arr, int start, int end)
{
	if(start >= end)
		return 0;

	int tmp = arr[start];
	int i = start, j = end;

	while(i < j)
	{
		while(i < j && arr[j] >= tmp)
			j--;
		if(i < j)
			arr[i] = arr[j];
		while(i < j && arr[i] <= tmp)
			i++;
		if(i < j)
			arr[j] = arr[i];
	}
	arr[j] = tmp;

	sort(arr, start, j - 1);
	sort(arr, j + 1, end);
}

int cmp(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int arr[n];
	int i;
	for(i = 0; i < n; i++)
		scanf("%d", &arr[i]);

	//sort(arr, 0, n - 1);
	qsort(arr, n, sizeof(int), cmp);

	for(i = 0; i < n; i++)
		printf("%d ", arr[i]);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1025

#include <stdio.h>
#include <stdlib.h>

typedef struct st
{
	int a;
	int b;
} st;

int cmp(const void *a, const void *b)
{
	return (((st *)a)->a - ((st *)b)->a);
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	st arr[n];
	int i;
	for(i = 0; i < n; i++)
		scanf("%d %d", &arr[i].a, &arr[i].b);

	qsort(arr, n, sizeof(st), cmp);

	int max = arr[0].b;
	long long len = arr[0].b - arr[0].a;

	for(i = 1; i < n; i++)
	{
		if(max <= arr[i].a)
			len += arr[i].b - arr[i].a;
		else if(max <= arr[i].b)
			len += arr[i].b - max;
		else
			continue;
		max = arr[i].b;
	}

	printf("%lld\n", len);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1026

#include <stdio.h>
#include <string.h>

int sub(char *a, char *b, int lb)
{
	int n = 0;
	int i;
	int c;
	while(strncmp(a, b, lb) >= 0)
	{
		c = 0;
		for(i = lb - 1; i >= 0; i--)
		{
			a[i] = a[i] - b[i] + '0' - c;
			c = 0;
			if(a[i] < '0')
			{
				a[i] += 10;
				c = 1;
			}
		}
		n++;
	}
	return n;
}

int main(int argc, char *args[])
{
	char a[10001] = {0};
	char b[10001] = {0};
	char c[10001] = {0};

	scanf("%s", a);
	scanf("%s", b);

	int la = strlen(a);
	int lb = strlen(b);

	int n;
	int d = 0;
	int i, j = 0;
	for(i = 0; i <= la - lb; i++)
	{
		n = sub(a + i, b, lb);
		if(j || n)
		{
			c[d++] = n + '0';
			j = 1;
		}
		if(a[i + 1])
			a[i + 1] += (a[i] - '0') * 10;
	}

	printf("%s\n", c);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1027

#include <stdio.h>

typedef struct st
{
	int ti;
	int ui;
	int gi;
	int r;
} st;

int main(int argc, char *args[])
{
	int n, max, s;
	scanf("%d %d %d", &n, &max, &s);

	st info[n];
	int i, j;

	for(i = 0; i < n; i++)
	{
		scanf("%d %d %d", &info[i].ti, &info[i].ui, &info[i].gi);
		info[i].r = 0;
	}

	int t = 0; //上一架降落的飞机
	int min, num, refly;
	i = 1;
	while(1)
	{
		if(i < n)
		{
			min = i;
			num = 1;
		}
		else
		{
			min = -1;
			num = 0;
		}
		//检查复飞飞机和当前飞机中的最小到达时间和与该时间到达的数量
		refly = 0;
		for(j = 1; j < i; j++)
			if(info[j].r == 1)
			{
				if(info[min].ti > info[j].ti)
				{
					min = j;
					num = 1;
				}
				else if(info[min].ti == info[j].ti)
					num++;
				refly = 1;
			}
		if(num == 0 && refly == 0) //没有飞机
		{
			for(i = 0; i < n; i++)
				printf("%d\n", info[i].ti + info[i].ui);
			break;
		}
		if(info[min].ti >= max) //有飞机未降落
		{
			printf("GO DATING\n");
			break;
		}
		if(num >= 2) //同时到达
		{
			printf("CHANGE BOYFRIEND\n");
			break;
		}
		if(i == min) //选择的是当前飞机
			i++;

		if(info[t].ti + info[t].ui + s <= info[min].ti + info[min].ui)
		{
			t = min;
			info[t].r = 0;
		}
		else
		{
			info[min].ti += info[min].gi;
			info[min].r = 1;
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1028

/*
尚未解决
*/
#include <stdio.h>

int check(int *info, int n)
{
	int buf[6] = {0};
	int isif = 0;
	int i;
	for(i = 0; i < n; i++)
	{
		buf[info[i]]++;
		if(isif == 1)
		{
			if(info[i] != 4)
				return 0;
			isif = 0;
			continue;
		}

		if(info[i] == 3) //进入if
		{
			isif = 1;
			continue;
		}

		//end大于begin，then大于if，else大于then
		if(buf[1] < buf[2] || buf[3] < buf[4] || buf[3] < buf[5] || buf[4] < buf[5])
			return 0;
	}
	if(buf[1] != buf[2] || buf[3] != buf[4])
		return 0;
	return 1;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int info[n];
	int i;
	for(i = 0; i < n; i++)
	{
		char s[6] = {0};
		scanf("%s", s);
		switch(s[0])
		{
		case 'b': //begin
			info[i] = 1;
			break;
		case 'e':
			if(s[1] == 'n') //end
				info[i] = 2;
			else //else
				info[i] = 5;
			break;
		case 'i': //if
			info[i] = 3;
			break;
		case 't': //then
			info[i] = 4;
			break;
		}
	}

	if(check(info, n))
		printf("YES\n");
	else
		printf("NO\n");
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1029

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);
	getchar();

	int info[n][n * 101];
	int index[n];
	int num;
	int i, j, k;
	char c = 0;
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n * 101; j++)
			info[i][j] = 0;
		index[i] = 0;
	}
	for(i = 0; i < n; i++)
	{
		for(j = 0; c != '\n'; j++)
		{
			num = 0;
			while(1)
			{
				c = getchar();
				if(c != ' ' && c != '\n')
				{
					num = num * 10 + c - '0';
					continue;
				}
				break;
			}
			info[i][j] = num;
		}
		c = 0;
		index[i] = j;
	}

	int nx[n - 1], ny[n - 1];
	int x, y;
	for(i = 0; i < n - 1; i++)
		scanf("%d %d", &nx[i], &ny[i]);

	int check;
	int bk = 1;
	while(bk)
	{
		bk = 0;
		for(i = 0; i < n - 1; i++)
		{
			if(nx[i] < 0)
				continue;
			bk = 1;

			check = 1;
			for(j = 0; j < n - 1; j++)
				if(nx[i] == ny[j])
					check = 0;
			if(check == 0)
				continue;

			x = nx[i] - 1;
			y = ny[i] - 1;
			for(j = 0; j < index[x]; j++)
				info[y][index[y] + j] = info[x][j];
			index[y] += index[x];
			index[x] = 0;

			nx[i] = -1;
			ny[i] = -1;
		}
	}

	for(i = 0; i < n; i++)
		if(index[i] != 0)
		{
			for(j = 0; j < index[i]; j++)
				printf("%d ", info[i][j]);
			break;
		}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1031

#include <stdio.h>

int arr[101][101] = {0};
int tmp[101][101] = {0};
int n;

int _findway(int x, int y, int min, int max)
{
	if(tmp[x][y] == 1)
		return 0;
	if(x == n - 1 && y == n - 1)
		return 1;

	tmp[x][y] = 1;
	if(x > 0 && arr[x - 1][y] >= min && arr[x - 1][y] <= max)
		if(_findway(x - 1, y, min, max) == 1)
			return 1;
	if(x < n - 1 && arr[x + 1][y] >= min && arr[x + 1][y] <= max)
		if(_findway(x + 1, y, min, max) == 1)
			return 1;
	if(y > 0 && arr[x][y - 1] >= min && arr[x][y - 1] <= max)
		if(_findway(x, y - 1, min, max) == 1)
			return 1;
	if(y < n - 1 && arr[x][y + 1] >= min && arr[x][y + 1] <= max)
		if(_findway(x, y + 1, min, max) == 1)
			return 1;
	return 0;
}

int findway(int x, int y, int min, int max)
{
	int i, j;
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			tmp[i][j] = 0;
	return _findway(x, y, min, max);
}

int main(int argc, char *args[])
{
	scanf("%d", &n);

	int min = 0, max = 0;
	int i, j, k;
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
		{
			scanf("%d", &arr[i][j]);
			if(min > arr[i][j])
				min = arr[i][j];
			if(max < arr[i][j])
				max = arr[i][j];
		}

	int high, low, mid;
	int flag;
	high = max - min;
	low = 0;

	while(high - low > 1)
	{
		mid = (high + low) / 2;
		flag = 0;
		for(i = min; i <= max - mid; i++)
		{
			if(i + mid < arr[0][0] || i + mid < arr[n - 1][n - 1] || i > arr[0][0] || i > arr[n - 1][n - 1])
				continue;
			if(findway(0, 0, i, i + mid) == 1)
			{
				flag = 1;
				break;
			}
		}

		if(flag)
			high = mid;
		else
			low = mid;
	}
	printf("%d\n", high);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1032

#include <stdio.h>

int ackeman(int m, int n)
{
	if(m == 0)
		return n + 1;
	if(n == 0)
		return ackeman(m - 1, 1);
	return ackeman(m - 1, ackeman(m, n - 1));
}

int main(int argc, char *args[])
{
	int m, n;
	scanf("%d %d", &m, &n);
	printf("%d\n", ackeman(m, n));
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1033

#include <stdio.h>

typedef struct node
{
	struct node *next;
	struct node *prev;
	int type;
	char c;
	long long d;
} node;

/*
用前缀表达式^不好处理
查找最内层的括号，计算内部的表达式，得出一个值后替换整个括号
*/
int main(int argc, char *args[])
{
	node stack[101] = {0};
	int index = 0;
	char c;
	int lc = 1; //
	int sign;
	int num = 0;
	int isnum = 0; //是否需要存储数字
	int k = 0; //括号数量

	while((c = getchar()) != '\n')
	{
		sign = 1;
		switch(c)
		{
		case '-':
			if(lc == 0) //负号
				goto fab;
			sign = -1;
		case '0' ... '9':
			lc = 0;
			isnum = 1;
			num = sign * (num * 10 + c - '0');
			break;
		case ' ':
			break;
		case '+': case '*': case '/': case '^':
			fab:
			lc = 1;
		case '(': case ')':
			if(c == '(')
				k++;
			else if(c == ')')
				k--;
			if(k < 0)
			{
				printf("Error\n");
				return 0;
			}
			if(isnum)
			{
				stack[index].type = 1;
				stack[index].d = num;
				index++;
				num = 0;
				isnum = 0;
			}

			stack[index].type = 2;
			stack[index].c = c;
			index++;
			break;
		}
	}
	if(k != 0)
	{
		printf("Error\n");
		return 0;
	}
	if(isnum)
	{
		stack[index].type = 1;
		stack[index].d = num;
		index++;
	}

	int start, end;
	int i, j;

	for(i = 0; i < index - 1; i++)
		stack[i].next = stack + i + 1;
	stack[i].next = NULL;

	stack[0].prev = NULL;
	for(i = 1; i < index; i++)
		stack[i].prev = stack + i - 1;

	node *stnd = stack;
	node *ednd = stack + index - 1;
	node *nd;
	node *startnd, *endnd;
	while(1)
	{
		startnd = stnd;
		endnd = ednd;
		for(nd = stnd; nd != ednd->next; nd = nd->next)
		{
			if(nd->type == 2)
			{
				if(nd->c == '(')
					startnd = nd;
				else if(nd->c == ')')
				{
					endnd = nd;
					//5 + ((1 + 3) * 6) ^ 1 --> 5 + (1 + 3 * 6) ^ 1
					if(startnd == stnd)
						stnd = startnd->next;
					if(startnd->prev)
						startnd->prev->next = startnd->next;
					if(startnd->next)
						startnd->next->prev = startnd->prev;
					if(endnd == ednd)
						ednd = endnd->prev;
					if(endnd->prev)
						endnd->prev->next = endnd->next;
					if(endnd->next)
						endnd->next->prev = endnd->prev;
					startnd = startnd->next;
					endnd = endnd->prev;
					index -= 2;
					break;
				}
			}
		}

		long long tmp;
		for(nd = endnd; nd != startnd->prev; nd = nd->prev)
		{
			if(nd->type == 2 && nd->c == '^')
			{
				tmp = 1;
				for(j = 0; j < nd->next->d; j++)
					tmp *= nd->prev->d;
				nd->prev->d = tmp;
				nd->prev->next = nd->next->next;
				if(nd->next->next)
					nd->next->next->prev = nd->prev;
				if(nd->next == ednd)
					ednd = nd->prev;
				index -= 2;
			}
		}
		for(nd = startnd; nd != endnd->next; nd = nd->next)
		{
			if(nd->type == 2)
			{
				if(nd->c == '*')
					nd->prev->d *= nd->next->d;
				else if(nd->c == '/')
				{
					if(nd->next->d == 0)
					{
						printf("Error\n");
						return 0;
					}
					nd->prev->d /= nd->next->d;
				}
				else
					continue;
				nd->prev->next = nd->next->next;
				if(nd->next->next)
					nd->next->next->prev = nd->prev;
				if(nd->next == ednd)
					ednd = nd->prev;
				nd = nd->prev;
				index -= 2;
			}
		}
		for(nd = startnd; nd != endnd->next; nd = nd->next)
		{
			if(nd->type == 2)
			{
				if(nd->c == '+')
					nd->prev->d += nd->next->d;
				else if(nd->c == '-')
					nd->prev->d -= nd->next->d;
				else
					continue;
				nd->prev->next = nd->next->next;
				if(nd->next->next)
					nd->next->next->prev = nd->prev;
				if(nd->next == ednd)
					ednd = nd->prev;
				nd = nd->prev;
				index -= 2;
			}
		}
		if(index <= 1)
			break;
	}
	printf("%lld\n", stnd->d);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1034

#include <stdio.h>

int main(int argc, char *args[])
{
	int n, k;
	scanf("%d %d", &n, &k);

	long long rp[n];
	int i, j;
	for(i = 0; i < n; i++)
		scanf("%lld", &rp[i]);

	long long min, max;

	max = 0;
	for(i = 0; i < k; i++)
		if(max < rp[i])
			max = rp[i];

	min = max;
	for(i = 1; i < n; i++)
	{
		if(max < rp[(i + k - 1) % n]) //下一个元素大于最大值则用将最大值设置为下一个元素值
			max = rp[(i + k - 1) % n];
		else if(max == rp[i - 1]) //上一个元素值为最大值，重新计算最大值
		{
			max = 0;
			for(j = i; j < i + k; j++)
				if(max < rp[j % n])
					max = rp[j % n];
		}
		if(min > max)
			min = max;
	}
	printf("%lld\n", min);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1035

#include <stdio.h>
#include <stdlib.h>

typedef struct stock
{
	int id;
	int price;
} stock;

int cmp(const void *a, const void *b)
{
	stock *x = (stock *)a;
	stock *y = (stock *)b;
	if(x->price > y->price || x->price == y->price && x->id < y->id)
		return -1;
	return 1;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	stock st[n];
	int i, j;
	for(i = 0; i < n; i++)
	{
		st[i].id = i + 1;
		scanf("%d", &st[i].price);
	}
	qsort(st, n, sizeof(stock), cmp);

	int k;
	scanf("%d", &k);

	char c;
	for(i = 0; i < k; i++)
	{
		while((c = getchar()) == '\n');
		if(c == 'M')
		{
			int x, y;
			int posid, posprice = -1;
			scanf("%d %d", &x, &y);
			for(j = 0; j < n; j++)
			{
				if(st[j].id == x)
					posid = j;
				if(st[j].price > y || st[j].price == y && st[j].id < x) //posid插到posprice后的位置
					posprice = j;
			}
			int t;
			if(posid < posprice)
				for(t = posid; t < posprice; t++)
					st[t] = st[t + 1];
			else if(posid > posprice)
				for(t = posid; t > posprice + 1; t--)
					st[t] = st[t - 1];
			else
				t = posid;
			st[t].id = x;
			st[t].price = y;
			//qsort(st, n, sizeof(stock), cmp); //如果性能达不到要求，则开始只排列一次，后续用插入法
		}
		else if(c == 'Q')
		{
			int r;
			scanf("%d", &r);

			int id;
			for(j = 0; j < r; j++)
			{
				scanf("%d", &id);
				printf("%d ", st[id - 1].id);
			}
			printf("\n");
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1036

#include <stdio.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	long long a[n];
	int i, j, k;
	for(i = 0; i < n; i++)
		scanf("%lld", &a[i]);

	long long w1 = 0, t1 = 0;
	long long sum;
	for(i = 0; i < 3; i++)
	{
		sum = 0;
		for(j = i, k = n / 3 + ((i < (n % 3)) ? 1 : 0) - 1; j < n; j += 3, k--)
		{
			w1 += a[j] * k;
			sum += a[j];
		}
		if(t1 < sum)
			t1 = sum;
	}
	printf("%lld %lld\n", w1, t1);

	long long w2 = 0, t2 = 0;
	long long q[3] = {0};
	long long min, max;
	for(i = 0; i < n; i++)
	{
		if(q[0] == 0)
			q[0] = a[i];
		else if(q[1] == 0)
			q[1] = a[i];
		else if(q[2] == 0)
			q[2] = a[i];
		else //等待min分钟后，一个队列空出
		{
			min = q[0] < q[1] ? q[0] : q[1];
			min = min < q[2] ? min : q[2];
			q[0] -= min;
			q[1] -= min;
			q[2] -= min;
			w2 += min * (n - i);
			t2 += min;
			i--;
		}
	}
	max = q[0] > q[1] ? q[0] : q[1];
	max = max > q[2] ? max : q[2];
	t2 += max;
	printf("%lld %lld\n", w2, t2);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1037

#include <stdio.h>
#include <stdlib.h>

int cmp(const void *a, const void *b)
{
	return *(int *)b - *(int *)a;
}

int main(int argc, char *args[])
{
	int m, n;
	scanf("%d %d", &m, &n);

	int a[m], b[n];
	int i, j;
	for(i = 0; i < m; i++)
		scanf("%d", &a[i]);
	for(i = 0; i < n; i++)
		scanf("%d", &b[i]);

	qsort(a, m, sizeof(int), cmp);
	qsort(b, n, sizeof(int), cmp);

	int num = m;
	for(i = 0, j = 0; i < m && j < n; j++)
	{
		if(a[i] > b[j])
		{
			num++;
			i++;
		}
	}
	printf("%d\n", num);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1038

/*
f(n + 1) = (f(n) + k[n]) % (n + 1)
*/
#include <stdio.h>

int main(int argc, char *args[])
{
	int m;
	scanf("%d", &m);

	int k[m - 1];
	int i;
	for(i = 0; i < m - 1; i++)
		scanf("%d", &k[i]);

	int f = 0;
	for(i = 2; i <= m; i++)
		f = (f + k[m - i]) % i;
	printf("%d\n", f + 1);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1039

#include <stdio.h>

int left[80000] = {0};
int right[80000] = {0};
int idx[80000] = {0};

int getindex(int node, int id)
{
	if(node == -1)
		return 0;
	idx[node] = id;
	getindex(left[node], id * 2);
	getindex(right[node], id * 2 + 1);
}

int lastlist(int node)
{
	if(node == -1)
		return 0;
	lastlist(left[node]);
	lastlist(right[node]);
	printf("%d ", node);
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int a, b, c;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%d %d %d", &a, &b, &c);
		left[a] = b;
		right[a] = c;
	}

	getindex(1, 1);
	for(i = 1; i <= n; i++)
		printf("%d ", idx[i]);
	printf("\n");
	lastlist(1);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1040

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int left[n];
	int right[n];
	int i;
	for(i = 0; i < n; i++)
	{
		left[i] = 0;
		right[i] = 0;
	}

	int d;
	for(i = 1; i < n; i++)
	{
		scanf("%d", &d);
		if(left[d] == 0)
			left[d] = i;
		else
			right[d] = i;
	}


	int size = n / 2 + 1;
	int *queue;
	queue = malloc(sizeof(int) * size);
	int start = 0, end = 0;
	int num;
	printf("0 ");

	queue[end] = 0;
	end = (end + 1) % size;
	while(start != end)
	{
		num = queue[start];
		start = (start + 1) % size;

		if(left[num])
		{
			queue[end] = left[num];
			end = (end + 1) % size;
			printf("%d ", left[num]);
		}
		if(right[num])
		{
			queue[end] = right[num];
			end = (end + 1) % size;
			printf("%d ", right[num]);
		}
	}
	printf("\n");
	free(queue);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1041

#include <stdio.h>
#include <stdlib.h>

typedef struct plane
{
	int id;
	int high;
} plane;

int cmp(const void *a, const void *b)
{
	plane *x = (plane *)a, *y = (plane *)b;
	if(x->high > y->high || x->high == y->high && x->id > y->id)
		return 1;
	return 0;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	plane arr[n];
	int i;
	for(i = 0; i < n; i++)
	{
		arr[i].id = i + 1;
		scanf("%d", &arr[i].high);
	}

	qsort(arr, n, sizeof(plane), cmp);
	char cmd[16] = {0};
	while(scanf("%s", cmd) != EOF)
	{
		if(cmd[0] == 'd')
		{
			int x, y;
			scanf("%d %d", &x, &y);
			int px, py;
			for(i = 0; i < n; i++)
			{
				if(arr[i].id == x)
				{
					px = i;
					y = arr[i].high - y;
					break;
				}
			}
			py = -1;
			for(i = 0; i < n; i++)
			{
				if(arr[i].high < y || arr[i].high == y && arr[i].id < x)
					py = i;
				else
					break;
			}
			for(i = px; i > py + 1; i--)
				arr[i] = arr[i - 1];
			arr[i].id = x;
			arr[i].high = y;
			//qsort(arr, n, sizeof(plane), cmp);
		}
		else if(cmd[0] == 'f')
		{
			int m;
			scanf("%d", &m);
			for(i = 0; i < n && arr[i].high <= m; i++)
				;
			printf("%d\n", arr[i].id);
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1042

#include <stdio.h>

int cx[20000] = {0};
int sx[20000] = {0};
int ck[20000] = {0};

int list_before(int node)
{
	int n;
	if(node == 0)
		return 0;

	printf("%d ", node);
	for(n = cx[node]; n > 0; n = sx[n])
		list_before(n);
}

int list_after(int node)
{
	int n;
	if(node == 0)
		return 0;

	for(n = cx[node]; n > 0; n = sx[n])
		list_after(n);
	printf("%d ", node);
}

int list_floor(int node)
{
	int queue[20000] = {0};
	int start = 0, end = 0;

	queue[start] = node;
	start = (start + 1) % 20000;

	int n;
	while(start != end)
	{
		n = queue[end];
		end = (end + 1) % 20000;

		printf("%d ", n);
		for(n = cx[n]; n > 0; n = sx[n])
		{
			queue[start] = n;
			start = (start + 1) % 20000;
		}
	}
	printf("\n");
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int x;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &x);
		scanf("%d %d", &cx[x], &sx[x]);
		ck[cx[x]] = 1;
		ck[sx[x]] = 1;
	}

	int root = 1;
	for(i = 1; i <= n; i++)
		if(ck[i] == 0)
		{
			root = i;
			break;
		}

	list_before(root);
	printf("\n");
	list_after(root);
	printf("\n");
	list_floor(root);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1043

#include <stdio.h>
#include <stdlib.h>

int *left;
int *right;

int tree()
{
	int n;
	scanf("%d", &n);

	int x;
	int i;

	left = malloc(n * sizeof(int));
	right = malloc(n * sizeof(int));
	for(i = 0; i <= n - 1; i++)
		left[i] = right[i] = 0;

	for(i = 1; i <= n - 1; i++)
	{
		scanf("%d", &x);
		if(left[x] == 0)
			left[x] = i;
		else
			right[x] = i;
	}

	int check = 1;
	int num = n / 2 + 1;
	int queue[num];
	int start = 0, end = 0;

	queue[start] = 0;
	start = (start + 1) % num;

	int m;
	while(start != end)
	{
		m = queue[end];
		end = (end + 1) % num;
		if(left[m])
		{
			if(check == 0)
				return 0;
			queue[start] = left[m];
			start = (start + 1) % num;
		}
		else
			check = 0;
		if(right[m])
		{
			if(check == 0)
				return 0;
			queue[start] = right[m];
			start = (start + 1) % num;
		}
		else
			check = 0;
	}
	return 1;
}

int main(int argc, char *args[])
{
	if(tree() == 1)
		printf("true\n");
	else
		printf("false\n");
	free(left);
	free(right);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1044

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct unit
{
	char name[24];
	int food;
	int wood;
	int level;
} unit;

int cmp(const void *a, const void *b)
{
	return ((unit *)b)->level - ((unit *)a)->level;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	unit u[n];
	int i, j;
	for(i = 0; i < n; i++)
		scanf("%s %d %d %d", u[i].name, &u[i].food, &u[i].wood, &u[i].level);
	qsort(u, n, sizeof(unit), cmp);

	int food = 0, wood = 0;
	char type[10];
	int num;
	int farmer = 0;
	int t;
	scanf("%d", &t);

	int fd[1000];
	int wd[1000];
	int fds = 0, fde = 0;
	int wds = 0, wde = 0;
	for(i = 0; i < t; i++)
	{
		scanf("%s %d", type, &num);
		num += farmer;
		if(type[0] == 'f')
		{
			fd[fds] = num;
			fds = (fds + 1) % 1000;
			food += num;
		}
		else
		{
			wd[wds] = num;
			wds = (wds + 1) % 1000;
			wood += num;
		}

		for(j = 0; j < n; j++)
		{
			if(u[j].food <= food && u[j].wood <= wood)
			{
				int tmp = 0;
				while(u[j].food > tmp)
				{
					tmp += fd[fde];
					food -= fd[fde];
					fde = (fde + 1) % 1000;
				}
				tmp = 0;
				while(u[j].wood > tmp)
				{
					tmp += wd[wde];
					wood -= wd[wde];
					wde = (wde + 1) % 1000;
				}
				if(strcmp(u[j].name, "farmer") == 0)
					farmer++;
				printf("#%d: a %s was created.\n", i + 1, u[j].name);
				break;
			}
		}
	}
	printf("food: %d, wood: %d\n", food, wood);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1045

#include <stdio.h>

int child[200001] = {0};
int brother[200001] = {0};
int root[200001] = {0};
int die[200001] = {0};

int addchild(int node, int num)
{
	if(child[node] == 0)
	{
		child[node] = num;
		return 0;
	}
	int n;
	for(n = child[node]; brother[n]; n = brother[n]) ;
	brother[n] = num;
	return 0;
}

int getchild(int node)
{
	die[node] = 1;
	if(child[node] == 0)
		return 0;
	int n, i = 0;
	for(n = child[node]; n; n = brother[n])
	{
		if(die[n] == 0)
			i++;
		root[n] = 1;
	}
	return i;
}

int main(int argc, char *args[])
{
	root[1] = 1;

	int n;
	scanf("%d", &n);

	int num = 1;
	char c[2];
	int x, y;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%s", c);
		if(c[0] == 'B')
		{
			scanf("%d %d", &x, &y);
			addchild(x, y);
		}
		else
		{
			scanf("%d", &x);
			y = getchild(x);
			if(root[x])
				num += y - 1;
			else
				num += y;
			printf("%d\n", num);
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1046

#include <stdio.h>

int left[100001] = {0};
int right[100001] = {0};
int total[100001] = {0};
int parent[100001] = {0};
int root[100001] = {0};
int red[100001] = {0};

int getroot()
{
	int i;
	for(i = 0; i < 100001; i++)
	{
		if(parent[i] == 1 && root[i] == 0)
			return i;
	}
}

int gettotal(int node)
{
	int l, r;
	if(node == 0)
		return 0;
	l = gettotal(left[node]);
	r = gettotal(right[node]);
	total[node] = l + r + 1;
	return l + r + 1;
}

int setred(int node, int num)
{
	if(node == 0)
		return num;
	num = setred(left[node], num);
	num--;
	if(num == 0)
	{
		red[node] = 1;
		return 0xFFFFFFFF;
	}
	num = setred(right[node], num);
	return num;
}

int getred(int node)
{
	if(node == 0)
		return 0;
	return getred(left[node]) + getred(right[node]) + red[node];
}

int main(int argc, char *args[])
{
	int n, p, q;
	scanf("%d %d %d", &n, &p, &q);

	int x;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &x);
		scanf("%d %d", &left[x], &right[x]);
		parent[x] = 1;
		root[left[x]] = 1;
		root[right[x]] = 1;
	}

	int r = getroot();
	gettotal(r);

	int t;
	int y;
	for(i = 0; i < p; i++)
	{
		scanf("%d %d", &t, &x);
		y = x % total[t] + 1;
		setred(t, y);
	}

	int w;
	for(i = 0; i < q; i++)
	{
		scanf("%d", &w);
		printf("%d\n", getred(w));
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1047

#include <stdio.h>

#define clockwise(x, y) clock[x][y] = (clock[x][y] + 3) % 12

int clock[3][3];

int move_1()
{
	clockwise(0, 0);
	clockwise(0, 1);
	clockwise(1, 0);
	clockwise(1, 1);
}

int move_2()
{
	clockwise(0, 0);
	clockwise(0, 1);
	clockwise(0, 2);
}

int move_3()
{
	clockwise(0, 1);
	clockwise(0, 2);
	clockwise(1, 1);
	clockwise(1, 2);
}

int move_4()
{
	clockwise(0, 0);
	clockwise(1, 0);
	clockwise(2, 0);
}

int move_5()
{
	clockwise(0, 1);
	clockwise(1, 0);
	clockwise(1, 1);
	clockwise(1, 2);
	clockwise(2, 1);
}

int move_6()
{
	clockwise(0, 2);
	clockwise(1, 2);
	clockwise(2, 2);
}

int move_7()
{
	clockwise(1, 0);
	clockwise(1, 1);
	clockwise(2, 0);
	clockwise(2, 1);
}

int move_8()
{
	clockwise(2, 0);
	clockwise(2, 1);
	clockwise(2, 2);
}

int move_9()
{
	clockwise(1, 1);
	clockwise(1, 2);
	clockwise(2, 1);
	clockwise(2, 2);
}

int check()
{
	int i, j;
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			if(clock[i][j] != 0)
				return 0;
	return 1;
}

int (*move_func[10])() = {NULL, move_1, move_2, move_3, move_4, move_5, move_6, move_7, move_8, move_9};
int move_step[10] = {0, 4, 3, 4, 3, 5, 3, 4, 3, 4};
int move[10] = {0};
int result[1000][10];
int number = 0;

int main(int argc, char *args[])
{
	int i, j;
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
		{
			scanf("%d", &clock[i][j]);
			clock[i][j] %= 12;
		}

	while(1)
	{
		if(check() == 1)
		{
			for(i = 1; i < 10; i++)
				move[0] += move_step[i] * move[i];
			for(i = 0; i < 10; i++)
				result[number][i] = move[i];
			number++;
		}
		for(i = 1; i < 10; i++)
		{
			move_func[i]();
			move[i]++;
			if(move[i] == 4)
				move[i] = 0;
			else
				break;
		}
		if(i == 10)
			break;
	}
	int min = result[0][0];
	int num = 0;
	for(i = 1; i < number; i++)
		if(min > result[i][0])
		{
			min = result[i][0];
			num = i;
		}
	for(i = 1; i < 10; i++)
		for(j = 0; j < result[num][i]; j++)
			printf("%d ", i);
	printf("\n");

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1048

#include <stdio.h>

int left[1024] = {0};
int right[1024] = {0};
int root[1024] = {0};
int unroot[1024] = {0};

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int x;
	int i;

	n = (n + 1) / 2 - 1;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &x);
		scanf("%d %d", &left[x], &right[x]);
		root[x] = 1;
		unroot[left[x]] = 1;
		unroot[right[x]] = 1;
	}

	for(i = 0; i < 1024; i++)
		if(root[i] == 1 && unroot[i] == 0)
			break;
	int node = i;

	int queue[1024];
	int start = 0, end = 0;
	queue[start] = node;
	start = (start + 1) % 1024;
	while(start != end)
	{
		node = queue[end];
		end = (end + 1) % 1024;
		if(left[node])
		{
			queue[start] = left[node];
			start = (start + 1) % 1024;
		}
		if(right[node])
		{
			queue[start] = right[node];
			start = (start + 1) % 1024;
		}
		printf("%d\n", node);
	}

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1049

#include <stdio.h>

int main(int argc, char *args[])
{
	int t;
	scanf("%d", &t);

	int n, m;
	int i, j, k;
	for(i = 0; i < t; i++)
	{
		scanf("%d %d", &n, &m);

		int stack[m];
		int sp = 0;
		int x;
		int c = 1;

		k = 0; //若不是顺序的则需要用栈
		for(j = 0; j < n; j++)
		{
			scanf("%d", &x);
			if(c == 0)
				continue;
			while(k < x)
			{
				if(sp >= m) //栈溢出
				{
					c = 0;
					break;
				}
				stack[sp] = k;
				sp++;
				k++;
			}
			if(sp > 0 && stack[sp - 1] == x)
				sp--;
			else
			{
				if(k == x)
					k++;
				else
					c = 0;
			}
		}
		if(c == 1)
			printf("YES\n");
		else
			printf("NO\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1050

#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
	struct node *next;
	int data;
} node;

typedef struct list
{
	struct node *head;
	struct node *tail;
	int num;
} list;

list *list_new()
{
	list *l;
	l = malloc(sizeof(list));
	l->head = NULL;
	l->tail = NULL;
	l->num = 0;
	return l;
}

int list_free(list *l)
{
	free(l);
}

int list_push(list *l, int data)
{
	node *n, *p;
	n = malloc(sizeof(node));
	n->next = NULL;
	n->data = data;

	if(l->num == 0)
	{
		l->head = n;
		l->tail = n;
	}
	else if(data <= l->head->data)
	{
		n->next = l->head;
		l->head = n;
	}
	else if(data >= l->tail->data)
	{
		l->tail->next = n;
		l->tail = n;
	}
	else
	{
		for(p = l->head; p->next; p = p->next)
		{
			if(p->next->data >= data)
			{
				n->next = p->next;
				p->next = n;
				break;
			}
		}
	}
	l->num++;
}

//需要释放内存，性能不够需要改成链表合并
int list_add(list *l1, list *l2)
{
	node *n;
	for(n = l2->head; n; n = n->next)
		list_push(l1, n->data);
}

int list_pop_min(list *l)
{
	node *n;
	if(l->num == 0)
		return -1;
	else
		n = l->head;

	l->head = l->head->next;
	if(l->num == 1)
		l->tail = NULL;
	l->num--;

	int data = n->data;
	return data;
}

//内存没有释放
int main(int argc, char *args[])
{
	int n, m;
	scanf("%d %d", &n, &m);

	list *girl[n];
	int data;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &data);
		girl[i] = list_new();
		list_push(girl[i], data);
	}

	int a, b;
	int p;
	for(i = 0; i < m; i++)
	{
		scanf("%d", &p);
		switch(p)
		{
		case 0:
			scanf("%d %d", &a, &b);
			list_add(girl[a], girl[b]);
			break;
		case 1:
			scanf("%d", &a);
			printf("%d\n", list_pop_min(girl[a]));
			break;
		case 2:
			scanf("%d %d", &a, &b);
			list_push(girl[a], b);
			break;
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1051

#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
	struct node *next;
	int data;
} node;

typedef struct list
{
	struct node *head;
	struct node *tail;
} list;

list *list_new()
{
	list *l;
	l = malloc(sizeof(list));
	l->head = NULL;
	l->tail = NULL;
	return l;
}

int list_free(list *l)
{
	free(l);
}

int list_push(list *l, int data)
{
	node *n;
	n = malloc(sizeof(node));
	n->next = NULL;
	n->data = data;

	if(!l->tail)
		l->head = n;
	else
		l->tail->next = n;
	l->tail = n;
}

int list_find(list *l, int data)
{
	node *n, *p;
	int num = 0;

	num++;
	if(l->head->data == data)
		return num;
	for(n = l->head; n->next; n = n->next)
	{
		num++;
		if(n->next->data == data)
		{
			p = n->next;
			n->next = n->next->next;
			p->next = l->head;
			l->head = p;
			return num;
		}
	}
	return num;
}

int main(int argc, char *args[])
{
	int m, n;
	scanf("%d", &n);

	list *l = list_new();
	int data;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%d", &data);
		list_push(l, data);
	}

	scanf("%d", &m);

	int sum = 0;
	for(i = 0; i < m; i++)
	{
		scanf("%d", &data);
		sum += list_find(l, data);
	}
	printf("%d\n", sum);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1052

#include <stdio.h>

int num[101] = {0};
int set[101][201] = {0};

int add(int a, int b)
{
	int i;
	for(i = 0; i < 201; i++)
		if(set[b][i] == 1)
			set[a][i] = 1;
}

int sub(int a, int b)
{
	int i;
	for(i = 0; i < 201; i++)
		if(set[a][i] == 1 && set[b][i] == 1)
			set[a][i] = 0;
}

int mul(int a, int b)
{
	int i;
	for(i = 0; i < 201; i++)
		if(set[a][i] == 1 && set[b][i] == 0)
			set[a][i] = 0;
}

int main(int argc, char *args[])
{
	int m, n;
	scanf("%d", &n);

	int i, j;
	for(i = 1; i <= n; i++)
		scanf("%d", &num[i]);

	int d;
	for(i = 1; i <= n; i++)
		for(j = 0; j < num[i]; j++)
		{
			scanf("%d", &d);
			set[i][d] = 1;
		}

	char c[2];
	int a, b;
	scanf("%d", &m);
	for(i = 0; i < m; i++)
	{
		scanf("%s %d %d", c, &a, &b);
		switch(c[0])
		{
		case '+':
			add(a, b);
			break;
		case '-':
			sub(a, b);
			break;
		case '*':
			mul(a, b);
			break;
		}
	}

	for(i = 1; i <= n; i++)
	{
		for(j = 0; j < 201; j++)
			if(set[i][j] == 1)
				printf("%d ", j);
		printf("\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1053

#include <stdio.h>

int row[100000];
int col[100000];

int main(int argc, char *args[])
{
	int i, j;
	for(i = 0; i < 100000; i++)
	{
		row[i] = i;
		col[i] = i;
	}

	int m, n;
	scanf("%d", &n);

	int x1[n];
	int y1[n];
	int z1[n];
	for(i = 0; i < n; i++)
		scanf("%d %d %d", &x1[i], &y1[i], &z1[i]);

	scanf("%d", &m);

	int op, x, y;
	int tmp;
	for(i = 0; i < m; i++)
	{
		scanf("%d %d %d", &op, &x, &y);
		switch(op)
		{
		case 0:
			tmp = row[x];
			row[x] = row[y];
			row[y] = tmp;
			break;
		case 1:
			tmp = col[x];
			col[x] = col[y];
			col[y] = tmp;
			break;
		case 2:
			x = row[x];
			y = col[y];
			for(j = 0; j < n; j++)
				if(x == x1[j] && y == y1[j])
				{
					printf("%d\n", z1[j]);
					break;
				}
			if(j == n)
				printf("0\n");
			break;
		}
	}

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1054

#include <stdio.h>
#include <stdlib.h>

typedef struct treap
{
	struct treap *l, *r;
	int size;
	int key;
	int data;
} treap;

treap *treap_new(int data)
{
	treap *t;
	t = calloc(1, sizeof(treap));
	t->size = 1;
	t->key = rand();
	t->data = data;
	return t;
}

int treap_free(treap *t)
{
	if(t == NULL)
		return 0;
	treap_free(t->l);
	treap_free(t->r);
	free(t);
}

int treap_size(treap *a)
{
	return a ? a->size : 0;
}

int treap_update(treap *a)
{
	a->size = 1 + treap_size(a->l) + treap_size(a->r);
}

treap *treap_merge(treap *a, treap *b)
{
	if(a == NULL)
		return b;
	if(b == NULL)
		return a;

	if(a->key < b->key)
	{
		a->r = treap_merge(a->r, b);
		treap_update(a);
		return a;
	}
	else
	{
		b->l = treap_merge(a, b->l);
		treap_update(b);
		return b;
	}
}

int treap_split(treap *a, int n, treap **x, treap **y)
{
	treap *p, *q;
	int m;

	if(a == NULL)
	{
		*x = NULL;
		*y = NULL;
		return 0;
	}

	m = treap_size(a->l);
	if(n <= m)
	{
		treap_split(a->l, n, &p, &q);
		a->l = q;
		*x = p;
		*y = a;
	}
	else
	{
		treap_split(a->r, n - m - 1, &p, &q);
		a->r = p;
		*x = a;
		*y = q;
	}
	treap_update(a);
	return 0;
}

int treap_index(treap *a, int id)
{
	int m;
	if(a == NULL)
		return -1;

	m = treap_size(a->l);
	if(id <= m)
		return treap_index(a->l, id);
	if(id == m + 1)
		return a->data;
	return treap_index(a->r, id - m - 1);
}

int treap_find(treap *a, int data)
{
	if(a == NULL)
		return 0;
	if(data <= a->data)
		return treap_find(a->l, data);
	else
		return 1 + treap_size(a->l) + treap_find(a->r, data);
}

int treap_insert(treap **a, int data)
{
	treap *p, *q;
	int k = treap_find(*a, data);
	treap_split(*a, k, &p, &q);
	*a = treap_merge(treap_merge(p, treap_new(data)), q);
}

int treap_delete(treap **a, int k)
{
	treap *p, *q, *s, *t;
	treap_split(*a, k - 1, &p, &q);
	treap_split(q, 1, &s, &t);
	treap_free(s);
	*a = treap_merge(p, t);
}

int main(int argc, char *args[])
{
	srand(0);
	treap *t = NULL;

	int n, min;
	scanf("%d %d", &n, &min);

	treap *p, *q;
	int k;
	int total = 0;
	int sum = 0;
	char c[2];
	int d;
	int i;
	for(i = 0; i < n; i++)
	{
		scanf("%s %d", c, &d);
		switch(c[0])
		{
		case 'I':
			d -= total;
			if(d + total >= min) //初始工资太低的不计算在在内
				treap_insert(&t, d);
			break;
		case 'A':
			total += d;
			break;
		case 'S':
			total -= d;
			k = treap_find(t, min - total);
			sum += k;
			treap_split(t, k, &p, &q);
			treap_free(p);
			t = q;
			break;
		case 'F':
			d = treap_index(t, treap_size(t) - d + 1);
			if(d != -1)
				 d += total;
			printf("%d\n", d);
			break;
		}
	}
	printf("%d\n", sum);
	treap_free(t);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1055

#include <stdio.h>

//有一个样本过不了，原因不明
int main(int argc, char *args[])
{
	char arr[302][302] = {0};
	int sum[302][302] = {0}; //前缀和
	int sumx[302][302] = {0}; //x方向的前缀和
	int sumy[302][302] = {0}; //y方向的前缀和

	int t;
	int r, c;
	int i, j, k;
	int px, py, q;
	int u, v, x, y;
	int n;
	scanf("%d", &t);

	for(k = 0; k < t; k++)
	{
		scanf("%d %d", &r, &c);

		for(i = 1; i <= r; i++)
		{
			scanf("%s", arr[i] + 1);
			for(j = 1; j <= c; j++)
			{
				arr[i][j] -= '0';
				sum[i][j] = arr[i][j] + sum[i - 1][j] + sum[i][j - 1] - sum[i - 1][j - 1];
				sumx[i][j] = arr[i][j] * i + sumx[i - 1][j] + sumx[i][j - 1] - sumx[i - 1][j - 1];
				sumy[i][j] = arr[i][j] * j + sumy[i - 1][j] + sumy[i][j - 1] - sumy[i - 1][j - 1];
			}
		}

		for(n = r < c ? r : c; n >= 3; n--)
		{
			for(i = 1; i <= r - n + 1; i++)
				for(j = 1; j <= c - n + 1; j++)
				{
					u = i + n - 1;
					v = j + n - 1;
					//i ... i + n - 1, j ... j + n - 1
					q = sum[u][v] - sum[u][j - 1] - sum[i - 1][v] + sum[i - 1][j - 1] - arr[u][v] - arr[u][j] - arr[i][v] - arr[i][j];
					if(q == 0) //全为0。。。
					{
						printf("%d\n", n);
						goto loop;
					}
					px = sumx[u][v] - sumx[u][j - 1] - sumx[i - 1][v] + sumx[i - 1][j - 1] - (arr[u][v] + arr[u][j]) * u - (arr[i][v] + arr[i][j]) * i;
					py = sumy[u][v] - sumy[u][j - 1] - sumy[i - 1][v] + sumy[i - 1][j - 1] - (arr[u][v] + arr[i][v]) * v - (arr[u][j] + arr[i][j]) * j;

					x = (2 * px) - (u + i) * q;
					y = (2 * py) - (v + j) * q;
					if(x == 0 && y == 0)
					{
						printf("%d\n", n);
						goto loop;
					}
				}
		}
		printf("IMPOSSIBLE\n");
loop: ;
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1056

#include <stdio.h>
#include <stdlib.h>

typedef struct treap
{
	struct treap *l, *r;
	int size;
	int key;
	int data;
} treap;

treap *treap_new(int data)
{
	treap *a = calloc(1, sizeof(treap));
	a->size = 1;
	a->key = rand();
	a->data = data;
	return a;
}

int treap_free(treap *a)
{
	if(a == NULL)
		return 0;
	treap_free(a->l);
	treap_free(a->r);
	free(a);
}

int treap_size(treap *a)
{
	return a ? a->size : 0;
}

int treap_update(treap *a)
{
	a->size = treap_size(a->l) + treap_size(a->r) + 1;
}

treap *treap_merge(treap *a, treap *b)
{
	if(a == NULL)
		return b;
	if(b == NULL)
		return a;
	if(a->key < b->key)
	{
		a->r = treap_merge(a->r, b);
		treap_update(a);
		return a;
	}
	else
	{
		b->l = treap_merge(a, b->l);
		treap_update(b);
		return b;
	}
}

int treap_split(treap *a, int k, treap **p, treap **q)
{
	if(a == NULL)
	{
		*p = NULL;
		*q = NULL;
		return 0;
	}
	treap *s, *t;
	int m = treap_size(a->l);
	if(k <= m)
	{
		treap_split(a->l, k, &s, &t);
		a->l = t;
		*p = s;
		*q = a;
	}
	else
	{
		treap_split(a->r, k - m - 1, &s, &t);
		a->r = s;
		*p = a;
		*q = t;
	}
	treap_update(a);
	return 0;
}

int treap_find(treap *a, int data)
{
	if(a == NULL)
		return 0;
	if(data <= a->data)
		return treap_find(a->l, data);
	return treap_find(a->r, data) + treap_size(a->l) + 1;
}

int treap_index(treap *a, int k)
{
	if(a == NULL)
		return 0;
	int m = treap_size(a->l);
	if(k <= m)
		return treap_index(a->l, k);
	if(k == m + 1)
		return a->data;
	return treap_index(a->r, k - m - 1);
}

int treap_insert(treap **a, int data)
{
	treap *p, *q, *s, *t;
	int k = treap_find(*a, data);
	treap_split(*a, k, &p, &q);
	*a = treap_merge(treap_merge(p, treap_new(data)), q);
}

int treap_delete(treap **a, int data)
{
	treap *p, *q, *s, *t;
	int k = treap_find(*a, data);
	treap_split(*a, k, &p, &q);
	treap_split(q, 1, &s, &t);
	treap_free(s);
	*a = treap_merge(p, t);
}

treap *root = NULL;
int arr[500001] = {0};
int box[500001] = {0};
int num[500001] = {0};
int total;

int add(int x, int y)
{
	int a = arr[x];
	int b = arr[y];
	while(box[a])
		a = box[a];
	while(box[b])
		b = box[b];
	if(a != b && num[a] && num[b])
	{
		box[b] = a;
		if(num[b] > 1)
			treap_delete(&root, num[b]);
		else
			total--;
		if(num[a] > 1)
			treap_delete(&root, num[a]);
		else
			total--;
		num[a] += num[b];
		treap_insert(&root, num[a]);
	}
}

int del(int x)
{
	int a = arr[x];
	while(box[a])
		a = box[a];
	if(num[a])
	{
		if(num[a] > 1)
			treap_delete(&root, num[a]);
		total--;
		num[a] = 0;
	}
}

int main(int argc, char *args[])
{
	int i;
	for(i = 0; i < 500001; i++)
	{
		arr[i] = i;
		num[i] = 1;
	}

	int m, n;
	scanf("%d %d", &n, &m);
	total = n;

	char c[2];
	int x, y;
	while(m--)
	{
		scanf("%s", c);
		switch(c[0])
		{
		case 'C':
			scanf("%d %d", &x, &y);
			add(x, y);
			break;
		case 'D':
			scanf("%d", &x);
			del(x);
			break;
		case 'Q':
			scanf("%d", &x);
			if(x > treap_size(root) + total)
				y = 0;
			else
			{
				y = treap_index(root, treap_size(root) - x + 1);
				if(y == 0)
					y = 1;
			}
			printf("%d\n", y);
			break;
		}
	}
	treap_free(root);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1057

#include <stdio.h>
#include <stdlib.h>

typedef struct treap
{
	struct treap *l, *r;
	int size;
	int key;
	int data;
} treap;

treap *treap_new(int data)
{
	treap *a = malloc(sizeof(treap));
	a->l = NULL;
	a->r = NULL;
	a->size = 1;
	a->key = rand();
	a->data = data;
	return a;
}

int treap_free(treap *a)
{
	if(a == NULL)
		return 0;
	treap_free(a->l);
	treap_free(a->r);
	free(a);
}

int treap_size(treap *a)
{
	return a ? a->size : 0;
}

int treap_update(treap *a)
{
	a->size = treap_size(a->l) + treap_size(a->r) + 1;
}

treap *treap_merge(treap *a, treap *b)
{
	if(a == NULL)
		return b;
	if(b == NULL)
		return a;
	if(a->key < b->key)
	{
		a->r = treap_merge(a->r, b);
		treap_update(a);
		return a;
	}
	else
	{
		b->l = treap_merge(a, b->l);
		treap_update(b);
		return b;
	}
}

int treap_split(treap *a, int k, treap **p, treap **q)
{
	if(a == NULL)
	{
		*p = NULL;
		*q = NULL;
		return 0;
	}
	treap *s, *t;
	int m = treap_size(a->l);
	if(k <= m)
	{
		treap_split(a->l, k, &s, &t);
		a->l = t;
		*p = s;
		*q = a;
	}
	else
	{
		treap_split(a->r, k - m - 1, &s, &t);
		a->r = s;
		*p = a;
		*q = t;
	}
	treap_update(a);
	return 0;
}

int treap_find(treap *a, int data)
{
	if(a == NULL)
		return 0;
	if(data <= a->data)
		return treap_find(a->l, data);
	return treap_find(a->r, data) + treap_size(a->l) + 1;
}

int treap_index(treap *a, int k)
{
	if(a == NULL)
		return 0;
	int m = treap_size(a->l);
	if(k <= m)
		return treap_index(a->l, k);
	if(k == m + 1)
		return a->data;
	return treap_index(a->r, k - m - 1);
}

int treap_insert(treap **a, int data)
{
	treap *p, *q;
	int k = treap_find(*a, data);
	treap_split(*a, k, &p, &q);
	*a = treap_merge(treap_merge(p, treap_new(data)), q);
}

int treap_delete(treap **a, int x, int y)
{
	treap *p, *q, *s, *t;
	int k1 = treap_find(*a, x);
	int k2 = treap_find(*a, y + 1);
	treap_split(*a, k1, &p, &q);
	treap_split(q, k2 - k1, &s, &t);
	int size = treap_size(s);
	treap_free(s);
	*a = treap_merge(p, t);
	return size;
}

int main(int argc, char *args[])
{
	int m;
	scanf("%d", &m);

	treap *root = NULL;
	char c[2];
	int x, y;
	while(m--)
	{
		scanf("%s", c);
		switch(c[0])
		{
		case 'L':
			scanf("%d", &x);
			printf("%d\n", treap_index(root, treap_size(root) - x + 1));
			break;
		case 'S':
			scanf("%d %d", &x, &y);
			printf("%d\n", treap_delete(&root, x, y));
			break;
		case 'Z':
			scanf("%d", &x);
			treap_insert(&root, x);
			break;
		}
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1058

/*
p个人真话，q个人假话
p个人：
不到N个人比我工作得多，... n >= p ... 1, p <= n
至少M个人的工资比我高，1 ... p ... 1 ... m, p + m <= p + q
q个人则相反
1 ... q ... 1 ... n, q + n <= p + q
... m >= q ... 1, q <= m
p = n, q = m
*/
#include <stdio.h>

int main(int argc, char *args[])
{
	long long m, n;
	scanf("%lld %lld", &n, &m);
	printf("%lld\n", m + n);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1059

#include <stdio.h>
#include <stdlib.h>

int arr[2000] = {0};

int cmp(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int i, j, k;
	for(i = 0; i < n; i++)
		scanf("%d", &arr[i]);

	qsort(arr, n, sizeof(int), cmp);

	int r, s, t;
	int m = 0;
	for(i = 0; i < n; i++)
		for(j = 0; j < i; j++)
		{
			//没有超时，如果超时可以用位图
			r = (((long long)arr[i]) * ((long long)arr[j])) % 100000000;
			s = 0;
			t = n - 1;
			while(s <= t)
			{
				k = (s + t) / 2;
				if(r == arr[k])
				{
					if(arr[i] != arr[k] && arr[j] != arr[k] && arr[i] != arr[j])
						m += 2;
					break;
				}
				else if(r < arr[k])
					t = k - 1;
				else
					s = k + 1;
			}
		}
	printf("%d\n", m);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1060

#include <stdio.h>

/*
i, j, k有一个必为move[m]，假设i == move[m]
f[m][move[m]][j][k] = f[m - 1][s][j][k] + cost[s][move[m]]
f[m][i][move[m]][k] = f[m - 1][i][s][k] + cost[s][move[m]]
f[m][i][j][move[m]] = f[m - 1][i][j][s] + cost[s][move[m]]

i, j, move[m - 1]
f[m][i][j](move[m])           = f[m - 1][i][j](move[m - 1]) + cost[move[m - 1]][move[m]]
f[m][i][move[m - 1]](move[m]) = f[m - 1][i][j](move[m - 1]) + cost[j][move[m]]
f[m][move[m - 1]][j](move[m]) = f[m - 1][i][j](move[m - 1]) + cost[i][move[m]]
m - 1移动到m，i, j, move[m - 1]，可能移动三个中的一个
*/
int main(int argc, char *args[])
{
	int cost[201][201] = {0};
	int l, n, m;
	scanf("%d %d", &l, &n);

	int i, j, k;
	for(i = 1; i <= l; i++)
		for(j = 1; j <= l; j++)
			scanf("%d", &cost[i][j]);

	int move[n];
	for(i = 0; i < n; i++)
		scanf("%d", &move[i]);

	int f[2][201][201] = {0xFFFFFF};
	for(i = 1; i <= l; i++)
		for(j = 1; j <= l; j++)
			f[1][i][j] = 0xFFFFFF;

	if(move[0] == 1 || move[0] == 2 || move[0] == 3)
	{
		f[1][1][2] = 0;
		f[1][1][3] = 0;
		f[1][2][3] = 0;
	}
	else
	{
		f[1][1][2] = cost[3][move[0]];
		f[1][1][3] = cost[2][move[0]];
		f[1][2][3] = cost[1][move[0]];
	}

	int p, q;
	int	s, t;
	for(k = 1; k < n; k++)
	{
		s = k % 2;
		t = (k + 1) % 2;
		p = move[k - 1];
		q = move[k];
		for(i = 1; i <= l; i++)
			for(j = 1; j <= l; j++)
				f[t][i][j] = 0xFFFFFF;
		for(i = 1; i <= l; i++)
			for(j = 1; j <= l; j++)
			{
				if(q == i)
				{
					if(f[t][p][j] > f[s][i][j])
						f[t][p][j] = f[s][i][j];
				}
				else if(q == j)
				{
					if(f[t][i][p] > f[s][i][j])
						f[t][i][p] = f[s][i][j];
				}
				else if(q == p)
				{
					if(f[t][i][j] > f[s][i][j])
						f[t][i][j] = f[s][i][j];
				}
				else
				{
					if(f[t][i][j] > f[s][i][j] + cost[p][q])
						f[t][i][j] = f[s][i][j] + cost[p][q];
					if(f[t][i][p] > f[s][i][j] + cost[j][q])
						f[t][i][p] = f[s][i][j] + cost[j][q];
					if(f[t][p][j] > f[s][i][j] + cost[i][q])
						f[t][p][j] = f[s][i][j] + cost[i][q];
				}
			}
	}

	int min = 0xFFFFFF;
	for(i = 1; i <= l; i++)
		for(j = 1; j <= l; j++)
			if(min > f[t][i][j])
				min = f[t][i][j];
	printf("%d\n", min);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1061

#include <stdio.h>

/*
total[k]表示k到n总花费的最小值
mark[k]表示total[k]取最小值时最前面直接复制文件的服务器的编号
则k到mark[k]之间的服务器全部为间接复制
表明cost[i] > mark[k] - i (i >= k && i < mark[k])
mark[k] <= mark[k + 1]
total[k] = total[mark[k]] + (mark[k] - k + 1) * (mark[k] - k) / 2
 1  2  3  4  5  6  7  8  9 10
 2  3  1  5  4  5  6  3  1  2
 2  1  2  1  4  3  2  1  1  0
18 16 15 14 13  9  6  4  3  2
需要斜率优化，否则最后一个样例会超时
*/
int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int cost[n + 1];
	int i, j, k;
	for(i = 1; i <= n; i++)
		scanf("%d", &cost[i]);

	long long total[n + 1];
	total[n] = cost[n];
	int mark = n;
	long long tmp;
	for(i = n - 1; i >= 1; i--)
	{
		total[i] = total[i + 1] + mark - i;

		for(j = mark - 1; j >= i; j--)
		{
			tmp = ((long long)(j - i + 1)) * (j - i) / 2 + total[j + 1] + cost[j];
			if(total[i] > tmp)
			{
				total[i] = tmp;
				mark = j;
			}
		}
	}

	printf("%lld\n", total[1]);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1062

#include <stdio.h>

/*
最长递增子序列可以优化到O(nlogn)的时间复杂度
*/
int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int robot[n + 1];
	int inc[n + 1];
	int dec[n + 1];
	int i, j, k;
	for(i = 1; i <= n; i++)
	{
		scanf("%d", &robot[i]);
		inc[i] = dec[i] = 1;
	}

	for(i = 1; i <= n; i++)
	{
		k = n + 1 - i;
		for(j = 1; j < i; j++)
			if(robot[i] > robot[j])
				inc[i] = inc[i] > inc[j] + 1 ? inc[i] : inc[j] + 1;
		for(j = n; j > k; j--)
			if(robot[k] > robot[j])
				dec[k] = dec[k] > dec[j] + 1 ? dec[k] : dec[j] + 1;
	}

	int max = 0;
	for(i = 1; i <= n; i++)
	{
		if(max < inc[i] + dec[i] - 1)
			max = inc[i] + dec[i] - 1;
	}
	printf("%d\n", n - max);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1063

#include <stdio.h>

typedef struct tree
{
	int data;
	int row, col;
	struct tree *up, *down, *left, *right;
} tree;

tree high[501][501] = {0};
int root[501][501] = {0};

int dfs(tree *t)
{
	if(t == NULL)
		return 0;
	int max = 0;
	int tmp;
	tmp = dfs(t->up);
	max = max > tmp ? max : tmp;
	tmp = dfs(t->down);
	max = max > tmp ? max : tmp;
	tmp = dfs(t->left);
	max = max > tmp ? max : tmp;
	tmp = dfs(t->right);
	max = max > tmp ? max : tmp;
	return max + 1;
}

int main(int argc, char *args[])
{
	int r, c;
	scanf("%d %d", &r, &c);

	int i, j;
	for(i = 1; i <= r; i++)
		for(j = 1; j <= c; j++)
			scanf("%d", &high[i][j].data);
	for(i = 1; i <= r; i++)
		for(j = 1; j <= c; j++)
		{
			high[i][j].row = i;
			high[i][j].col = j;
			if(i != 1 && high[i][j].data > high[i - 1][j].data)
				high[i][j].up = &high[i - 1][j];
			if(i != r && high[i][j].data > high[i + 1][j].data)
				high[i][j].down = &high[i + 1][j];
			if(j != 1 && high[i][j].data > high[i][j - 1].data)
				high[i][j].left = &high[i][j - 1];
			if(j != c && high[i][j].data > high[i][j + 1].data)
				high[i][j].right = &high[i][j + 1];
		}
	for(i = 1; i <= r; i++)
		for(j = 1; j <= c; j++)
		{
			if(high[i][j].up)
				root[high[i][j].up->row][high[i][j].up->col] = 1;
			if(high[i][j].down)
				root[high[i][j].down->row][high[i][j].down->col] = 1;
			if(high[i][j].left)
				root[high[i][j].left->row][high[i][j].left->col] = 1;
			if(high[i][j].right)
				root[high[i][j].right->row][high[i][j].right->col] = 1;
		}

	int max = 0;
	int tmp;
	for(i = 1; i <= r; i++)
		for(j = 1; j <= c; j++)
			if(root[i][j] == 0)
			{
				tmp = dfs(&high[i][j]);
				max = max > tmp ? max : tmp;
			}
	printf("%d\n", max);

	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1064

#include <stdio.h>

/*
尚有3个样例不通过，原因未明
*/
int main(int argc, char *args[])
{
	int n;
	scanf("%d", &n);

	int stock[n + 2];
	int dec[n + 2];
	int num[n + 2];
	int i, j, k;
	for(i = 1; i <= n; i++)
	{
		scanf("%d", &stock[i]);
		dec[i] = 1;
		num[i] = 1;
	}
	stock[n + 1] = dec[n + 1] = num[n + 1] = 0;

	for(i = 1; i <= n + 1; i++)
	{
		int visit[5001] = {0};
		int top = 0;
		int visited;
		for(j = i - 1; j >= 1; j--)
			if(stock[j] > stock[i])
			{
				if(dec[i] < dec[j] + 1)
				{
					dec[i] = dec[j] + 1;
					num[i] = num[j];
					top = 0;
					visit[top++] = stock[j];
				}
				else if(dec[i] == dec[j] + 1)
				{
					visited = 0;
					for(k = 0; k < top; k++)
					{
						if(stock[j] == visit[k])
						{
							visited = 1;
							break;
						}
					}
					if(visited == 0)
					{
						num[i] += num[j];
						visit[top++] = stock[j];
					}
				}
			}
	}

	int max = 0;
	int m = 0;
	for(i = 1; i <= n + 1; i++)
	{
		if(max < dec[i])
		{
			max = dec[i];
			m = num[i];
		}
	}
	printf("%d %d\n", max - 1, m);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1065

#include <stdio.h>
#include <string.h>

/*
f[m][n] = 0, m == 0 || n == 0
f[m][n] = f[m - 1][n - 1] + 1, str1[m] == str2[n]
f[m][n] = max(f[m - 1][n], f[m][n - 1]), str1[m] != str2[n]
*/
int main(int argc, char *args[])
{
	char str1[1002] = {0};
	char str2[1002] = {0};
	scanf("%s", str1 + 1);
	scanf("%s", str2 + 1);
	int m = strlen(str1 + 1);
	int n = strlen(str2 + 1);

	char f[1001][1001] = {0};
	int i, j;
	for(i = 0; i <= m; i++)
		for(j = 0; j <= n; j++)
		{
			if(i == 0 || j == 0)
				f[i][j] = 0;
			else if(str1[i] == str2[j])
				f[i][j] = f[i - 1][j - 1] + 1;
			else
				f[i][j] = f[i - 1][j] > f[i][j - 1] ? f[i - 1][j] : f[i][j - 1];
		}

	printf("%d\n", f[m][n]);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1066

#include <stdio.h>

/*
  a b c d e f d g
g               1
d       1 1 1 1 1
f       1 1 2 2 2
e       1 2 2 2 2
d       1 2 2 3 3
c     1
b   1 1
a 1 1 1
对于任意字母，若增加费用大于删除费用，增加一个对应字母不如删除该字母，若删除费用大于增加费用，删除该字母不如增加对应字母，因此对每个字母的操作费用只需考虑增加删除中的最小值即可
序列去除对称子序列后剩余字母必须进行一次操作使得该字母对称
则问题相当于计算子序列操作费用和最大的对称子序列（即正序和逆序的最大子序列）
需要考虑m, n为0和name为空的情况
*/
int f[2001][2001] = {0}; //大数组不要放在函数内

int main(int argc, char *args[])
{
	int n, m;
	scanf("%d %d", &n, &m);

	char name[2002] = {0};
	scanf("%s", name + 1);

	int min[26] = {0};
	int a, b;
	char c[2];
	int i, j, k;
	for(i = 0; i < n; i++)
	{
		scanf("%s %d %d", c, &a, &b);
		min[c[0] - 'a'] = a < b ? a : b;
	}

	for(i = 1; i <= m; i++)
		for(j = 1; j <= m; j++)
		{
			if(name[i] == name[m + 1 - j])
				f[i][j] = f[i - 1][j - 1] + min[name[i] - 'a'];
			else
				f[i][j] = f[i - 1][j] > f[i][j - 1] ? f[i - 1][j] : f[i][j - 1];
		}
	int max = 0;
	for(i = 1; i <= m; i++)
		max += min[name[i] - 'a'];
	printf("%d\n", max - f[m][m]);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1067

#include <stdio.h>

//(1 + 2 + 3 + 4 + 5 + 6) * 20000 / 2 = 210000
int f[7][210001] = {0};

int main(int argc, char *args[])
{
	int a[7];
	int n;
	int dv = 0;
	for(n = 1; 1; n++)
	{
		scanf("%d %d %d %d %d %d", &a[1], &a[2], &a[3], &a[4], &a[5], &a[6]);
		if(!a[1] && !a[2] && !a[3] && !a[4] && !a[5] && !a[6])
			break;

		int total = 0;
		int i, j, k;
		for(i = 1; i <= 6; i++)
			total += a[i] * i;
		dv = 0;
		if(total % 2 == 0)
		{
			total /= 2;
			//多重背包
			for(i = 0; i <= total; i++)
				f[1][i] = a[1] < i ? a[1] : i;
			for(k = 2; k <= 6; k++)
			{
				for(i = 0; i <= total; i++)
				{
					if(i < k)
						f[k][i] = f[k - 1][i];
					else
					{
						int max = 0;
						int tmp;
						for(j = 1; j <= a[k] && i >= j * k; j++)
						{
							tmp = f[k - 1][i - j * k] + j * k;
							max = max > tmp ? max : tmp;
						}
						f[k][i] = max;
					}
				}
			}
			if(f[6][total] == total)
				dv = 1;
		}
		printf("Collection #%d:\n", n);
		if(dv == 1)
			printf("Can be divided.\n");
		else
			printf("Can't be divided.\n");
		printf("\n");
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1068

#include <stdio.h>

char f[2000001] = {0}; //价格能否凑出
int max = 0; //能凑出的最大值
int min = 1; //不能凑出的最小值

int main(int argc, char *args[])
{
	int k, n;
	scanf("%d %d", &k, &n);

	int price[n + 1];
	int pmax = 0;
	int i, j, t;
	for(i = 1; i <= n; i++)
	{
		scanf("%d", &price[i]);
		pmax = pmax > price[i] ? pmax : price[i];
	}

	f[0] = 1;
	for(i = 1; i <= k; i++)
	{
		int pmin = min - pmax > 0 ? min - pmax : 0;
		for(j =	max; j >= pmin; j--)
		{
			if(f[j])
			{
				for(t = 1; t <= n; t++)
				{
					int tmp = j + price[t];
					f[tmp] = 1;
					max = max > tmp ? max : tmp;
				}
			}
		}
		while(f[min])
			min++;
	}

	printf("%d\n", min - 1);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1069

#include <stdio.h>

/*
将若干个数量的物品合并，将数量遍历复杂度的O(n)降低到O(logn)
将数量依次减去2的幂，直到不够减，剩余的作为最后一个元素
1, 2, 4, 8, ..., 2 ^ k, m
将每个元素的价值合并（乘以数量）看作一个元素
*/
int main(int argc, char *args[])
{
	int n, m;
	while(1)
	{
		scanf("%d %d", &n, &m);
		if(n == 0 && m == 0)
			break;
		int ptmp[n + 1];
		int i, j;
		for(i = 1; i <= n; i++)
			scanf("%d", &ptmp[i]);

		int price[2000] = {0};
		int index = 1;
		for(i = 1; i <= n; i++)
		{
			int num, tmp;
			scanf("%d", &num);

			for(tmp = 1; num > 0; num -= tmp, tmp *= 2)
			{
				if(num >= tmp)
					price[index++] = tmp * ptmp[i];
				else
				{
					price[index++] = num * ptmp[i];
					break;
				}
			}
		}
		index--;

		char f[100001] = {0};
		f[0] = 1;
		int max = 0; //能凑出的最大值
		for(i = 1; i <= index; i++)
		{
			//去掉max也能通过，样例时间上有增有减
			for(j = m < (max + price[i]) ? m : (max + price[i]); j >= price[i]; j--)
			{
				if(f[j - price[i]])
				{
					f[j] = 1;
					max = max > j ? max : j;
				}
			}
		}
		int total = 0;
		for(i = 1; i <= m; i++)
			if(f[i])
				total++;
		printf("%d\n", total);
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1070

#include <stdio.h>

/*
标准的01背包问题，但需要保存前若干大的价值和
只过了两个样例
*/
int V[301] = {0};
int W[301] = {0};
int dp[6001] = {0};

//因70不大，可直接顺序查找，如果该值较大，可考虑用treap等二叉搜索树
int data[6001][70] = {0}; //保存前70大的数据
int num[6001] = {0}; //保存了多少个数据

//只有70个大小的优先队列
int add(int a, int b, int w)
{
	int tmp[70] = {0};
	int i = 0, j = 0, k = 0;
	while(i < num[a] && j < num[b] && k < 70)
	{
		if(data[a][i] > data[b][j] + w)
			tmp[k++] = data[a][i++];
		else
			tmp[k++] = data[b][j++] + w;
	}
	if(i >= num[a])
		while(j < num[b] && k < 70)
			tmp[k++] = data[b][j++] + w;
	else if(j >= num[b])
		while(i < num[a] && k < 70)
			tmp[k++] = data[a][i++];

	for(i = 0; i < k; i++)
		data[a][i] = tmp[i];
	num[a] = k;
}

int main(int argc, char *args[])
{
	int k, v, n;
	scanf("%d %d %d", &k, &v, &n);

	int i, j;
	int t;
	for(i = 1; i <= n; i++)
		scanf("%d %d", &V[i], &W[i]);

	for(i = 0; i <= 6000; i++)
		num[i] = 1;
	for(i = 1; i <= n; i++)
	{
		for(j = v; j >= V[i]; j--)
		{
			if(dp[j] == dp[j - V[i]] + V[i] || dp[j] == 0)
				add(j, j - V[i], W[i]);
		}
	}

	k = k < num[v] ? k : num[v];
	int total = 0;
	for(i = 0; i < k; i++)
		total += data[v][i];
	printf("%d\n", total);
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1071

#include <stdio.h>
#include <string.h>

/*
add a d1, change a b d2 -> add b d1 + d2
change a b d1, erase b d2 -> erase a d1 + d2
change a b d1, change b c d2 -> change a c d1 + d2
a -> add a, erase a, change a b
*/
#define MAX 10000000
int main(int argc, char *args[])
{
	char str[100] = {0};
	scanf("%s", str);

	int len = strlen(str);

	int n;
	scanf("%d", &n);

	int add[26] = {0};
	int erase[26] = {0};
	int change[26][26] = {0};
	int i, j, k;
	for(i = 0; i < 26; i++)
	{
		add[i] = MAX;
		erase[i] = MAX;
		for(j = 0; j < 26; j++)
			change[i][j] = MAX;
	}

	char s[10];
	char c1[2], c2[2];
	int d;
	for(i = 0; i < n; i++)
	{
		scanf("%s", s);
		switch(s[0])
		{
		case 'a':
			scanf("%s %d", c1, &d);
			add[c1[0] - 'a'] = d;
			break;
		case 'e':
			scanf("%s %d", c1, &d);
			erase[c1[0] - 'a'] = d;
			break;
		case 'c':
			scanf("%s %s %d", c1, c2, &d);
			change[c1[0] - 'a'][c2[0] - 'a'] = d;
			break;
		}
	}

	//将间接操作变为直接操作
	int check = 0;
	do
	{
		check = 0;
		for(i = 0; i < 26; i++)
		{
			if(add[i] < MAX)
			{
				for(j = 0; j < 26; j++)
					if(change[i][j] < MAX && add[j] > add[i] + change[i][j])
					{
						add[j] = add[i] + change[i][j];
						check = 1;
					}
			}
			if(erase[i] < MAX)
			{
				for(j = 0; j < 26; j++)
					if(change[j][i] < MAX && erase[j] > change[j][i] + erase[i])
					{
						erase[j] = change[j][i] + erase[i];
						check = 1;
					}
			}
			for(j = 0; j < 26; j++)
			{
				if(change[i][j] < MAX)
				{
					for(k = 0; k < 26; k++)
					{
						if(i == k)
							continue;
						if(change[j][k] < MAX && change[i][k] > change[i][j] + change[j][k])
						{
							change[i][k] = change[i][j] + change[j][k];
							check = 1;
						}
					}
				}
			}
		}
	} while(check);

	for(i = 0; i < len; i++)
	{
	}
	return 0;
}

#endif


#ifdef ONLINE_JUDGE_1072

#include <stdio.h>

int main(int argc, char *args[])
{
	return 0;
}

#endif


#endif