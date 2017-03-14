#!/bin/bash
FILE="/usr/share/ossim/uploads/otx-debug.tar.gz" 
[[ ! -f $FILE ]] || exit 0 
fgrep "[ERROR]: OTX: It wasn't possible to retrieve the Pulse detail: Cannot find the Pulse ID" /var/log/alienvault/api/api.log >/dev/null || exit 0 
tar -P --ignore-failed-read -czf $FILE /var/log/alienvault/*.log /var/log/alienvault/*/*.log /var/log/alienvault/*/*/*.log /var/log/ossim /var/log/redis /var/log/syslog /var/log/user.log /var/log/monit.log /var/log/messages /var/log/kern.log /var/log/lastlog /etc/redis /var/log/alienvault/rhythm
