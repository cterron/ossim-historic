#!/bin/sh
cd /usr/share/ossim/scripts/sem/ && perl /usr/share/ossim/scripts/sem/generate_stats.pl /var/ossim/logs/
cd /usr/share/ossim/scripts/sem/ && sh /usr/share/ossim/scripts/sem/forensic_stats_last_hour.sh
cd /usr/share/ossim/scripts/sem/ && sh /usr/share/ossim/scripts/sem/update_db.sh
