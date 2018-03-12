#include "logging.h"


static dict *d = NULL;
static FILE *stream = NULL;

__attribute__((constructor)) void logging_init(char *filename)
{
	thread t;
	thread_run(&t, config_auto, NULL);
	config_update("config/logging.conf");
	if(d == NULL)
		d = dict_new();

	if(filename)
	{
		if(stream && stream != stdout)
			fclose(stream);
		stream = fopen(filename, "rb+");
	}
	else
		stream = stdout;
}

__attribute__((destructor)) void logging_destroy()
{
	if(stream && stream != stdout)
		fclose(stream);
}

//若有使用的名称，必须设置，否则使用该名称的筛选将无效
void logging_set(char *name, int type)
{
	dict_set(d, name, &type);
}

/*
logging("message", LOGGING_INFO, "user", "admin", "uid", 1);
*/
void _logging(char *message, const char *file, const char *func, const int line, int level, ...)
{
	int _check = TRUE;
	int _level;
	config_with("config/logging.conf", {
		va_list ap;
		va_start(ap, level);
		while(1)
		{
			char *key = va_arg(ap, char *);
			if(key == NULL)
				break;

			int *type = dict_get(d, key);
			if(type == NULL)
				break;

			if(*type == 's')
			{
				char *val = va_arg(ap, char *);
				if(val == NULL)
					break;

				char *s = config_read_string("filter", key, NULL);
				if(s == NULL)
					continue;

				if(strcmp(val, s))
				{
					_check = FALSE;
					break;
				}
			}
			else if(*type == 'd')
			{
				int val = va_arg(ap, int);
				if(val == 0)
					break;

				int n = config_read_int("filter", key, 0);
				if(n == 0)
					continue;

				if(val != n)
				{
					_check = FALSE;
					break;
				}
			}
		}
		va_end(ap);

		if(_check == TRUE)
			_level = config_read_int("filter", "level", LOGGING_DEBUG);
		else
			_level = config_read_int("global", "level", LOGGING_ERROR);
	});

	if(level >= _level)
	{
		char datetime[128];
		tools_datetime(datetime);

		fprintf(stdout, "%s %s -> %s [%d] %s : %s\n", datetime, file, func, line, LOGGING_LEVEL[level], message);
		fflush(stdout);
	}
}


#ifdef TEST

#include "test.h"

void logging_test()
{
	int i;
	logging_set("user", 's');
	for(i = 0; i < 3; i++)
	{
		logging("abc", LOGGING_WARNING, "user", "abc");
		tools_sleep(1 S);
	}
}

#endif
