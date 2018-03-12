#ifndef CODE_H
#define CODE_H

#include "lib.h"


#define CODE_ERROR_SIZE					-1
#define CODE_ERROR_EMPTY				-2

#define CODE_ANSI_UTF8					0
#define CODE_ANSI_GBK					1

//支持的编码类型
static const int code_ansi[] = {CODE_ANSI_UTF8, CODE_ANSI_GBK};
//默认类型，优先采用默认类型解码
static int code_type = CODE_ANSI_UTF8;

typedef unsigned char ansi;
typedef unsigned short unicode;


//utf8不为空则用utf8指向的空间，为空则重新分配空间，若提供size则使用size大小，无提供则重新获取
#define encode_utf8(...) __VA_FUNC__(encode_utf8, __VA_ARGS__)
inline ansi *encode_utf8_1(unicode *code);
inline ansi *encode_utf8_2(unicode *code, ansi *utf8);
ansi *encode_utf8_3(unicode *code, ansi *utf8, int size);

#define decode_utf8(...) __VA_FUNC__(decode_utf8, __VA_ARGS__)
inline unicode *decode_utf8_1(ansi *utf8);
inline unicode *decode_utf8_2(ansi *utf8, unicode *code);
unicode *decode_utf8_3(ansi *utf8, unicode *code, int size);

#define encode_gbk(...) __VA_FUNC__(encode_gbk, __VA_ARGS__)
inline ansi *encode_gbk_1(unicode *code);
inline ansi *encode_gbk_2(unicode *code, ansi *gbk);
ansi *encode_gbk_3(unicode *code, ansi *gbk, int size);

#define decode_gbk(...) __VA_FUNC__(decode_gbk, __VA_ARGS__)
inline unicode *decode_gbk_1(ansi *gbk);
inline unicode *decode_gbk_2(ansi *gbk, unicode *code);
unicode *decode_gbk_3(ansi *gbk, unicode *code, int size);

//utf8/gbk -> unicode
ansi *encode(unicode *u);

//unicode -> utf8
unicode *decode(ansi *a);

//utf8/gbk -> utf8
ansi *code(ansi *a);


#endif
