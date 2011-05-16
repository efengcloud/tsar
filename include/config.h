/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */
#ifndef _CONFIG_H
#define _CONFIG_H

#include "define.h"

/*
 * tsar config file interface
 */

struct configure
{
	/* from arg */
	int	running_mode;	/* running mode */
	char	config_file[LEN_128];
	int	debug_level;

	char	output_interface[LEN_128];  /* which interface will enable*/
	
	/* output print */
	char	output_print_mod[LEN_512];  /* which mod will print throught argv */
	char	output_stdio_mod[LEN_512];  /* which mod will print throuhth conf file */
	char	output_nagios_mod[LEN_512];  /* which mod will output to nagios */
	int	print_interval;		 /* how many seconds will escape every print interval */
	int	print_nline_interval;	 /* how many lines will skip every print interval */
	int	print_mode;		/* data type will print: summary or detail */
	int	print_merge;		/* mult items is merge */
	int	print_ndays;		 /* how many line will print every time. default:10 */
	int	print_tail;
	int	print_file_number;	/* which tsar.data file used*/
	
	/* output db */
	char	output_db_mod[LEN_512];  /* which mod will output */
	char	output_db_addr[LEN_512]; /* db addr */
	
	/* output nagios */
	char	server_addr[LEN_512];
	int	*server_port;
	int	*cycle_time;
	char	send_nsca_cmd[LEN_512];
	char	send_nsca_conf[LEN_512];
	
	char    check_name[MAX_MOD_NUM][LEN_32];
	float	wmin[MAX_MOD_NUM];
	float	wmax[MAX_MOD_NUM];
	float	cmin[MAX_MOD_NUM];
	float	cmax[MAX_MOD_NUM];
	int	mod_num;
	/* output file */
	char	output_file_path[LEN_128];
};


void parse_config_file(const char *file_name);
void get_include_conf();
void get_threshold();
void get_nagios_conf();
#endif
