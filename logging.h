#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>
#include "config.h"
#include "dict.h"
#include "tools.h"

#define LOGGING_NOTSET									0
#define LOGGING_DEBUG									1
#define LOGGING_INFO 									2
#define LOGGING_WARNING								3
#define LOGGING_ERROR									4
#define LOGGING_CRITICAL								5

static const char *LOGGING_LEVEL[] =
{
	"NOTSET",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"CRITICAL"
};

void logging_init(char *filename);

#define logging(message, level, ...) _logging(message, __FILE__, __FUNCTION__, __LINE__, level, ##__VA_ARGS__, NULL)

void _logging(char *message, const char *file, const char *func, const int line, int level, ...);


#endif
