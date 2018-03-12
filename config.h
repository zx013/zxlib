#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>
#include "head.h"
#include "lib.h"
#include "lock.h"
#include "dict.h"
#include "thread.h"
#include "tools.h"

#define CONFIG_LINE			1024
#define CONFIG_TITLE		1024

int config_update(char *filename);

int config_auto();

inline char *config_get();

inline void config_set(char *filename);

#define config_with(filename, block) \
{ \
	char *restore = config_get(); \
	config_set(filename); \
	block; \
	config_set(restore); \
}

inline char *config_find(char *option, char *value);

#define config_read_string(...) __VA_FUNC__(config_read_string, __VA_ARGS__)
#define config_read_string_2(title, option) config_read_string_3(title, option, NULL)
inline char *config_read_string_3(char *title, char *option, char *replace);

#define config_read_int(...) __VA_FUNC__(config_read_int, __VA_ARGS__)
#define config_read_int_2(title, option) config_read_int_3(title, option, 0)
inline int config_read_int_3(char *title, char *option, int replace);

#endif