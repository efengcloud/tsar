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
