/****************************************************************
  程序名:add_item.ec 
  函数名:add_item() 
  功  能:增加科目号
****************************************************************/              
#include<stdio.h>
#include<string.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

add_item()
{

  short br_no;
  short kind;
  static short in[8]={24,13,4,32,30,46,47,8};
  short num=8,flag;
  struct in_data da;
  char   item_name[21];
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《增加科目号》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

/***********************************/
	
	sprintf(sql,"SELECT item_name FROM item_file WHERE item_num=%ld",da.item);
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
    sprintf(sys_err_str,"已有此科目,名称为[%s]",row[0]);
    dis_err(sys_err_str);
    mysql_free_result(res);
    return(-1);
	}

/***********************************/

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }
  
  dis_str(22,22,"正在写科目文件...",60,0);
  if (da.ltype[0]=='1')
    strcpy(da.ltype,"F");
  else if (da.ltype[0]=='2')
    strcpy(da.ltype,"S");
  else if (da.ltype[0]=='3')
    strcpy(da.ltype,"P");
  else if (da.ltype[0]=='4')
    strcpy(da.ltype,"M");
  
	sprintf(sql,"insert into item_file values('%s','%s',%ld,%ld,'%s','A',%ld,%d)",
	  da.ltype,da.acc_type,da.term,da.item,da.item_name,da.sum_flag,da.prt_ser);
		
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写科目文件错!%s",mysql_error(conn));
    dis_err(sys_err_str);
    mysql_rollback(conn);
    return(-1);
  }
  
  dis_str(22,22,"正在写总帐...",60,0);
  
  sprintf(sql,"SELECT branch FROM br_file WHERE br_flag='A'");
  
  mysql_query(conn,sql);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"%s.",mysql_error(conn));
    dis_err(sys_err_str);
    mysql_rollback(conn);
    return(-1);
  }

  res=mysql_store_result(conn);
  
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
  	br_no=atoi(row[0]);
    
    kind=0;
    while(kind<100)
    {
    	sprintf(sql,"insert into sumtable values(%d,%d,%ld,'A',0,0,0,0,0,0,0,0)",
	      br_no,kind,da.item);
		
      if(mysql_query(conn,sql)||(mysql_errno(conn)))
      {
        sprintf(sys_err_str,"写总帐文件错!%s",mysql_error(conn));
        dis_err(sys_err_str);
        mysql_rollback(conn);
        return(-1);
      }
      kind++;
      if(kind==13)
        kind=90;
      if(kind==92)
        kind=99;
    }
  }
  mysql_free_result(res);
  
  mysql_commit(conn);
  
  clear_l22();
  dis_err("增加完毕,按任一键返回!");
  return(0);
}
