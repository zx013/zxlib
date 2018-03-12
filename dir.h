#ifndef DIR_H
#define DIR_H

#include <dirent.h>
#include "lib.h"
#include "list.h"
#include "tools.h"
#include "file.h"

typedef struct tools_filenode
{
	char *name;
	char *path;
	__int64 size;
} tools_filenode;

__int64 tools_filesize(char *filepath);

void tools_listdir(char *filepath, list_t *list);

void tools_listdir_clear(list_t *list);


#endif
