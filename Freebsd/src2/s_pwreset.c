/****************************************************************
函数名:s_pwreset()
功  能:密码重置
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[];
extern short sys_branch,sys_operator;

extern MYSQL *conn;
extern char sql[];

char qq;

s_pwreset()
{
  static short in[4]={70,72,73,8};
  short num=4,flag;
  struct in_data da;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《密码重置》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)return(0);

  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';

  da.open_br=100;
  
  sprintf(sql,"update s_mast set passwd='%s' where open_br=%d and account=%ld",
    da.pwd_new2,da.open_br,da.account);
    
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"发生错误!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
  }else{
    dis_str(22,22,"密码重置成功",60,0);
    getch();
  }
  
  return(0);
}

