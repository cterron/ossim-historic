#!/bin/bash
/etc/init.d/openvas-manager stop
/etc/init.d/openvas-scanner stop
if [ "$1" == "repair" ]
then
   cd /var/lib/openvas/mgr
   mv tasks.db tasks.db.old
fi
/etc/init.d/openvas-scanner start
while [ `netstat -putan | grep -c openvassd` -eq 0 ]
do
  echo "Waiting 30 seconds to openvas-scanner...";
  sleep 30
done
/etc/init.d/openvas-manager rebuild
/etc/init.d/openvas-manager start
