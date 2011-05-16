/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */

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
