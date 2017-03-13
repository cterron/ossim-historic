# -*- coding: utf-8 -*-
#
# License:
#
#    Copyright (c) 2003-2006 ossim.net
#    Copyright (c) 2007-2016 AlienVault
#    All rights reserved.
#
#    This package is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; version 2 dated June, 1991.
#    You may not use, modify or distribute this program under any other version
#    of the GNU General Public License.
#
#    This package is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this package; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
#    MA  02110-1301  USA
#
#
# On Debian GNU/Linux systems, the complete text of the GNU General
# Public License can be found in `/usr/share/common-licenses/GPL-2'.
#
# Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
#

"""@package BackupManager
This module designed to run all the mysql backups operations
"""
import threading
import random
import os
import glob
import re
import string
import pickle
import commands
import gzip
import MySQLdb
import MySQLdb.cursors

from time import sleep
from threading import Lock
from datetime import datetime, timedelta, date

#
#    LOCAL IMPORTS
#
import Util
from DBConstantNames import *
from OssimDB import OssimDB
from OssimConf import OssimConf
from Logger import Logger

logger = Logger.logger
_CONF = OssimConf()


class DoRestore(threading.Thread):
    """This class is designed to do the restore jobs.
    It runs on a separate thread in process the restores jobs without stops the frameworkd work.
    """
    STATUS_ERROR = -1
    STATUS_OK = 0
    STATUS_WORKING = 1
    STATUS_PENDING_JOB = 2

    def __init__(self):
        threading.Thread.__init__(self)
        self.__status = DoRestore.STATUS_OK
        self.__keepWorking = True
        self.__myDB = OssimDB(_CONF[VAR_DB_HOST],
                              _CONF[VAR_DB_SCHEMA],
                              _CONF[VAR_DB_USER],
                              _CONF[VAR_DB_PASSWORD])
        self.__myDB_connected = False
        self.__bkConfig = {}
        self.__load_configuration()
        self.__reset_job_params()
        self.__mutex = Lock()
        self.__mutexPurge = Lock()
        self.__tables = ['acid_event',
                         'reputation_data',
                         'idm_data',
                         'otx_data',
                         'extra_data']
        self.__msgerror = ""

    def string_status(self):
        """
        This function returns the status of a purge/restore action
        Called by "backup_status" action asked by web UI
        """
        # Error
        if self.__status == DoRestore.STATUS_ERROR:
            status_string = 'status="%s" error="%s"' % (self.__status, self.__msgerror)
        # Pending, Running or Stopped
        else:
            # Purge mode first (does not matter if Restore mode were first)
            if self.__status > 0:
                status_string = 'status="%s"' % self.__status
            else:
                status_string = 'status="0"'
        
        return status_string

    def __reset_job_params(self):
        """Resets all the job parameters.
        """
        self.__beginDate = None
        self.__endDate = None
        self.__entity = ""
        self.__newbbdd = False
        self.__bbddhost = _CONF[VAR_DB_HOST]
        self.__bbdduser = _CONF[VAR_DB_USER]
        self.__bbddpasswd = _CONF[VAR_DB_PASSWORD]

    def __load_configuration(self):
        """Load the backup configuration from the database
        """
        if not self.__myDB_connected:
            if self.__myDB.connect():
                self.__myDB_connected = True
            else:
                logger.error("Can't connect to database")
                return
        query = 'SELECT * FROM config WHERE conf LIKE "%backup%"'
        data = self.__myDB.exec_query(query)
        if data is not None:
            for row in data:
                if row['conf'] in ['backup_base', 'backup_day', 'backup_dir', 'backup_events',
                                   'backup_store', 'frameworkd_backup_storage_days_lifetime']:
                    self.__bkConfig[row['conf']] = row['value']

    def __set_status_flag(self, value):
        """Sets the status flag
        """
        self.__mutex.acquire()
        self.__status = value
        logger.info("Change status status = %s" % self.__status)
        self.__mutex.release()

    def status(self):
        """Returns the job status.
        """
        return self.__status

    def set_job_params(self, dtbegin, dtend, entity, newbbdd, bbddhost, bbdduser, bbddpasswd):
        """Set the params for  a restore job.
        @param dtbegin datetime: restore begin date
        @param dtend datetime: restore end date
        @param entity uuid string: entity whose events we want to restore
        @param newbbdd string: Indicates if we want to use a new database to restore the backup
        @param bbddhost string:[used only when newbbdd = 1] Database host
        @param bbdduser string: [used only when newbbdd = 1] Database user
        @param bbddpasswd string: [used only when newbbdd =1] Database password
        """
        self.__msgerror = ""
        logger.info("""
        backup restore
        begin %s
        end %s
        entity %s
        newbbdd: %s
        bbddhost:%s
        bbdduser:%s
        
        """ % (dtbegin, dtend, entity, newbbdd, bbddhost, bbdduser))
        self.__beginDate = dtbegin
        self.__endDate = dtend
        self.__newbbdd = newbbdd
        self.__entity = entity
        if bbddhost is not None and bbddhost != "":
            self.__bbddhost = bbddhost
        if bbdduser is not None and bbdduser != "":
            self.__bbdduser = bbdduser
        if bbddpasswd is not None and bbddpasswd != "":
            self.__bbddpasswd = bbddpasswd
        self.__set_status_flag(DoRestore.STATUS_PENDING_JOB)

    @staticmethod
    def is_empty_string(value):
        """Check if a string is empty or none.
        """
        return value is None or value == ""

    def __get_create_table_statement(self, host, user, password, database, tablename, gettemporal):
        """Returns the create table statement.
        @param host string: Database host
        @param user string: Database user
        @param password string: Database password
        @param database string: Database scheme name
        @param tablename string: Database table name
        @param gettemporal bool: Indicates if we want the real create table statement
                                 or we want an create temporary table statement.
        """
        # mysqldump -u root -pnMMZ9yFuSu alienvault_siem acid_event --no-data
        create_table_statement = ""
        cmd = "mysqldump -h %s -u %s -p%s %s %s --no-data" % (host, user, password, database, tablename)

        try:
            status, output = commands.getstatusoutput(cmd)
            if status == 0:
                logger.info("create table statement retreived ok")
                create_table_statement = output
            else:
                logger.error("create table statement fail status:%s output:%s" % (status, output))
        except Exception, e:
            logger.error("Create table statement fail: %s" % str(e))

        create_table_statement = create_table_statement.lower()
        lines = []
        if gettemporal:
            lines.append('use alienvault_siem_tmp;')
        for line in create_table_statement.split('\n'):
            if not line.startswith('/*!') and not line.startswith('--'):
                lines.append(line)
        create_table_statement = '\n'.join(lines)
        return create_table_statement

    def __get_is_old_dump(self, backupfile):
        """Checks if a backup file is a backup from 
        the alienvault 4 or older.
        @param backupfile - File to restore
        """
        cmd_check = "zcat %s | grep \"Database: alienvault_siem\"" % backupfile
        status, output = commands.getstatusoutput(cmd_check)
        logger.info("status: %s output: %s" % (status, output))
        if status != 0:
            return True
        return False

    def __set_error(self, msg):
        """Sets the error status
        """
        logger.error(msg)
        self.__msgerror = msg
        self.__set_status_flag(DoRestore.STATUS_ERROR)

    def __unzip_backup_to_file(self, backupfile, outputfile):
        """Unzip a backup_file to the output_file
        """
        rt = True
        try:
            cmd = "gunzip -c %s > %s" % (backupfile, outputfile)
            status, output = commands.getstatusoutput(cmd)
            if status != 0:
                self.__set_error("Error decompressing the file '%s': %s " % (backupfile, output))
                return False

            os.chmod(outputfile, 0644)
        except Exception, e:
            self.__set_error("Error decompressing the file %s: %s " % (backupfile, str(e)))
            rt = False
        return rt

    def __do_old_restore(self, backupfile):
        """Restore an alienvault 3 backup inside the alienvault4 database.
        @param backupfile: File to restore
        """
        logger.info("OLDRESTORE Running restore database from version 3 to version 4")
        # 1 - Create a temporal schemes
        cmd = 'mysql -h %s -u %s -p%s -e "DROP DATABASE IF EXISTS snort_tmp;CREATE DATABASE IF NOT EXISTS snort_tmp;"' \
              % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd)
        status, output = commands.getstatusoutput(cmd)
        if status != 0:
            self.__set_error("OLDRESTORE Error creating the temporal snort database (%s:%s)" % (status, output))
            return False
        
        # 2 - Create the database schema
        cmd = "mysqldump --no-data -h %s -u %s -p%s snort > /tmp/snort_schema.sql" \
              % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd)
        status, output = commands.getstatusoutput(cmd)
        if status != 0:
            self.__set_error("OLDRESTORE Can't dump the database schema (%s:%s)" % (status, output))
            return False
        logger.info("OLDRESTORE schema dumped ok..")
        
        cmd = "mysql --host=%s --user=%s --password=%s snort_tmp < /tmp/snort_schema.sql" \
              % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd)
        
        status, output = commands.getstatusoutput(cmd)
        if status != 0:
            self.__set_error("OLDRESTORE Can't dump the database estructure (%s:%s)" % (status, output))
            return False

        # 3 - Dump the backup file to the temporal database
        logger.info("OLDRESTORE - Dumping the file to the temporal database...")
        random_string = ''.join(random.choice(string.ascii_uppercase) for _ in xrange(10))
        tmpfile = '/tmp/%s.sql' % random_string
        if not self.__unzip_backup_to_file(backupfile, tmpfile):
            self.__set_error("Error building the temporal file...")
            return False

        cmd = "mysql --host=%s --user=%s --password=%s snort_tmp < %s" \
              % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd, tmpfile)
        status, output = commands.getstatusoutput(cmd)

        if status != 0:
            self.__set_error("OLDRESTORE Can't dump the database data (%s:%s)" % (status, output))
            return False

        # 4 - Running migrate script
        cmd = "/usr/share/ossim/scripts/migrate_snort.pl snort_tmp"
        if not self.is_empty_string(self.__entity):
            cmd = "/usr/share/ossim/scripts/migrate_snort.pl snort_tmp %s" % self.__entity
        status, output = commands.getstatusoutput(cmd)
        if status != 0:
            self.__set_error("OLDRESTORE Migrate script fails: %s-%s" % (status, output))
            return False
        logger.info("OLDRESTORE Restore has been successfully executed")
        try:
            os.remove(tmpfile)
            os.remove('/tmp/snort_schema.sql')
        except:
            pass
        return True

    def __do_restore_without_entity(self, backup_file):
        """Restores the backup file regardless of the entity
        @param backup_file file to restore
        """
        tmp_file = "/tmp/set.sql"
        with open(tmp_file, 'w') as create_tmp_database:
            create_tmp_database.write("SET UNIQUE_CHECKS=0;SET @disable_count=1;\n")
        try:
            status, output = commands.getstatusoutput("pigz %s" % tmp_file)
            if status != 0:
                self.__set_error("Error gzipping %s: %s" % (tmp_file, output))
                return False
        except Exception, e:
            self.__set_error("Error gzipping %s: %s" % (tmp_file, str(e)))
            return False
        
        file_date = re.sub(r'.*(\d\d\d\d)(\d\d)(\d\d).*', '\\1-\\2-\\3', backup_file)
        restore_command = "zcat %s.gz %s | " \
                          "grep -i ^insert | " \
                          "mysql --host=%s --user=%s --password=%s alienvault_siem; " \
                          "echo \"CALL alienvault_siem.fill_tables('%s 00:00:00','%s 23:59:59');\" | " \
                          "mysql --host=%s --user=%s --password=%s alienvault_siem; " \
                          "rm -f %s.gz" \
                          % (tmp_file, backup_file, self.__bbddhost, self.__bbdduser, self.__bbddpasswd,
                             file_date, file_date, self.__bbddhost, self.__bbdduser, self.__bbddpasswd, tmp_file)
        logger.info("Running restore ")
        try:
            status, output = commands.getstatusoutput(restore_command)
            if status != 0:
                self.__set_error("Error running restore: %s" % output)
                return False
            else:
                logger.info("Restore OK")
        except Exception, e:
            self.__set_error("Error restoring backup '%s': %s" % (backup_file, str(e)))
            return False

        return True

    def __create_tmp_alienvault_siem_db(self):
        """Creates the alienvault_siem_tmp database
        """
        tmp_file = "/tmp/createdb.sql"
        with open(tmp_file, 'w') as create_tmp_database:
            create_tmp_database.write("DROP DATABASE IF EXISTS alienvault_siem_tmp;\n")
            create_tmp_database.write("CREATE DATABASE alienvault_siem_tmp;\n")

            # Create the temporary tables.
            for table in self.__tables:
                create_tmp_database.write(self.__get_create_table_statement(
                    self.__bbddhost, self.__bbdduser, self.__bbddpasswd, 'alienvault_siem', table, True))

        create_db_command = "mysql --host=%s --user=%s --password=%s < %s" \
                            % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd, tmp_file)
        status, output = commands.getstatusoutput(create_db_command)
        if status != 0:
            self.__set_error("Can't create temporal db %s" % output)
            return False
        return True

    def __do_restore(self, filename):
        """Restores the backup file taking into account the entity
        @param filename: backup file to restore
        """
        try:
            # 1 - Create a temporal database
            if not self.__create_tmp_alienvault_siem_db():
                return False
            db = MySQLdb.connect(host=self.__bbddhost, user=self.__bbdduser, passwd=self.__bbddpasswd)
            db.autocommit(True)
            cursor = db.cursor()

            # 2 - Unzip the backup file
            random_string = ''.join(random.choice(string.ascii_uppercase) for _ in xrange(10))
            tmp_file = '/tmp/%s.sql' % random_string
            if not self.__unzip_backup_to_file(filename, tmp_file):
                self.__set_error("Unable to decompress the backup file")
                return False

            # 3 - Dumps the backup over the tmp database
            try:
                cmd = "mysql --host=%s --user=%s --password=%s alienvault_siem_tmp < %s" \
                      % (self.__bbddhost, self.__bbdduser, self.__bbddpasswd, tmp_file)
                status, output = commands.getstatusoutput(cmd)
                os.remove(tmp_file)
            except Exception, e:
                self.__set_error(str(e))
                return False

            # 4 - Now remove all the data that do not belongs to the entity.A
            logger.info("Removing data from other entities")
            query_remove_acid_event = "DELETE FROM alienvault_siem_tmp.acid_event WHERE ctx!=unhex('%s')" \
                                      % self.__entity.replace('-', '')
            logger.info("Removing data from acid event: %s" % query_remove_acid_event)
            cursor.execute(query_remove_acid_event)
            cursor.fetchall()

            query_remove_reputation = "DELETE FROM alienvault_siem_tmp.reputation_data " \
                                      "WHERE event_id NOT IN (SELECT event_id FROM alienvault_siem_tmp.acid_event)"
            logger.info(query_remove_reputation)
            cursor.execute(query_remove_reputation)
            cursor.fetchall()

            query_remove_idm_data = "DELETE FROM alienvault_siem_tmp.idm_data " \
                                    "WHERE event_id NOT IN (SELECT event_id FROM alienvault_siem_tmp.acid_event)"
            logger.info(query_remove_idm_data)
            cursor.execute(query_remove_idm_data)
            cursor.fetchall()

            query_remove_otx_data = "DELETE FROM alienvault_siem_tmp.otx_data " \
                                    "WHERE event_id NOT IN (SELECT event_id FROM alienvault_siem_tmp.acid_event)"
            logger.info(query_remove_otx_data)
            cursor.execute(query_remove_otx_data)
            cursor.fetchall()

            query_remove_extra_data = "DELETE FROM alienvault_siem_tmp.extra_data " \
                                      "WHERE event_id NOT IN (SELECT event_id FROM alienvault_siem_tmp.acid_event)"
            logger.info(query_remove_extra_data)
            cursor.execute(query_remove_extra_data)
            cursor.fetchall()
            # 5 - finally record all the data and insert it on alienvault_siem
            # table: acid_event
            querytmp = "SELECT * INTO OUTFILE '/tmp/acid_event.sql' FROM alienvault_siem_tmp.acid_event"
            cursor.execute(querytmp)
            cursor.fetchall()

            querytmp = "LOAD DATA INFILE '/tmp/acid_event.sql' INTO TABLE alienvault_siem.acid_event"
            self.__myDB.exec_query(querytmp)

            logger.info("Restored data to acid_event")

            # table: reputation_data:
            querytmp = "SELECT * INTO OUTFILE '/tmp/reputation_data.sql' FROM alienvault_siem_tmp.reputation_data"
            cursor.execute(querytmp)
            cursor.fetchall()
            
            querytmp = "LOAD DATA INFILE '/tmp/reputation_data.sql' INTO TABLE alienvault_siem.reputation_data"
            self.__myDB.exec_query(querytmp)
            logger.info("Restored data to reputation_data")

            # table: otx_data:
            querytmp = "SELECT * INTO OUTFILE '/tmp/otx_data.sql' FROM alienvault_siem_tmp.otx_data"
            cursor.execute(querytmp)
            cursor.fetchall()
            
            querytmp = "LOAD DATA INFILE '/tmp/otx_data.sql' INTO TABLE alienvault_siem.otx_data"
            self.__myDB.exec_query(querytmp)
            logger.info("Restored data to otx_data")

            # table: idm_data:
            querytmp = "SELECT * INTO OUTFILE '/tmp/idm_data.sql' FROM alienvault_siem_tmp.idm_data"
            cursor.execute(querytmp)
            cursor.fetchall()
            querytmp = "LOAD DATA INFILE '/tmp/idm_data.sql' INTO TABLE alienvault_siem.idm_data"
            self.__myDB.exec_query(querytmp)
            logger.info("Restored data to idm_data")

            # 6 - Remove the temporary database and the temporal files
            try:
                query = "DROP DATABASE IF EXISTS alienvault_siem_tmp"
                cursor.execute(query)
                cursor.fetchall()
                os.remove('/tmp/acid_event.sql')
                os.remove('/tmp/reputation_data.sql')
                os.remove('/tmp/otx_data.sql')
                os.remove('/tmp/idm_data.sql')
                cursor.close()
                db.close()
            except Exception, e:
                self.__set_error("Error cleaning the data used to restore: %s" % str(e))
                return False
        except Exception, e:
            self.__set_error("Can't do the restore: %s" % str(e))
            return False

        return True

    def __do_job(self):
        """Runs the restore job.
        """
        self.__set_status_flag(DoRestore.STATUS_WORKING)
        logger.info("Running restore job ....")
        filestorestore = []
        insertfile = "insert-%s.sql.gz" % str(self.__beginDate.date()).replace('-', '')
        insertfile = os.path.join(self.__bkConfig['backup_dir'], insertfile)
        filestorestore.append(insertfile)
        total_files = []

        while self.__endDate > self.__beginDate:
            self.__beginDate += timedelta(days=1)
            insertfile = "insert-%s.sql.gz" % str(self.__beginDate.date()).replace('-', '')
            insertfile = os.path.join(self.__bkConfig['backup_dir'], insertfile)
            filestorestore.append(insertfile)

        for filename in glob.glob(os.path.join(self.__bkConfig['backup_dir'], 'insert-*.sql.gz')):
            if filename in filestorestore:
                logger.info("Appending file to restore job: %s" % filename)
                total_files.append(filename)

        for filename in total_files:
            if self.__get_is_old_dump(filename):
                rt = self.__do_old_restore(filename)
            elif self.is_empty_string(self.__entity):
                rt = self.__do_restore_without_entity(filename)
            else:
                rt = self.__do_restore(filename)
            if not rt:
                return
        self.__set_status_flag(DoRestore.STATUS_OK)

    def run(self):
        """Thread entry point.
        Waits until new jobs arrives
        """
        while self.__keepWorking:
            if self.__status == DoRestore.STATUS_PENDING_JOB:
                self.__do_job()

            sleep(1)


