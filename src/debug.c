#include "tsar.h"

void do_debug(log_level_t level, const char *fmt, ...)
{
	//FIXME
	if (level >= conf.debug_level) {
		va_list	argp;
		time_t	timep;

		time(&timep);
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		fflush(stderr);
		va_end(argp);
	}
	if (level == LOG_FATAL)
		exit(1);
}
