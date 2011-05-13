#ifndef _DEBUG_H
#define _DEBUG_H

typedef enum
{
	LOG_INFO,
	LOG_DEBUG,
	LOG_WARN,
	LOG_ERR,
	LOG_FATAL
} log_level_t;


void do_debug(log_level_t level, const char *fmt, ...);
#endif
