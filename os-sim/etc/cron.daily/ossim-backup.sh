#!/bin/bash

CONF=/etc/ossim/framework/ossim.conf
[ -f "$CONF" ] || exit

HOST=`grep ^ossim_host $CONF | cut -d= -f2`
USER=`grep ^ossim_user $CONF | cut -d= -f2`
PASS=`grep ^ossim_pass $CONF | cut -d= -f2`
BASE=`grep ^ossim_base $CONF | cut -d= -f2`

BACKUP=/var/lib/ossim/backup/ossim-backup_`date '+%F'`.sql.gz

mysqldump -h $HOST -u $USER -p$PASS $BASE | gzip -9c > $BACKUP


