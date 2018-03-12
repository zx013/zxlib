#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include "logging.h"

#ifndef _WIN32
#define GetLastError()								-1
#endif

//��goto����ʽ���д���������Ϊerror_goto���ɼ�¼�����λ��
#define __error_goto(label, level) \
	{ \
		char message[128] = {0}; \
		int last_error = GetLastError(); \
		if(last_error == -1) \
			sprintf(message, "error in %s, errno is %d", #label, errno); \
		else \
			sprintf(message, "error in %s, errno is %d, last error is %d", #label, errno, last_error); \
		logging(message, level); \
		errno = 0; \
		goto label; \
	}

#define debug_goto(label) __error_goto(label, LOGGING_DEBUG)
#define error_goto(label) __error_goto(label, LOGGING_ERROR)

#ifdef TEST
#define test_goto(label) debug_goto(label)
#else
#define test_goto(label) goto label
#endif

#endif
