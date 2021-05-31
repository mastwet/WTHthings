import pymysql

class cdk_db:

    __username = 'root'
    __passwd = 'HETONGsoft12345*'
    __host = 'gz-cdb-1i5dy153.sql.tencentcdb.com'
    __port = 59755

    con = None
    cursor = None

    def __init__(self):
        self.con = pymysql.connect(host=self.__host, port=self.__port, user=self.__username, password=self.__passwd, db='ticket_cdk', charset='utf8')
        self.cursor = self.con.cursor()
        try:
           self.__test()
        except Exception as e:
            raise e

    def __del__(self):
        self.con.close()
        self.cursor.close()

    #ok!
    def __test(self):

        check_db_sql = "SHOW TABLES;"
        check_table_sql = "DESCRIBE _cdk;"
        query_sql = "SELECT * FROM _cdk WHERE _verifi=2;"
        add_sql = "INSERT INTO _cdk(number,cdk,_verifi) VALUES(0,0,2);"
        del_sql = "DELETE FROM _cdk WHERE _verifi=2;"

        try:
            self.cursor.execute(check_db_sql)
            self.cursor.execute(check_table_sql)
            self.cursor.execute(add_sql)
            self.cursor.execute(query_sql)
            query = self.cursor.fetchone()
            self.cursor.execute(del_sql)
        except Exception as e:
            raise e
        if query == None:
            raise IOError('Cannot w/r to sqlserver!')

    def check_cdkey(self,number):
        try:
           query_sql = "SELECT * FROM _cdk WHERE number='{num}'".format(num = number);
           key_exis = self.cursor.execute(query_sql)
        except pymysql.err.OperationalError:
            return False
        except Exception:
            return False
        if key_exis:
            return True
        else:
            return False



    def add_cdkey(self,number,key):
        try:
            _insert_cdk = "INSERT INTO _cdk(number,cdk,_verifi) VALUES('{number}','{key}',1);".format(number=number,key=key)
            self.cursor.execute(_insert_cdk)
            self.con.commit()
        except Exception as e:
            raise e

    def del_cdkey(self,number):
        query_sql = "SELECT * FROM _cdk WHERE number='{num}'".format(num=number)
        del_sql = "DELETE FROM _cdk WHERE number='{num}';".format(num=number)
        if self.cursor.execute(query_sql) == 1:
           try:
               self.cursor.execute(del_sql)
               self.con.commit()
           except Exception as e:
               raise e

    def show_cdkey(self,number):
        query_sql = "SELECT * FROM _cdk WHERE number='{num}'".format(num=number)
        try:
          self.cursor.execute(query_sql)
        finally:
          return self.cursor.fetchone()

if __name__ == '__main__':
    db = cdk_db()
    print(db.show_cdkey('123'))
