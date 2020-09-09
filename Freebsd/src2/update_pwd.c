/**************************************************************
  ������:update_pwd.ec
  ������:update_pwd()
  ��  ��:1714
  ��  ��:�޸ı�������
**************************************************************/              
#include<stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[96];
extern short sys_operator;    

extern MYSQL *conn;
extern char sql[];

update_pwd() 
{
  char pwdso[9],pwdsi[9],old_pwd[9];
  short op_no;

  static short in[4]={19,20,21,8};
  short num=4,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,22,"���޸ı������롷");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  op_no=sys_operator;
  if(strncmp(da.pwd_new1,da.pwd_new2,6)!=0)
  {
    dis_str(22,22,"�������������벻һ��!",0,0);
    getch();
    return(1);
  }
    
	sprintf(sql,"select password from operfile where operator=%d",op_no);
	if(mysql_query(conn,sql))
	{
    sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(1);
	}
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  if(row==NULL)
  {
    sprintf(sys_err_str,"����!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(1);
  }
  strcpy(old_pwd,row[0]);
  mysql_free_result(res);
  
  cry_pwd(da.pwd_old,pwdsi,sys_operator); 
  if(strncmp(old_pwd,pwdsi,8)!=0) 
  {
    dis_str(22,22,"ԭ���벻��!",0,0);
    getch();
    return(1);
  }
  cry_pwd(da.pwd_new1,pwdsi,op_no); 
  
	sprintf(sql,"update operfile set password='%s' where operator=%d",pwdsi,op_no);
	if(mysql_query(conn,sql))
	{
    sprintf(sys_err_str,"�����������%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(1);
	}
  dis_err("�������!");
  return(0);
}

