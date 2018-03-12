OS_NAME = $(shell uname)

CC = gcc

OO = zx013

ifneq ($(OS_NAME), Linux)
LIBS = lib/windows/libcurl.a lib/windows/libevent.a -lws2_32 -lwldap32
else
LIBS = lib/linux/libcurl.a lib/linux/libevent.a -lpthread -lm -lrt -lldap -lssl -pg
endif

WARN = -Wno-int-to-pointer-cast

DEFS = -Iinclude/curl -Iinclude/event -DCURL_STATICLIB -DTEST

SOURCE = ${wildcard *.c}

#OBJECT=$(SOURCE:.c=.o)
OBJECT=${patsubst %.c, %.o, $(SOURCE)}

all: $(OBJECT)
	$(CC) -o $(OO) $(OBJECT) $(LIBS)

$(OBJECT): %.o:%.c
	$(CC) -c $(WARN) $(DEFS) -o $@ $<

clean :
	rm -rf $(OO) $(OBJECT)