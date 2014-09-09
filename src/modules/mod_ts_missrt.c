#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "tsar.h"

const static short int TS_RECORD_GET = 3;
const static int LINE_1024 = 1024;
const static int LINE_4096 = 4096;
const static char *RECORDS_NAME[]= {
  "proxy.process.http.transaction_counts.hit_revalidated",
  "proxy.process.http.transaction_totaltime.hit_revalidated",
  "proxy.process.http.transaction_counts.miss_cold",
  "proxy.process.http.transaction_totaltime.miss_cold",
  "proxy.process.http.transaction_counts.miss_not_cacheable",
  "proxy.process.http.transaction_totaltime.miss_not_cacheable",
  "proxy.process.http.transaction_counts.miss_changed",
  "proxy.process.http.transaction_totaltime.miss_changed"
};
const static char *sock_path = "/var/run/trafficserver/mgmtapisocket";

/*
 * Structure for TS information
 */
struct stats_ts_missrt {
  //ts 
  unsigned long long hit_revalidated;
  unsigned long long hit_revalidated_time;
  unsigned long long miss_cold;
  unsigned long long miss_cold_time;
  unsigned long long miss_not_cacheable;
  unsigned long long miss_not_cacheable_time;
  unsigned long long miss_changed;
  unsigned long long miss_changed_time;
};

static char *ts_missrt_usage = "    --ts_missrt         trafficserver miss qps and rt stats";

static struct mod_info ts_missrt_info[] = {
  {"revqps", DETAIL_BIT, 0, STATS_NULL},
  {" revrt", DETAIL_BIT, 0, STATS_NULL},
  {"coldps", DETAIL_BIT, 0, STATS_NULL},
  {"coldrt", DETAIL_BIT, 0, STATS_NULL},
  {"nocqps", DETAIL_BIT, 0, STATS_NULL},
  {" nocrt", DETAIL_BIT, 0, STATS_NULL},
  {"chgqps", DETAIL_BIT, 0, STATS_NULL},
  {" chgrt", DETAIL_BIT, 0, STATS_NULL},
};

void set_ts_missrt_record(struct module *mod, double st_array[],
                             U_64 pre_array[], U_64 cur_array[], int inter)
{
  int i;
  for (i = 0; i < 8; i++) {
    if (cur_array[i] >= pre_array[i]) {
      st_array[i] = (cur_array[i] - pre_array[i]) * 1.0 / inter;
    } else {
      st_array[i] = 0;
    }
    i++;
    if ((cur_array[i] >= pre_array[i]) && (st_array[i-1] != 0)) {
      st_array[i] = (cur_array[i] - pre_array[i]) * 1000.0 / inter / st_array[i-1];
    } else {
      st_array[i] = 0;
    }
  }
}

void read_ts_missrt_stats(struct module *mod)
{
  int fd = -1;
  struct sockaddr_un un;
  struct stats_ts_missrt st_ts;
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
  pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld",
      st_ts.hit_revalidated,
      st_ts.hit_revalidated_time,
      st_ts.miss_cold,
      st_ts.miss_cold_time,
      st_ts.miss_not_cacheable,
      st_ts.miss_not_cacheable_time,
      st_ts.miss_changed,
      st_ts.miss_changed_time
      );
  buf[pos] = '\0';
  set_mod_record(mod, buf);
}

void mod_register(struct module *mod)
{
  register_mod_fileds(mod, "--ts_missrt", ts_missrt_usage, ts_missrt_info, 8, read_ts_missrt_stats, set_ts_missrt_record);
}
