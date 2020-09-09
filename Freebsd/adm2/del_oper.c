/****************************************************************
  程序名:del_oper.ec    
  函数名:del_oper()    
  功  能:删除主机管理员
****************************************************************/              
#include<stdio.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern short sys_operator;    
extern short sys_today; 
extern MYSQL *conn;
extern char sql[];

del_oper()
{

  struct f_operator op;
  short cnt,op_no;
  long day;
  static short in[2]={15,8};
  short num=2,flag;
  struct in_data da;
  
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《删除主机员》");  

  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  clear_line(22,22,77);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

  if (da.operator==sys_operator)
  {
    dis_str(22,22,"不可删除自己!",0,0);
    getch();
    return(-1);
  }
  
	sprintf(sql,"SELECT * FROM operfile WHERE operator=%d",da.operator);
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
	if(row==NULL)
	{
    dis_str(22,22,"无此编号的操作员!",0,0);
    getch();
    return(-1);
	}
	
	set_f_operator(&op,row);
  mysql_free_result(res);
  
  mvprintw(9,30,"主机员姓名:[%s]",op.op_name);
  refresh();
  if (op.op_flag[0]!='A')
  {
    dis_str(22,22,"此编号已作废!",0,0);
    getch();
    return(-1);
  }
  if (op.op_level[0]!='7')
  {
    dis_str(22,22,"此编号不是主机员!",0,0);
    getch();
    return(-1);
  }
  
	sprintf(sql,"SELECT count(*) FROM operfile WHERE op_flag='A' and op_level='7'");
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
	cnt=atoi(row[0]);
  mysql_free_result(res);
	
  if(cnt==1)
  {
    dis_str(22,22,"只有一个主机员!",0,0);
    getch();
    return(-1);
  }
  if(strncmp(op.tty_no,"tty",3)==0) 
  {
    mvprintw(22,22,"本主机员已登录在[%s]终端上!",op.tty_no);
    refresh();
    getch();
    return(-1);
  }
  
	sprintf(sql,"update operfile set op_flag='C' where operator=%d",da.operator);
	mysql_query(conn,sql);
	  
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"修改操作员文件错!%s",mysql_error(conn));
	  dis_str(22,22,sys_err_str,60,0);
	  getch();
	  return(-1);
	}
	
  clear_l22();
  dis_err("处理完毕,按任一键返回主菜单.");
  return(0);
}

