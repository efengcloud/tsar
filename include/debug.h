/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */
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
