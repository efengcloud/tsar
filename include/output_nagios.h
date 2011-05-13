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
