#include "config.h"

static dict *d = NULL;
static char lock = 0;
static __thread char *choice = NULL;

void adjust_line(char *line)
{
	char *s = line;
	do
	{
		while(*s == ' ' || *s == '\r' || *s == '\n')
			s++;
	} while(*line++ = *s++);
}

int find_line(char *line, char c)
{
	char *s = line;
	while(*s && *s != c)
		s++;
	if(!*s)
		return NONE;
	return s - line;
}

/*
{"filename1": {"time": 1, "info": {"name": "abc", "num": 1}}}
*/
int config_update(char *filename)
{
	char line[CONFIG_LINE];
	char title[CONFIG_TITLE];
	int len, index;
	FILE *fp;

	lock(lock);

	if(d == NULL)
		d = dict_new();

	fp = fopen(filename, "rb");
	if(fp == NULL)
	{
		unlock(lock);
		return FALSE;
	}

	struct stat st;
	tools_stat(filename, &st);

	dict_set(d, filename, "time", st.st_mtime);

	memset(line, 0, CONFIG_LINE);
	while(fgets(line, CONFIG_LINE, fp))
	{
		adjust_line(line);
		if(*line == 0 || *line == '#')
			continue;
		len = strlen(line);
		if(line[0] == '[' && line[len - 1] == ']')
		{
			line[0] = line[len - 1] = 0;
			strcpy(title, line + 1);

			if(dict_get(d, filename, "info", title) == NULL)
				dict_set(d, filename, "info", lib_strdup(title), NULL);
		}
		else
		{
			index = find_line(line, '=');
			if(index == NONE)
				continue;
			line[index] = 0;
			 
			if(dict_get(d, filename, "info", title, line) == NULL)
				dict_set(d, filename, "info", title, lib_strdup(line), lib_strdup(line + index + 1));
			else
				dict_set(d, filename, "info", title, line, lib_strdup(line + index + 1));
		}
	}
	fclose(fp);

	choice = filename;

	unlock(lock);
	return TRUE;
}

//只有修改或增加会生效，删除不会生效
int config_auto()
{
	struct stat st;
	while(1)
	{
		if(d)
		{
			dict_each(d, lambda((dict *key, dict *data), {
				tools_stat(dict_data(key), &st);
				if((time_t)dict_get(data, "time") != st.st_mtime)
					config_update(dict_data(key));
			}));
		}
		//tools_yield();
		tools_sleep(1 S);
	}
	thread_exit();
}

inline char *config_get()
{
	return choice;
}

inline void config_set(char *filename)
{
	choice = filename;
}

//查找option的值为value的项
inline char *config_find(char *option, char *value)
{
	char *title = NULL;
	dict_each(dict_get_node(d, choice, "info"), lambda((dict *key, dict *data), {
		char *h;
		if(!(h = dict_get(data, option)))
			return TRUE;
		if(!strcmp(h, value))
		{
			title = dict_data(key);
			return FALSE;
		}
	}));
	return title;
}


inline char *config_read(char *title, char *option)
{
	return dict_get(d, choice, "info", title, option);
}

inline char *config_read_string_3(char *title, char *option, char *replace)
{
	char *string;
	string = config_read(title, option);
	if(string == NULL)
		return replace;
	return string;
}

inline int config_read_int_3(char *title, char *option, int replace)
{
	char *string;
	string = config_read(title, option);
	if(string == NULL)
		return replace;
	return atoi(string);
}


#ifdef TEST

#include "test.h"

/*
整体测试时在该函数出现段错误，单独测试时正常。开始必现，后来该问题消失，原因不明。
*/
void config_test()
{
	test_assert(config_update("test/config/test1.conf"), TRUE);
	test_assert(config_update("test/config/test2.conf"), TRUE);
	//config_choice(filename); 默认选择最后init的文件
	config_with("test/config/test1.conf", {
		test_assert(config_read_string("test10", "name", ""), "name10", 's');
		test_assert(config_read_string("test11", "name", ""), "name11", 's');
		test_assert(config_read_string("test12", "name", ""), "name12", 's');
	});
	config_with("test/config/test2.conf", {
		test_assert(config_read_string("test10", "name", "t1"), "t1", 's');
		test_assert(config_read_string("test10", "name", "t2"), "t2", 's');
		test_assert(config_read_string("test10", "name", "t3"), "t3", 's');

		test_assert(config_read_int("test20", "num", 0), 0);
		test_assert(config_read_int("test21", "num", 0), 1);
		test_assert(config_read_int("test22", "num", 0), 2);

		test_assert(config_find("num", "0"), "test20", 's');
		test_assert(config_find("num", "1"), "test21", 's');
		test_assert(config_find("num", "2"), "test22", 's');
	});
}

#endif
