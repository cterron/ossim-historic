import sys
import MySQLdb

class OssimDB:

    def __init__ (self) :
        self.__db = None
        self.conn = None

    def connect (self, host, db, user, passwd = ""):
        try:
            self.__db = MySQLdb.connect(host, user, passwd, db)
        except MySQLdb.OperationalError, e:
            print e
            sys.exit()
        self.conn = self.__db.cursor (cursorclass = MySQLdb.cursors.DictCursor)

    # execute query and return the result in a hash
    def exec_query (self, query) :
        self.conn.execute(query)
        return self.conn.fetchall()

    def close (self):
        self.__db.close()


if __name__ == "__main__" :
    db = OssimDB()
    db.connect("localhost", "ossim", "root", "ossim")
    hash = db.exec_query("SELECT * FROM config")
    for row in hash: print row
    db.close()