class BackupRestoreManager:
    """Class to manage all the restore request from the web.
    """
    
    def __init__(self, conf):
        """ Default Constructor.
        @param conf: OssimConf Configuration object.
        """
        logger.info("Initializing  BackupRestoreManager")
        self.__conf = conf
        self.__worker = DoRestore()
        self.__worker.start()

    def process(self, message):
        """ Process the requests:
        @param message string: Request to process. 
            Examples:
            backup action="backup_restore"  begin="YYYY-MM-DD" end="YYYY-MM-DD"
                   entity="ab352ced-c83d-4c9b-bc55-aae6c3e0069d" newbbdd="1" bbddhost="192.168.2.1" bbdduser="pepe"
                   bbddpasswd="kktua" \n
            backup action="backup_status"

            # FYI: Purge related staff was removed from UI in ENG-99491 and was not working since than.
                   Now removed from backend too.
            backup action="purge_events" dates="2012-05-12,2012-05-14,2012-05-15"
                   bbddhost="host" bbdduser="kkka2" bbddpasswd="agaag"
        """
        action = Util.get_var("action=\"([a-z\_]+)\"", message)

        if action == "backup_restore":
            logger.info("Restoring")
            begindate = Util.get_var("begin=\"(\d{4}\-\d{2}\-\d{2})\"", message)
            enddate = Util.get_var("end=\"(\d{4}\-\d{2}\-\d{2})\"", message)
            entity = Util.get_var("entity=\"([a-f0-9]{8}\-[0-9a-f]{4}\-[0-9a-f]{4}\-[0-9a-f]{4}\-[0-9a-f]{12})\"",
                                  message.lower())
            newbbdd = Util.get_var("newbbdd=\"(0|1|yes|true|no|false)\"", message.lower())
            bbddhost = Util.sanitize(Util.get_var("bbddhost=\"(\S+)\"", message))
            bbdduser = Util.sanitize(Util.get_var("bbdduser=\"(\S+)\"", message))
            bbddpasswd = Util.sanitize(Util.get_var("bbddpasswd=\"(\S+)\"", message))
            try:
                dtbegin = datetime.strptime(begindate, '%Y-%m-%d')
            except Exception:
                response = message + ' errno="-1" error="Invalid begin date. Format YYYY-MM-DD"  ackend\n'
                return response
            try:
                dtend = datetime.strptime(enddate, '%Y-%m-%d')
            except Exception:
                response = message + ' errno="-2" error="Invalid end date. Format YYYY-MM-DD" ackend\n'
                return response
            if dtend < dtbegin:
                response = message + ' errno="-3" error="End date < Begin Date" ackend\n'
                return response

            self.__worker.set_job_params(dtbegin, dtend, entity, newbbdd, bbddhost, bbdduser, bbddpasswd)
            response = message + ' status="%s" ackend\n' % self.__worker.status()

        elif action == "backup_status":
            logger.info("status")
            response = message + ' %s ackend\n' % self.__worker.string_status()
        else:
            response = message + ' errno="-4" error="Unknown command" ackend\n' 

        return response


