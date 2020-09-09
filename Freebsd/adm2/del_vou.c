/****************************************************************
  程序名:del_vou.ec 
  函数名:del_vou() 
  功  能:删除凭证号
****************************************************************/              
#include<stdio.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

del_vou()
{

  short br_no;
  short kind;
  static short in[2]={49,8};
  short num=2,flag;
  struct in_data da;
  char   vou_name[11],vou_flag[2];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《删除凭证号》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

	sprintf(sql,"SELECT vou_name,flag FROM vou_file WHERE vou_type='%s'",da.vou_code);
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    sprintf(sys_err_str,"无此凭证!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}
	strcpy(vou_name,row[0]);
	strcpy(vou_flag,row[1]);
  mysql_free_result(res);
  
  mvprintw(9,30,"凭证名称:[%s]",vou_name);
  refresh();
  if (vou_flag[0]!='A')
  {
    dis_str(22,22,"本凭证已作废!",60,0);
    getch();
    return(-1);
  }

  dis_str(22,22,"正在修改凭证文件...",60,0);
  
  sprintf(sql,"update vou_file set flag='K' where vou_type='%s'",da.vou_code);
	
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"修改凭证文件错!%s",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }
 
  clear_l22();
  dis_err("删除完毕,按任一键返回!");
  return(0);
}
