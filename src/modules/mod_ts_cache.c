#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "tsar.h"

/*
 * Structure for TS information
 */
struct stats_ts_cache {
        //ts cache
        unsigned long long total_hit;
        unsigned long long total_miss;
        unsigned long long ssd_hit;
        unsigned long long ram_hit;
        unsigned long long sas_hit;
        unsigned long long user_resp;
        unsigned long long os_resp;
        unsigned long long total_band;
};
//return value type
const static short int TS_REC_INT = 0;
const static short int TS_REC_COUNTER = 0;
const static short int TS_REC_FLOAT = 2;
const static short int TS_REC_STRING = 3;
//command type
const static short int TS_RECORD_GET = 3;
//records
const static int LINE_1024 = 1024;
const static int LINE_4096 = 4096;
const static char *RECORDS_NAME[]= {
        //ts cache
        "proxy.process.cache.read.success",
        "proxy.process.cache.read.failure",
        "proxy.process.cache.ssd.read.success",
        "proxy.process.cache.ram.read.success",
        "proxy.process.cache.sas.read.success",
        "proxy.node.http.user_agent_total_response_bytes",
        "proxy.node.http.origin_server_total_response_bytes"
};
//socket patch
const static char *sock_path = "/var/run/trafficserver/mgmtapisocket";
//const static char *sock_path = "/usr/local/var/trafficserver/mgmtapisocket";

static char *ts_cache_usage = "    --ts_cache          trafficserver cache statistics";

static struct mod_info ts_cache_info[] = {
  {"   hit", DETAIL_BIT, 0, STATS_NULL},
  {"  miss", DETAIL_BIT, 0, STATS_NULL},
  {"   ssd", DETAIL_BIT, 0, STATS_NULL},
  {"   ram", DETAIL_BIT, 0, STATS_NULL},
  {"   sas", DETAIL_BIT, 0, STATS_NULL},
  {"  ur_b", HIDE_BIT, 0, STATS_NULL},
  {"  or_b", HIDE_BIT, 0, STATS_NULL},
  {"  band", DETAIL_BIT, 0, STATS_NULL}
};

void set_ts_cache_record(struct module *mod, double st_array[],
                             U_64 pre_array[], U_64 cur_array[], int inter)
{
  int i = 0;
  for (i = 0; i < 8; ++i) {
    st_array[i] = 0;
  }
  if ((cur_array[0] + cur_array[1]) > (pre_array[0] + pre_array[1])) {
    for (i = 0; i < 5; ++i) {
      if (cur_array[i] >= pre_array[i]) {
        st_array[i] = (cur_array[i] - pre_array[i]) * 100.0 / (cur_array[0] + cur_array[1] - pre_array[0] - pre_array[1]);
      }
    }
  }
  if (cur_array[5] > pre_array[5] && cur_array[6] >= pre_array[6] && (cur_array[5] - pre_array[5]) >= (cur_array[6] - pre_array[6])) {
    st_array[5] = cur_array[5] - pre_array[5];
    st_array[6] = cur_array[6] - pre_array[6];
    st_array[7] = 100.0 * (1 - st_array[6] * 1.0 / st_array[5]);
  }
}

void read_ts_cache_stats(struct module *mod)
{
  int fd = -1;
  struct sockaddr_un un;
  struct stats_ts_cache st_ts;
  int pos;
  char buf[LINE_4096];
  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    goto done;
  }
  bzero(&st_ts, sizeof(st_ts));
  bzero(&un, sizeof(un));
  un.sun_family = AF_UNIX;
  strcpy(un.sun_path, sock_path);
  if (connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
    goto done;
  }

  int record_len = sizeof(RECORDS_NAME)/sizeof(RECORDS_NAME[0]);
  int i;
  const char *info;
  for ( i = 0; i < record_len; ++i) {
    info = RECORDS_NAME[i];
    long int info_len = strlen(info);
    short int command = TS_RECORD_GET;
    char write_buf[LINE_1024];
    *((short int *)&write_buf[0]) = command;
    *((long int *)&write_buf[2]) = info_len;
    strcpy(write_buf+6, info);
    write(fd, write_buf, 2+4+strlen(info));

    short int ret_status;
    long int ret_info_len;
    short int ret_type;
    long ret_val;
    int read_len = read(fd, buf, LINE_1024);
    if (read_len != -1) {
      ret_status = *((short int *)&buf[0]);
      ret_info_len = *((long int *)&buf[2]);
      ret_type = *((short int *)&buf[6]);
    }
    if (0 == ret_status) {
      if (ret_type < 2) {
	ret_val= *((long int *)&buf[8]);
      } else if (2 == ret_type) {
	float ret_val_float = *((float *)&buf[8]);
        ret_val_float *= 100;
        ret_val = (unsigned long long)ret_val_float;
      } else {
        goto done;
      }
    }
    ((unsigned long long *)&st_ts)[i] = ret_val;
  }
done:
  if (-1 != fd)
    close(fd);
  pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld",
                st_ts.total_hit,
                st_ts.total_miss,
                st_ts.ssd_hit,
                st_ts.ram_hit,
                st_ts.sas_hit,
                st_ts.user_resp,
                st_ts.os_resp,
                st_ts.total_band
                );
  buf[pos] = '\0';
  set_mod_record(mod, buf);
}

void mod_register(struct module *mod)
{
  register_mod_fileds(mod, "--ts_cache", ts_cache_usage, ts_cache_info, 8, read_ts_cache_stats, set_ts_cache_record);
}
