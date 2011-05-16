/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */
#ifndef _OUTPUT_FILE_H
#define _OUTPUT_FILE_H
/*
 * output data to file
 */

struct buffer {
	char	*data;
	int	len;
};

struct file_header
{
	int	version;
	time_t	t_start;	
};

void output_file();
#endif
