#
# Regular cron jobs for the instead package
#
0 4	* * *	root	[ -x /usr/bin/instead_maintenance ] && /usr/bin/instead_maintenance
