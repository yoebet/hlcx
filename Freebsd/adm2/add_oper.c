/****************************************************************
  程序名:add_oper.ec 
  函数名:add_oper() 
  功  能:增加主机员
****************************************************************/              
#include<stdio.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today;
extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

add_oper()
{

  char op_level[2],init_pwd[9];
  static short in[3]={15,16,8};
  short num=3,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;
 
  short op_no;
  long day;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《增加主机员》");  
  refresh();

  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

	sprintf(sql,"SELECT op_level FROM operfile WHERE operator=%d",da.operator);
	mysql_query(conn,sql);
	  
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s",mysql_error(conn));
	  dis_str(22,22,sys_err_str,60,0);
	  getch();
	  return(-1);
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row)
	{
    dis_str(22,22,"本操作员号已存在,不可增加!",0,0);
    mysql_free_result(res);
    getch();
    return(-1);
	}
	
  cry_pwd("000000",init_pwd,da.operator);
	sprintf(sql,"insert into operfile values(%d,'%s','7','%s','A','无',0,0)",
	  da.operator,init_pwd,da.customer);
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  op_no=sys_operator;
  day=sys_today;
  
  clear_l22();
  dis_err("增加完毕,初始密码为0,请通知本人修改密码!");
  return(0);
}
