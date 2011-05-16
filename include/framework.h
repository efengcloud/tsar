/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */
#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H
/*
 * tsar framwork
 */

#include "define.h"
struct mod_info {
	char	hdr[LEN_128];
	int	summary_bit;	/* bit set indefi summary */
	int	merge_mode;
	int	stats_opt;
};

struct module
{
	char	name[LEN_32];
	char	opt_line[LEN_32];
	char	record[LEN_1024];
	char    usage[LEN_256];

	struct  mod_info *info;
	void    *lib;
	int	enable;

	/* private data used by framework*/
	int	n_item;
	int	n_col;
	long	n_record;

	int	pre_flag:4;
	int	st_flag:4;

	U_64	*pre_array;
	U_64	*cur_array;
	double	*st_array;
	double	*max_array;
	double	*mean_array;
	double	*min_array;
	
	/* callback function of module */
	void (*data_collect) (struct module *);
	void (*set_st_record) (struct module *mod, double *, U_64 *, U_64 *, int );

	/* mod manage */	
	void (*mod_register) (struct module *);
};


void register_mod_fileds(struct module *mod, char *opt, char *usage, 
				struct mod_info *info, int n_col, void *data_collect, void *set_st_record);
void set_mod_record(struct module *mod, char *record);
void init_module_fields();
void reload_modules(char *s_mod);
void load_modules();
void free_modules();
void collect_record();
void read_line_to_module_record(char *line);
int  collect_record_stat();
void disable_col_zero();
#endif
