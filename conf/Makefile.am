AUTOMAKE_OPTIONS = foreign

confdir=/etc/tsar
conf_DATA= nagios.conf tsar.conf
confddir=/etc/tsar/conf.d
confd_DATA=
rotatedir=/etc/logrotate.d
mandir=${datadir}/man

rotate_DATA=tsar.logrotate

crondir=/etc/cron.d
cron_DATA=tsar.cron

EXTRA_DIST=$(conf_DATA) $(rotate_DATA) tsar.8 $(cron_DATA)

man_MANS=tsar.8
