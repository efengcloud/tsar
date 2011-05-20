/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */

#include "tsar.h"


void output_file()
{
	struct 	module *mod;
	FILE	*fp = NULL;
	int	i, ret = 0;
	char	line[LEN_4096] = {0};
	char	detail[LEN_1024] = {0};
	char	s_time[LEN_256] = {0};

	if (!(fp = fopen(conf.output_file_path, "a+"))) {
		if (!(fp = fopen(conf.output_file_path, "w")))
			do_debug(LOG_FATAL, "output_file: can't create data file = %s  err=%d\n", conf.output_file_path, errno);
	}

	sprintf(s_time, "%ld", statis.cur_time);
	strcat(line, s_time);

	for (i = 0; i < statis.total_mod_num; i++) {
		mod = &mods[i];
		if (mod->enable && strlen(mod->record)) {
			/* save collect data to output_file */
			sprintf(detail, "%s%s%s%s", SECTION_SPLIT, mod->opt_line, STRING_SPLIT, mod->record);
			strcat(line, detail);
			ret = 1;
		}
	}
	strcat(line, "\n");

	if (ret) {
		if(fputs(line, fp) < 0)
			 do_debug(LOG_WARN, "write line error\n");
	}
	fclose(fp);
	if(chmod(conf.output_file_path, 0666) < 0 )
		do_debug(LOG_WARN, "chmod file %s error\n",conf.output_file_path);
}

