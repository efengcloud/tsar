/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */
#ifndef _OUTPUT_NAGIOS_H
#define _OUTPUT_NAGIOS_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
/*
 * output data to db and nagios
 */

void output_db();
void output_nagios();
#endif
