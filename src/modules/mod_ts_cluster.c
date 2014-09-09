#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "tsar.h"

//return value type
//const static short int TS_REC_INT = 0;
//const static short int TS_REC_COUNTER = 0;
//const static short int TS_REC_FLOAT = 2;
//const static short int TS_REC_STRING = 3;
//command type
const static short int TS_RECORD_GET = 3;
//records
const static int LINE_1024 = 1024;
const static int LINE_4096 = 4096;
const static char *RECORDS_NAME[]= {
  "proxy.process.cluster.write_lock_misses",
  "proxy.process.cluster.remote_op_read_timeouts",
  "proxy.process.cluster.remote_op_write_timeouts",
  "proxy.process.cluster.remote_op_reply_timeouts",
  "proxy.process.cluster.open_delay_timeouts",
  "proxy.process.cluster.cache_callback_time",
  "proxy.process.cluster.rmt_cache_callback_time",
  "proxy.process.cluster.setdata_no_clustervc",
  "proxy.process.cluster.setdata_no_tunnel",
  "proxy.process.cluster.setdata_no_cachevc",
  "proxy.process.cluster.vc_write_stall"
};
//socket patch
const static char *sock_path = "/var/run/trafficserver/mgmtapisocket";
//const static char *sock_path = "/usr/local/var/trafficserver/mgmtapisocket";

/*
 * Structure for TS information
 */
struct stats_ts_cluster {
  //ts conn
  unsigned long long write_lock_misses;
  unsigned long long remote_op_read_timeouts;
  unsigned long long remote_op_write_timeouts;
  unsigned long long remote_op_reply_timeouts;
  unsigned long long open_delay_timeouts;
  unsigned long long cache_callback_time;
  unsigned long long rmt_cache_callback_time;
  unsigned long long setdata_no_clustervc;
  unsigned long long setdata_no_tunnel;
  unsigned long long setdata_no_cachevc;
  unsigned long long vc_wirte_stall;
};

static char *ts_cluster_usage = "    --ts_cluster        trafficserver statistics on cluster internals";

static struct mod_info ts_cluster_info[] = {
  {"   rlm", DETAIL_BIT, 0, STATS_NULL},
  {"  rort", DETAIL_BIT, 0, STATS_NULL},
  {"  rowt", DETAIL_BIT, 0, STATS_NULL},
  {" roret", DETAIL_BIT, 0, STATS_NULL},
  {"   odt", DETAIL_BIT, 0, STATS_NULL},
  {"   cct", DETAIL_BIT, 0, STATS_NULL},
  {"  rcct", DETAIL_BIT, 0, STATS_NULL},
  {"   snc", DETAIL_BIT, 0, STATS_NULL},
  {"   snt", DETAIL_BIT, 0, STATS_NULL},
  {"   snv", DETAIL_BIT, 0, STATS_NULL},
  {"   vws", DETAIL_BIT, 0, STATS_NULL},
};

void set_ts_cluster_record(struct module *mod, double st_array[],
                             U_64 pre_array[], U_64 cur_array[], int inter)
{
  int i;
  for (i = 0; i < 11; i++) {
    if (cur_array[i] >= pre_array[i]) {
      st_array[i] = (cur_array[i] - pre_array[i]) * 1.0 / inter;
    }
  }
}

void read_ts_cluster_stats(struct module *mod)
{
  int fd = -1;
  struct sockaddr_un un;
  struct stats_ts_cluster st_ts;
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

    short int ret_status = 0;
    short int ret_type = 0;
    long ret_val = 0;
    int read_len = read(fd, buf, LINE_1024);
    if (read_len != -1) {
      ret_status = *((short int *)&buf[0]);
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
  pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld",
      st_ts.write_lock_misses,
      st_ts.remote_op_read_timeouts,
      st_ts.remote_op_write_timeouts,
      st_ts.remote_op_reply_timeouts,
      st_ts.open_delay_timeouts,
      st_ts.cache_callback_time,
      st_ts.rmt_cache_callback_time,
      st_ts.setdata_no_clustervc,
      st_ts.setdata_no_tunnel,
      st_ts.setdata_no_cachevc,
      st_ts.vc_wirte_stall
      );
  buf[pos] = '\0';
  set_mod_record(mod, buf);
}

void mod_register(struct module *mod)
{
  register_mod_fileds(mod, "--ts_cluster", ts_cluster_usage, ts_cluster_info, 11, read_ts_cluster_stats, set_ts_cluster_record);
}
