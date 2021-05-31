
import random
import database

class cdk:

    __db = None

    def __init__(self):
        self.__db = database.cdk_db()

    def __has_cdk(self,num):
        if self.__db.check_cdkey(number=num):
            return True

    def gen_cdk(self,order_form):
        number = self.__gen_number(order_form)
        if self.__has_cdk(number):
            cd = self.__db.show_cdkey(number)
            return cd
        else:
            cdk = self.__gen_passwd()
            cd = (cdk,number,1)
            self.__db.add_cdkey(number,key=cdk)
            return cd

    def del_cdk(self,order_form):
        number = self.__gen_number(order_form)
        if self.__has_cdk(number):
            cd = self.__db.show_cdkey(number)
            self.__db.del_cdkey(number)

    def __gen_number(self,order_form):
        return order_form[0:9]

    def __gen_passwd(self):
        str = "qazwsxedcrfvtgbyhnujmikolp1234567890"
        cdk = ""
        for i in range(0, 20):
            cdk += str[random.randint(0, 35)]
        return cdk

    def __check_sdk(self,order_form):
        number = self.__gen_number(order_form)
        if self.__db.check_cdkey(number):
            return True

if __name__== '__main__':
    #test code dont
  #  cd = cdk()
    #print(cd.gen_cdk("098098098098098098"))
    #cd.del_cdk("098098098098098098")
    pass
