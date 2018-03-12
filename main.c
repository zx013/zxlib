#include "dict.h"
#include "test.h"


void dict_9_test()
{
	dict *d;
	char *s;

	d = dict_new();
	dict_set(d, 1, "中华");

	printf("%d\n", dict_json_size(d));
	s = dict_json_dump(d);
	printf("%s, %d\n", s, strlen(s));

	lib_free(s);
	dict_free(d);
}


int main(int argc, char *argv[])
{
	/*
	dict *d;

	d = dict_json_load(argv[2]);
	printf("%s\n", dict_json_dump(d));
	printf("%02x%02x\n", argv[2][0], argv[2][1]);
	*/
	return 0;
}
