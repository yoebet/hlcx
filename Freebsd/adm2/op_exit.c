/****************************************************************
  程序名:op_exit.ec    
  函数名:op_exit()    
  功  能:主机员签退
****************************************************************/              
#include<stdio.h>
#include<assert.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern short sys_operator;    
extern short sys_today;    

extern MYSQL *conn;
extern char sql[];

op_exit()
{
  short oper_num;

  clear_scr(5,22,22,77);
  oper_num=sys_operator;

  sprintf(sql,"UPDATE operfile SET tty_no= '%s' ,tty_pid = %d WHERE operator = %d","无",0,oper_num);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)!=0)){
    sprintf(sys_err_str,"修改操作员错! %s",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }
  
  return(0);
}
