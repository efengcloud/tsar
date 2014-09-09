#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "tsar.h"

const static short int TS_RECORD_GET = 3;
const static int LINE_1024 = 1024;
const static int LINE_4096 = 4096;
const static char *RECORDS_NAME[]= {
  "proxy.process.cluster.ping_total_count",
  "proxy.process.cluster.ping_time_used",
  "proxy.process.cluster.io.send_msg_count",
  "proxy.process.cluster.io.send_bytes",
  "proxy.process.cluster.io.call_writev_count",
  "proxy.process.cluster.io.recv_msg_count",
  "proxy.process.cluster.io.recv_bytes",
  "proxy.process.cluster.io.call_read_count",
  "proxy.process.cluster.io.send_delayed_time",
  "proxy.process.cluster.io.push_msg_count",
  "proxy.process.cluster.io.push_msg_bytes",
  "proxy.process.cluster.ping_success_count"
};
const static char *sock_path = "/var/run/trafficserver/mgmtapisocket";

/*
 * Structure for TS information
 */
struct stats_ts_clusterio {
  //ts 
  unsigned long long ping_total_count;
  unsigned long long ping_time_used;
  unsigned long long send_msg_count;
  unsigned long long send_bytes;
  unsigned long long call_writev_count;
  unsigned long long recv_msg_count;
  unsigned long long recv_bytes;
  unsigned long long call_read_count;
  unsigned long long send_delayed_time;
  unsigned long long push_msg_count;
  unsigned long long push_msg_bytes;
  unsigned long long ping_success_count;
};

static char *ts_clusterio_usage = "    --ts_clusterio         trafficserver cluster io performance stats";

static struct mod_info ts_clusterio_info[] = {
  {"pingtc",   HIDE_BIT, 0, STATS_NULL},  // 0
  {" pingt", DETAIL_BIT, 0, STATS_NULL},  // 1
  {"  send",   HIDE_BIT, 0, STATS_NULL},
  {" sendb", DETAIL_BIT, 0, STATS_NULL},  // 3
  {" write", DETAIL_BIT, 0, STATS_NULL},
  {"  recv",   HIDE_BIT, 0, STATS_NULL},  // 5
  {" recvb", DETAIL_BIT, 0, STATS_NULL},
  {"  read", DETAIL_BIT, 0, STATS_NULL},  // 7
  {"snddel", DETAIL_BIT, 0, STATS_NULL},
  {"  push",   HIDE_BIT, 0, STATS_NULL},
  {" pushb", DETAIL_BIT, 0, STATS_NULL},  // 10
  {"pingsc",   HIDE_BIT, 0, STATS_NULL},
};

void set_ts_clusterio_record(struct module *mod, double st_array[],
                             U_64 pre_array[], U_64 cur_array[], int inter)
{
  // 
  int i;
  for (i = 0; i < 12; i++) {
    if (cur_array[i] >= pre_array[i]) {
      st_array[i] = (cur_array[i] - pre_array[i]) * 1.0 / inter;
    } else {
      st_array[i] = 0;
    }
  }
  if (st_array[0] > 0) { st_array[1] = st_array[1] / st_array[0]; } else { st_array[1] = 0; }
  if (st_array[2] > 0) { st_array[3] = st_array[3] / st_array[2]; } else { st_array[3] = 0; }
  if (st_array[5] > 0) { st_array[6] = st_array[6] / st_array[5]; } else { st_array[6] = 0; }
  if (st_array[2] > 0) { st_array[8] = st_array[8] / st_array[2]; } else { st_array[8] = 0; }
  if (st_array[9] > 0) { st_array[10] = st_array[10] / st_array[9]; } else { st_array[10] = 0; }
}

void read_ts_clusterio_stats(struct module *mod)
{
  int fd = -1;
  struct sockaddr_un un;
  struct stats_ts_clusterio st_ts;
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
  pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld",
      st_ts.ping_total_count,
      st_ts.ping_success_count,
      st_ts.ping_time_used,
      st_ts.send_msg_count,
      st_ts.send_bytes,
      st_ts.call_writev_count,
      st_ts.recv_msg_count,
      st_ts.recv_bytes,
      st_ts.call_read_count,
      st_ts.send_delayed_time,
      st_ts.push_msg_count,
      st_ts.push_msg_bytes
      );
  buf[pos] = '\0';
  set_mod_record(mod, buf);
}

void mod_register(struct module *mod)
{
  register_mod_fileds(mod, "--ts_clusterio", ts_clusterio_usage, ts_clusterio_info, 12, read_ts_clusterio_stats, set_ts_clusterio_record);
}
