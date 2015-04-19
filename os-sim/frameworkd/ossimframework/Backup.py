# 
# Replaces and improves the 'etc/cron.daily/ossim-backup' script
#
#  -  ossim, phpgacl and snort database backups using mysqldump
#  -  backup files are compressed with gzip and stored in conf[backup_dir]
#  -  if purge=True, outdated files (conf[backup_day]) are removed
#

import threading, os, gzip, time, glob

import Const

from OssimDB import OssimDB
from OssimConf import OssimConf

# TODO: global CONF and DB object.
# They are used in almost all threads
_CONF  = OssimConf(Const.CONFIG_FILE)
_DB    = OssimDB()
_DB.connect(_CONF['ossim_host'],
            _CONF['ossim_base'],
            _CONF['ossim_user'],
            _CONF['ossim_pass'])

DATABASES = [ 'ossim', 'phpgacl', 'snort' ]

class Backup(threading.Thread):

    def __init__(self, purge=True):
        self._databases = {}
        for db in DATABASES:
            self._databases[db] = {}
        self.backup_dir = _CONF['backup_dir']
        self.purge = purge
        threading.Thread.__init__(self)

    def get_database_properties(self, db):

        properties = {}

        if not db:
            return properties

        if _CONF[db + '_type'] == 'mysql':
            for key in ['base', 'host', 'user', 'pass']:
                properties[key] = _CONF[db + '_' + key]

        return properties

    def compress_backup_file(self, backup_file):

        try:
            fo = open(backup_file)
        except IOError, e:
            print __name__, ": Error opening backup file: %s" % (e)
            return

        fd = gzip.GzipFile(backup_file + ".gz", 'wb')
        fd.write(fo.read())
        fo.close()
        fd.close()
        os.unlink(backup_file)

        print __name__, \
            ": Created backup file", backup_file + ".gz [%s bytes]" %\
                (os.path.getsize(backup_file + ".gz"))


    def purge_outdated_files(self):

        # purge files matching (ossim|phpgacl|snort.*?\sql.gz)
        def file_ok_to_purge(file):

            for base in DATABASES:
                if file.startswith(base):
                    return True

            return False

        # cleanup old files
        purge_date = time.time() - int(_CONF['backup_day'])*60*60*24
        for file in glob.glob(os.path.join(self.backup_dir, '*.sql.gz')):
            if file_ok_to_purge(file):
                if os.path.getctime(os.path.join(self.backup_dir, file)) < \
                   purge_date:
                    print __name__, \
                        ": Removing outdated file %s.." % \
                        (os.path.join(self.backup_dir, file))
                    os.unlink(os.path.join(self.backup_dir, file))


    def run(self):

        while 1:

            for db in self._databases.iterkeys():
                self._databases[db] = self.get_database_properties(db)

            for db_name, db_properties in self._databases.iteritems():

                date_string = time.strftime("%F", time.localtime())
                backup_file = os.path.join(self.backup_dir, \
                    db_name + "-backup_" + date_string + ".sql")

                if os.path.isfile(backup_file + ".gz"):
                    print __name__, \
                        ": backup file already created (%s)" % (backup_file)
                    continue

                print __name__, \
                    ": database backup [%s] in progress.." % (db_name)

                # mysqldump -h $HOST -u $USER -p$PASS $BASE | gzip -9c > $BACKUP
                os.system('mysqldump -h %s -u %s -p%s %s > %s' % \
                    (db_properties['host'],
                     db_properties['user'],
                     db_properties['pass'],
                     db_properties['base'],
                     backup_file))

                # compress the .sql file
                self.compress_backup_file(backup_file)

            if self.purge:
                self.purge_outdated_files()

            time.sleep(Const.SLEEP)


if __name__ == "__main__":
    backup = Backup(purge=True)
    backup.start()

    while 1:
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            import os, signal
            pid = os.getpid()
            os.kill(pid, signal.SIGTERM)


