#ifndef _TSAR_H
#define _TSAR_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>

#include "framework.h"
#include "debug.h"
#include "config.h"

#include "output_file.h"
#include "output_print.h"
#include "output_nagios.h"
#include "common.h"

struct statistic
{
	int	total_mod_num;
	time_t	cur_time;
};


extern struct configure conf;
extern struct module	mods[MAX_MOD_NUM];
extern struct statistic statis;

#endif
