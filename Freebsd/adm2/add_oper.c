/****************************************************************
  ������:add_oper.ec 
  ������:add_oper() 
  ��  ��:��������Ա
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
  mvaddstr(5,30,"����������Ա��");  
  refresh();

  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

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
    dis_str(22,22,"������Ա���Ѵ���,��������!",0,0);
    mysql_free_result(res);
    getch();
    return(-1);
	}
	
  cry_pwd("000000",init_pwd,da.operator);
	sprintf(sql,"insert into operfile values(%d,'%s','7','%s','A','��',0,0)",
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
  dis_err("�������,��ʼ����Ϊ0,��֪ͨ�����޸�����!");
  return(0);
}