class BackupManager(threading.Thread):
    """Manage the periodic backups.
    """
    UPDATE_BACKUP_FILE = '/etc/ossim/framework/lastbkday.fkm'
    LISTEN_SOCK = '/etc/ossim/framework/bksock.sock'

    def __init__(self):
        """Default constructor.
        """
        threading.Thread.__init__(self)
        self.__myDB = OssimDB(_CONF[VAR_DB_HOST],
                              _CONF[VAR_DB_SCHEMA],
                              _CONF[VAR_DB_USER],
                              _CONF[VAR_DB_PASSWORD])
        self.__myDB_connected = False
        self.__keepWorking = True
        # self.__mutex = Lock()
        self.__bkConfig = {}
        self.__load_configuration()
        self.__stopE = threading.Event()
        self.__stopE.clear()

    def __load_configuration(self):
        """Loads the backup manager configuration and updates it with the database values.
        """
        self.__load_backup_config()
        if not self.__myDB_connected:
            if self.__myDB.connect():
                self.__myDB_connected = True
            else:
                logger.error("Can't connect to database")
                return
        query = 'SELECT * FROM config WHERE conf LIKE "%backup%"'
        data = self.__myDB.exec_query(query)
        tmp_config = {}
        if data is not None:
            for row in data:
                if row['conf'] in ['backup_base', 'backup_day', 'backup_dir', 'backup_events', 'backup_store',
                                   'frameworkd_backup_storage_days_lifetime', 'backup_hour']:
                    tmp_config[row['conf']] = row['value']

        for key, value in tmp_config.iteritems():
            if key == 'last_run':
                continue
            if key not in self.__bkConfig:
                logger.info("Backup new config key: '%s' '%s'" % (key, value))
                self.__bkConfig[key] = tmp_config[key]
            elif value != self.__bkConfig[key]:
                logger.info('Backup Config value has changed %s=%s and old value %s=%s'
                            % (key, value, key, self.__bkConfig[key]))
                self.__bkConfig[key] = tmp_config[key]

        if 'last_run' not in self.__bkConfig:
            self.__bkConfig['last_run'] = date(year=1, month=1, day=1)
        if 'backup_day' not in self.__bkConfig:
            self.__bkConfig['backup_day'] = 30
        if 'frameworkd_backup_storage_days_lifetime' not in self.__bkConfig:
            self.__bkConfig['frameworkd_backup_storage_days_lifetime'] = 5

        self.__update_backup_config_file()

    def __update_backup_config_file(self):
        """Update the backup configuration file
        """
        try:
            with open(BackupManager.UPDATE_BACKUP_FILE, "wb") as bk_configfile:
                pickle.dump(self.__bkConfig, bk_configfile)
            os.chmod(BackupManager.UPDATE_BACKUP_FILE, 0644)
        except Exception, e:
            logger.error("Error dumping backup config update_file...: %s" % str(e))

    def __load_backup_config(self):
        """Load the backup configuration from the backup file
        """
        self.__bkConfig = {}

        if os.path.isfile(BackupManager.UPDATE_BACKUP_FILE):
            try:
                with open(BackupManager.UPDATE_BACKUP_FILE) as bk_configfile:
                    self.__bkConfig = pickle.load(bk_configfile)
                if not isinstance(self.__bkConfig, dict):
                    logger.warning("Error loading backup configuration file.")
                    logger.info("New configuration will be loaded from database")
                    self.__bkConfig = {}
            except Exception, e:
                logger.warning("Error loading backup configuration file...:%s" % str(e))
                logger.info("New configuration will be loaded from database")
                self.__bkConfig = {}

    def purge_old_backup_files(self):
        """Purge old backup files.
        """
        backup_files = []
        backup_days = 5
        try:
            backup_days = int(_CONF[VAR_BACKUP_DAYS_LIFETIME])
        except ValueError:
            logger.warning("Invalid value for backup_day in config table")
        today = datetime.now() - timedelta(days=1)
        while backup_days > 0:
            dtstr = "%s" % today.date().isoformat()
            dtstr = dtstr.replace('-', '')
            str_insert = '%s/insert-%s.sql.gz' % (self.__bkConfig['backup_dir'], dtstr)
            str_delete = '%s/delete-%s.sql.gz' % (self.__bkConfig['backup_dir'], dtstr)  # For backward compatibility
            backup_files.append(str_insert)
            backup_files.append(str_delete)
            backup_days -= 1
            today -= timedelta(days=1)

        for bkp_file in glob.glob(os.path.join(self.__bkConfig['backup_dir'], '[insert|delete]*.sql.gz')):
            if bkp_file not in backup_files:
                logger.info("Removing outdated backup file: %s" % bkp_file)
                try:
                    os.unlink(bkp_file)
                except Exception, e:
                    logger.error("Error removing outdated files: %s" % str(e))

    def get_current_backup_files(self):
        backup_files = []
        try:
            backup_files = glob.glob(os.path.join(self.__bkConfig['backup_dir'], 'insert*.sql.gz'))
        except Exception as err:
            logger.error("An error occurred while reading the current database backups %s" % str(err))
        return backup_files

    @staticmethod
    def check_disk_usage():
        """Check max disk usage.
        """
        mega = 1024 * 1024
        disk_state = os.statvfs('/var/ossim/')
        # free space in megabytes.
        capacity = float((disk_state.f_bsize * disk_state.f_blocks) / mega)
        free_space = float((disk_state.f_bsize * disk_state.f_bavail) / mega)
        percentage_free_space = (free_space * 100) / capacity
        min_free_space_allowed = 10
        try:
            min_free_space_allowed = 100 - int(_CONF[VAR_BACKUP_MAX_DISKUSAGE])
        except Exception, e:
            logger.error("Error when calculating free disk space: %s" % str(e))

        logger.debug("Min free space allowed: %s - current free space: %s" % (min_free_space_allowed,
                                                                              percentage_free_space))
        if percentage_free_space < min_free_space_allowed:
            return False
        return True

    def __is_process_running(self, process_name=""):
        """
        Check if there is a process running in the system
        """
        try:
            num_process = int(
                commands.getoutput("ps auxwww | grep %s | grep -v grep | grep -v tail | wc -l" % process_name))
            if num_process > 0:
                return True
        except Exception, e:
            logger.warning("Error checking process status '%s': %s" % process_name, str(e))

        return False

    def __should_run_backup(self):
        """Checks if it should runs a new backup.
        Backup Hour: By default every day at 01:00:00
        """
        now = datetime.now()
        backup_hour = now.replace(year=self.__bkConfig['last_run'].year,
                                  month=self.__bkConfig['last_run'].month,
                                  day=self.__bkConfig['last_run'].day,
                                  hour=1,
                                  minute=0,
                                  second=0) + timedelta(days=1)

        if 'backup_hour' in self.__bkConfig:
            try:
                (config_backup_hour, config_backup_minute) = self.__bkConfig['backup_hour'].split(':')
                backup_hour = backup_hour.replace(hour=int(config_backup_hour),
                                                  minute=int(config_backup_minute))
            except Exception, e:
                logger.error("Wrong backup_hour: %s" % str(e))
                logger.warning("Bad parameter in backup_hour config table, using default time (01:00:00 Local time)")

        # Run backups when:
        # - It has reached the backup hour
        # - alienvault-reconfig is not running
        # - alienvault-update is not running
        logger.debug('Time now: %s  - Scheduled to run backup at: %s' % (now, backup_hour))
        if backup_hour > now:
            return False
        if self.__is_process_running('alienvault-reconfig'):
            logger.info("There is a alienvault-reconfig process running. Cannot run a Backup at this time")
            return False
        if self.__is_process_running('alienvault-update'):
            logger.info("There is a alienvault-update process running. Cannot run a Backup at this time")
            return False

        return True

    def __delete_events_older_than_timestamp(self, limit_date):
        """
        Delete all the events older than %limit_date
        """
        deletes = []
        total_events = 0

        # Get the date of the oldest event in database
        begin_date = self.__get_oldest_event_in_database_datetime()

        # Runs the deletes...
        while begin_date < limit_date:
            end_date = begin_date.replace(hour=23, minute=59, second=59, microsecond=0)
            if end_date > limit_date:
                end_date = limit_date
            query = "SELECT COUNT(id) AS total FROM alienvault_siem.acid_event WHERE timestamp BETWEEN '%s' AND '%s';" \
                    % (begin_date, end_date)

            query_result = self.__myDB.exec_query(query)
            if len(query_result) == 1:
                events_to_delete = query_result[0]['total']
                total_events += events_to_delete
                if events_to_delete != 0:
                    logger.info("Events to delete: '%s' events from %s to %s" % (events_to_delete,
                                                                                 begin_date,
                                                                                 end_date))
                    block = 100000
                    delete_tmp_table = "alienvault_siem.backup_delete_temporal"
                    delete_mem_table = "alienvault_siem.backup_delete_memory"

                    # Get the event ids from begin_date to end_date to delete
                    deletes.append("CREATE TABLE IF NOT EXISTS %s (id binary(16) NOT NULL, PRIMARY KEY (id));"
                                   % delete_tmp_table)
                    deletes.append("TRUNCATE TABLE %s;" % delete_tmp_table)
                    deletes.append("INSERT IGNORE INTO %s SELECT id FROM alienvault_siem.acid_event "
                                   "WHERE timestamp BETWEEN '%s' and '%s';" % (delete_tmp_table, begin_date, end_date))
                    deletes.append("CREATE TEMPORARY TABLE %s (id binary(16) NOT NULL, PRIMARY KEY (`id`)) "
                                   "ENGINE=MEMORY;" % delete_mem_table)
                    # Delete by blocks
                    for _ in xrange(0, events_to_delete + 1, block):
                        deletes.append("INSERT INTO %s SELECT id FROM %s LIMIT %d;" % (delete_mem_table,
                                                                                       delete_tmp_table,
                                                                                       block))
                        deletes.append("CALL alienvault_siem.delete_events('%s');" % delete_mem_table)
                        deletes.append("DELETE t FROM %s t, %s m WHERE t.id=m.id;" % (delete_tmp_table,
                                                                                      delete_mem_table))
                        deletes.append("TRUNCATE TABLE %s;" % delete_mem_table)

                    deletes.append("DROP TABLE %s;" % delete_mem_table)
                    deletes.append("DROP TABLE %s;" % delete_tmp_table)

                    # Delete accumulate tables entries
                    deletes.append("DELETE FROM alienvault_siem.ac_acid_event WHERE timestamp <= '%s';" % end_date)
                    deletes.append("DELETE FROM alienvault_siem.po_acid_event WHERE timestamp <= '%s';" % end_date)

                # Go to the next date
                next_day = begin_date.replace(hour=0, minute=0, second=0, microsecond=0) + timedelta(days=1)
                next_date = self.__get_oldest_event_in_database_datetime(next_day.strftime("%Y-%m-%d %H:%M:%S"))

                # Check last day of events
                if begin_date.day == next_date.day:
                    break

                begin_date = next_date
        deletes.append("CALL alienvault_siem.fill_tables('%s', '%s')" % ('1900-01-01 00:00:00',
                                                                         begin_date.strftime("%Y-%m-%d %H:%M:%S")))

        logger.info("-- Total events to delete: %s" % total_events)

        for delete in deletes:
            logger.info("Running delete: %s" % delete)
            try:
                self.__myDB.exec_query(delete)
            except Exception, e:
                logger.error("Error running delete: %s", str(e))

    def __delete_by_backup_days(self):
        """ Runs the delete command using the backup_day threshold
        """
        #
        # Check by backup days.
        #
        try:
            backup_days = int(self.__bkConfig['backup_day'])
        except Exception:
            logger.warning("Invalid value for: Events to keep in the Database (Number of days) -> %s"
                           % self.__bkConfig['backup_day'])
            backup_days = 0

        if backup_days > 0:  # backup_days = 0 unlimited.
            limit_day = datetime.now().replace(hour=0, minute=0,
                                               second=0, microsecond=0) - timedelta(days=int(backup_days))
            self.__delete_events_older_than_timestamp(limit_day)
        else:
            logger.info("Unlimited number of events. Events to keep in the Database (Number of days) = %s"
                        % backup_days)

    def __delete_by_number_of_events(self):
        """ Runs the delete using the maximum number of events in the database
            as threshold.
        """
        #
        # Check by max number of events in the Database.
        #
        try:
            max_events = int(self.__bkConfig['backup_events'])
        except Exception:
            logger.info("Invalid value for: Events to keep in the Database (Number of events) -> %s"
                        % self.__bkConfig['backup_events'])
            max_events = 0

        if max_events > 0:  # backup_events = 0 -> unlimited
            query = "SELECT timestamp FROM alienvault_siem.acid_event ORDER BY timestamp DESC LIMIT 1 OFFSET %s;" \
                    % self.__bkConfig['backup_events']
            data = self.__myDB.exec_query(query)

            if len(data) == 1:
                limit_date = data[0]['timestamp']
                self.__delete_events_older_than_timestamp(limit_date)
        else:
            logger.info("Unlimited number of events. Events to keep in the Database (Number of events) = %s"
                        % max_events)

    def __get_oldest_event_in_database_datetime(self, min_timestamp='1900-01-01 00:00:00'):
        """Returns the datetime of the oldest event in the database
        """
        oldest_event = datetime.now()

        query = "SELECT MIN(timestamp) AS last_event FROM alienvault_siem.acid_event WHERE timestamp > '%s';" \
                % min_timestamp
        # Get the oldest event from the database and do the backup from that day
        # until yesterday (do if not exists yet).
        data = self.__myDB.exec_query(query)
        if len(data) == 1:
            oldest_event = data[0]['last_event']
            if oldest_event is None:
                oldest_event = datetime.now()
            oldest_event = oldest_event.replace(hour=0, minute=0, second=0, microsecond=0)
        return oldest_event

    def __store_backups(self):
        """Returns if we have to store the backups.
        """
        rt = False
        if self.__bkConfig['backup_store'].lower() in ['1', 'yes', 'true']:
            rt = True
        return rt

    def __is_backups_enabled(self):
        """Returns if the backups are enabled...
        """
        try:
            max_events = int(self.__bkConfig['backup_events'])
        except Exception:
            logger.info("Invalid value for: Events to keep in the Database (Number of events) -> %s"
                        % self.__bkConfig['backup_events'])
            max_events = 0
        try:
            backup_days = int(self.__bkConfig['backup_day'])
        except Exception:
            logger.warning("Invalid value for: Events to keep in the Database (Number of days) -> %s"
                           % self.__bkConfig['backup_day'])
            backup_days = 0
        backup_enabled = True
        if max_events == 0 and backup_days == 0:
            logger.info("Backups are disabled  MaxEvents = 0, BackupDays = 0")
            backup_enabled = False
        return backup_enabled

    def __run_backup(self):
        """Run the backup job.
        """
        # Check the disk space
        if not self.check_disk_usage():
            logger.warning("[ALERT DISK USAGE] Can not run backups due to low free disk space")
            return
        # Purge old backup files
        self.purge_old_backup_files()
        backup_cmd = "ionice -c2 -n7 mysqldump alienvault_siem $TABLE -h %s -u %s -p%s -c -n -t -f --skip-add-locks " \
                     "--skip-disable-keys --skip-triggers --single-transaction --hex-blob --quick --insert-ignore -w " \
                     "$CONDITION" % (_CONF[VAR_DB_HOST], _CONF[VAR_DB_USER], _CONF[VAR_DB_PASSWORD])

        # Should I  store the backups? -> only if store is true.
        if self.__should_run_backup():  # Time to do the backup?
            logger.info("Running backups system...")
            # do backup
            if not self.__myDB_connected:
                if self.__myDB.connect():
                    self.__myDB_connected = True
                else:
                    logger.info("Can't connect to database..")
                    return

            self.__bkConfig['last_run'] = datetime.now().date()
            first_event_date_time = self.__get_oldest_event_in_database_datetime()
            try:
                bkp_days = int(_CONF[VAR_BACKUP_DAYS_LIFETIME])
            except ValueError:
                bkp_days = 5

            threshold_day = datetime.today() - timedelta(days=bkp_days+1)

            if self.__store_backups() and self.__is_backups_enabled():
                try:
                    today = datetime.now().replace(hour=0, minute=0, second=0, microsecond=0)
                    while first_event_date_time < today:
                        # Changes: #8312
                        # We should create a dump file only for the last backup days
                        if first_event_date_time < threshold_day:
                            logger.info("Do not make backup because threshold day: first event datetime: %s, "
                                        "threshold day: %s" % (first_event_date_time, threshold_day))
                            first_event_date_time += timedelta(days=1)
                            continue
                        logger.info("="*50+"BACKUP: %s" % first_event_date_time)
                        backup_cmds = {}
                        date_backup = '%s' % first_event_date_time.date().isoformat()
                        date_backup_formatted = date_backup.replace('-', '')
                        insert_backup_file = '%s/insert-%s.sql' % (self.__bkConfig['backup_dir'], date_backup_formatted)
                        # For backward compatibility.
                        delete_backup_file = '%s/delete-%s.sql' % (self.__bkConfig['backup_dir'], date_backup_formatted)

                        # if file is already created, continue
                        if insert_backup_file+".gz" in self.get_current_backup_files():
                            first_event_date_time += timedelta(days=1)
                            logger.info("BACKUP %s Ignoring.... backup has already been done" % insert_backup_file)
                            continue

                        logger.info("New backup file: %s" % insert_backup_file)
                        #######################
                        # ACID EVENT
                        #######################
                        backup_acid_event_cmd = backup_cmd.replace('$TABLE', 'acid_event')
                        condition = '"timestamp BETWEEN \'%s 00:00:00\' AND \'%s 23:59:59\'"' % (date_backup,
                                                                                                 date_backup)
                        backup_acid_event_cmd = backup_acid_event_cmd.replace('$CONDITION', condition)
                        backup_cmds["%s_%s" % ('acid_event', date_backup)] = backup_acid_event_cmd
                        #######################
                        # RELATED TABLES
                        #######################
                        condition = '"event_id IN (' \
                                    'SELECT id FROM alienvault_siem.acid_event WHERE timestamp ' \
                                    'BETWEEN \'%s 00:00:00\' AND \'%s 23:59:59\')"' % (date_backup, date_backup)
                        for table in ['reputation_data', 'idm_data', 'otx_data', 'extra_data']:
                            cmd = backup_cmd.replace('$TABLE', table)
                            cmd = cmd.replace('$CONDITION', condition)
                            backup_cmds['%s_%s' % (table, date_backup)] = cmd

                        for table_day, cmd in backup_cmds.iteritems():
                            cmd += " >> %s" % insert_backup_file
                            status, output = commands.getstatusoutput(cmd)
                            if status == 0:
                                logger.info("Running Backup for day %s  OK" % table_day)
                            else:
                                logger.error("Error (%s) running: %s" % (status, table_day))
                                return
                        try:
                            status, output = commands.getstatusoutput("pigz -f %s" % insert_backup_file)
                            if status == 0:
                                logger.info("Backup file has been compressed")
                                os.chmod(insert_backup_file + ".gz", 0640)

                                # Create empty delete files, because UI uses them to show backups loaded in the DB.
                                with gzip.open(delete_backup_file + '.gz', 'w'):
                                    pass
                        except Exception, e:
                            logger.error("Error writing backup file: %s" % str(e))
                            return
                        first_event_date_time += timedelta(days=1)
                except Exception, e:
                    logger.error("Error running the backup: %s" % str(e))
            #
            # Deletes....
            #
            self.__delete_by_backup_days()
            self.__delete_by_number_of_events()
            self.__update_backup_config_file()

    def run(self):
        """ Entry point for the thread.
        """
        loop = 0
        while not self.__stopE.isSet():
            loop += 1
            # Reload configuration every 10 minutes
            if loop >= 20:
                logger.info("Reloading Backup Configuration")
                self.__load_configuration()
                loop = 0

            logger.info("BackupManager - Checking....")
            self.__run_backup()
            sleep(30)

    def stop(self):
        """Stop the current thread execution
        """
        self.__stopE.set()
