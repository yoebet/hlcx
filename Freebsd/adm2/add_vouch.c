/****************************************************************
  ������:add_vou.ec 
  ������:add_vou() 
  ��  ��:����ƾ֤����
****************************************************************/              
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern MYSQL *conn;
extern char sql[];

add_vou()
{

  short br_no;
  short kind;
  static short in[6]={49,48,4,46,47,8};
  short num=6,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  char  vou_name[11];

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"������ƾ֤���͡�");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڲ�ѯ���޴�ƾ֤...",60,0);
  
	sprintf(sql,"SELECT vou_name FROM vou_file WHERE vou_type='%s'",da.vou_code);
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
    sprintf(sys_err_str,"���д�ƾ֤,����Ϊ[%s]",row[0]);
    dis_err(sys_err_str);
    mysql_free_result(res);
    return(-1);
	}
/***********************************/

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }
  
  dis_str(22,22,"����дƾ֤�ļ�...",60,0);
  
	sprintf(sql,"insert into vou_file values('%s','%s','A','%s',%ld,%d)",
	  da.vou_code,da.vou_name,da.ltype,da.sum_flag,da.prt_ser);
		
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"дƾ֤�ļ���!%s",mysql_error(conn));
    dis_err(sys_err_str);
    mysql_rollback(conn);
    return(-1);
  }
  
  dis_str(22,22,"����д����ƾ֤����...",60,0);
  
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
    	sprintf(sql,"insert into vouch values(%d,%d,'A','%s',0,0,0,0,0)",
	      br_no,kind,da.vou_code);
		
      if(mysql_query(conn,sql)||(mysql_errno(conn)))
      {
        sprintf(sys_err_str,"дƾ֤�����ļ���!%s",mysql_error(conn));
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
  dis_err("�������,����һ������!");
  return(0);
}
