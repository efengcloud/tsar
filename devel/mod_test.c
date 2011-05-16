/*
 * (C) 2010-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License 2.0
 *
 */

#include "tsar.h"

/*
 * Structure for test infomation.
 */
struct stats_test {
        unsigned long long value_1;
        unsigned long long value_2;
        unsigned long long value_3;
};

#define STATS_TEST_SIZE (sizeof(struct stats_test))

static char *test_usage = "    --test               test information";


static void read_test_stats(struct module *mod)
{
        char buf[256];
	memset(buf, 0, sizeof(buf));
        struct stats_test st_test;
	memset(&st_test, 0, sizeof(struct stats_test));
	
	st_test.value_1 = 1;
	st_test.value_2 = 1;
	st_test.value_3 = 1;
	
        int pos = sprintf(buf, "%llu,%llu,%llu",
                          /* the store order is not same as read procedure */
                          st_test.value_1,
                          st_test.value_2,
                          st_test.value_3);

        buf[pos] = '\0';
	set_mod_record(mod, buf);
        return;
}

static struct mod_info test_info[] = {
        {"value1", SUMMARY_BIT,  0,  STATS_NULL},
        {"value2", DETAIL_BIT,  0,  STATS_NULL},
        {"value3", DETAIL_BIT,  0,  STATS_NULL}
};

static void set_cpu_record(struct module *mod, double st_array[],
                           U_64 pre_array[], U_64 cur_array[], int inter)
{
	int i;
        /* set st record */
        for (i = 0; i < mod->n_col; i++) {
                st_array[i] = cur_array[i];
        }
}

void mod_register(struct module *mod)
{
	register_mod_fileds(mod, "--test", test_usage, test_info, 3, read_test_stats, set_cpu_record);
}

