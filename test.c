#ifdef TEST

#include "test.h"

static int test_is_print = 1;

static int test_isstr(char *s)
{
	int inum, pnum;
	char *t;

	inum = pnum = 0;
	for(t = s; *t; t++)
	{
		switch(*t)
		{
		case '0' ... '9':
			inum++;
			break;
		case '.':
			pnum++;
			break;
		default:
			return TRUE;
		}
	}
	if(pnum)
		return FALSE;
	if(inum)
		return FALSE;
	return TRUE;
}

int test_assert_imple(char *s1, char *ss1, char *s2, char *ss2, int type, char *equal, const char *funcname)
{
	int test_i;
	char test_s[128];

	switch(type)
	{
	case 'f': case 'g':
	case 'd': case 'x': case 'p': case 'c':
		test_i = s1 == s2;
		break;
	case 's':
		test_i = s1 && s2 ? !strcmp(s1, s2) : s1 == s2 ? 1 : 0;
		break;
	}
	if(test_is_print)
	{
		sprintf(test_s, "<%s assert %%s %s %%s> : %%s is %%%c, %%s is %%%c\n", funcname, equal, type, type);
		switch(type)
		{
		case 'f': case 'g':
			printf(test_s, ss1, ss2, ss1, castdouble(s1), ss2, castdouble(s2));
			break;
		default:
			printf(test_s, ss1, ss2, ss1, s1, ss2, s2);
		}
		fflush(stdout);
	}
	if(equal[0] == 'u')
		test_i = !test_i;
	assert(test_i);
}

__attribute__((constructor)) void test_argv()
{
	int argc = 0;
	char *argv[1024] = {0};
	FILE *fp;
	struct timeval g_tv_s, g_tv_e;
	double g_tv;

	tools_init();
	logging_init(NULL);


	gettimeofday(&g_tv_s, NULL);
	printf("****** auto test begin ******\n\n\n");

	setvbuf(stdout, NULL, _IONBF, 0);
#ifdef _WIN32
	LPTSTR line;
	line = GetCommandLine();

	argv[argc++] = line;
	line = strstr(line, " ");

	while(line)
	{
		*line++ = 0;
		argv[argc++] = line;
		line = strstr(line, " ");
	}
#else
	char name[32];
	char result[1024];
	char *line;

	sprintf(name, "/proc/%d/cmdline", getpid());
	fp = fopen(name, "rb");
	fgets(result, 1024, fp);
	fclose(fp);

	line = result;
	while(*line)
	{
		argv[argc++] = line;
		line += strlen(line) + 1;
	}
#endif

	dict *d;
	char *state = NULL;
	char *value = NULL;
	int i;

	d = dict_new();

	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			state = argv[i];
			value = NULL;
			dict_set(d, argv[i], 1);
		}
		else
		{
			if(state)
			{
				if(value == NULL || test_isstr(argv[i]) == TRUE)
				{
					value = argv[i];
					if(state[1] == '-')
						dict_set(d, state, argv[i]);
					else
						dict_set(d, state, argv[i], 1);
				}
				else
					dict_set(d, state, value, atoi(argv[i]));
			}
		}
	}

	if(dict_get(d, "--noprint"))
	{
		test_is_print = 0;
	}

	char cmd[1024];
	char info[1024];
	char wildcard[1024];
	char *func;
	char *s, *t;
	__int64 tmp;
	__int64 num;

	sprintf(cmd, "nm %s\n", argv[0]);
	fp = popen(cmd, "r");
	while(fgets(info, 1024, fp))
	{
		s = strstr(info, " T ");
		if(!s) //筛选函数
			continue;
		*s = 0;
		func = s + 3; //定位到函数开始

		s = strstr(func, "_test"); //筛选_test结束的函数
		if(!s)
			continue;
		*s = 0;
		if(*(s + 6))
			continue;

		if(dict_get(d, "-f", func))
			continue;

		tmp = 1;
		if(dict_get(d, "-t"))
		{
			s = wildcard;
			t = func;
			while(*t)
				*s++ = *t++;
			while(s >= wildcard)
			{
				*s = '+';
				*(s + 1) = 0;
				if(tmp = (__int64)dict_get(d, "-t", wildcard))
					break;
				s--;
			}
			if(s < wildcard)
				if(!(tmp = (__int64)dict_get(d, "-t", func)))
					continue;
		}
		num = tmp ? tmp : 1;

		int interval = num + 1;
		tmp = (__int64)dict_get(d, "--interval");
		if(tmp)
		{
			interval = strtol((void *)tmp, NULL, 10);
			if(interval == 0)
				interval = 1;
		}

		struct timeval tv_s, tv_e;
		double tv;
		__int64 m_s, m_e;
		__int64 t_s, t_e;

		gettimeofday(&tv_s, NULL);
		m_s = lib_memory_total();
		t_s = lib_memory_time();
		printf("****** test begin %s ******\n", func);
		for(i = 0; i < num; i++)
		{
			((void(*)(void))strtol(info, NULL, 16))();
			if((i + 1) % interval)
				continue;
			gettimeofday(&tv_e, NULL);
			tv = (tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec) * 1.0 TO(S);

			m_e = lib_memory_total();
			t_e = lib_memory_time();
			if((m_e - m_s) TO(MB))
				printf("****** test %s %d %fs %lld %lldMB %lldKB******\n", func, num, tv, t_e - t_s, (m_e - m_s) TO(MB), (m_e - m_s) TO(KB) % 1024);
			else if((m_e - m_s) TO(KB))
				printf("****** test %s %d %fs %lld %lldKB %lldB******\n", func, num, tv, t_e - t_s, (m_e - m_s) TO(KB), (m_e - m_s) % 1024);
			else
				printf("****** test %s %d %fs %lld %lldB******\n", func, num, tv, t_e - t_s, m_e - m_s);
		}
		gettimeofday(&tv_e, NULL);
		tv = (tv_e.tv_sec - tv_s.tv_sec) + (tv_e.tv_usec - tv_s.tv_usec) * 1.0 TO(S);

		m_e = lib_memory_total();
		t_e = lib_memory_time();
		if((m_e - m_s) TO(MB))
			printf("****** test end %s %d %fs %lld %lldMB %lldKB******\n", func, num, tv, t_e - t_s, (m_e - m_s) TO(MB), (m_e - m_s) TO(KB) % 1024);
		else if((m_e - m_s) TO(KB))
			printf("****** test end %s %d %fs %lld %lldKB %lldB******\n", func, num, tv, t_e - t_s, (m_e - m_s) TO(KB), (m_e - m_s) % 1024);
		else
			printf("****** test end %s %d %fs %lld %lldB******\n", func, num, tv, t_e - t_s, m_e - m_s);
	}
	pclose(fp);
	dict_free(d);

	gettimeofday(&g_tv_e, NULL);
	g_tv = (g_tv_e.tv_sec - g_tv_s.tv_sec) + (g_tv_e.tv_usec - g_tv_s.tv_usec) * 1.0 TO(S);
	printf("\n****** auto test end %fs ******\n", g_tv);
}

#endif
