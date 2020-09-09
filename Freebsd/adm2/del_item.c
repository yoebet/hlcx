/****************************************************************
  程序名:del_item.ec 
  函数名:del_item() 
  功  能:删除科目号
  代  码:2403
  流  程:
         1.输入科目号(000000退出)
         2.从科目文件item_file中检查有无本科目
         3.科目文件item_file中置本科目为作废
****************************************************************/              
#include<stdio.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

del_item()
{

  short br_no;
  short kind;
  static short in[2]={24,8};
  short num=2,flag;
  struct in_data da;
  char   item_name[21],item_flag[2];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《删除科目号》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  
  
	sprintf(sql,"SELECT item_name,item_flag FROM item_file WHERE item_num='%ld'",da.item);
	mysql_query(conn,sql);
	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL)
	{
    sprintf(sys_err_str,"无此科目!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}
	strcpy(item_name,row[0]);
	strcpy(item_flag,row[1]);
  mysql_free_result(res);

  mvprintw(7+num,30,"科目名称:[%s]",item_name);
  refresh();
  if (item_flag[0]!='A')
  {
    dis_str(22,22,"本科目已作废!",60,0);
    getch();
    return(-1);
  }

  dis_str(22,22,"正在修改科目文件...",60,0);
  
  sprintf(sql,"update item_file set item_flag='K' where item_num=%ld",da.item);
	
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"修改科目文件错!%s",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }
 
  clear_l22();
  dis_err("删除完毕,按任一键返回!");
  return(0);
}
