/****************************************************************
  程序名:update_vouch.ec 
  函数名:update_vouch() 
  功  能:修改凭证内容
****************************************************************/              
#include<stdio.h>
#include<string.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern short sys_operator;    
extern long sys_today;
extern MYSQL *conn;
extern char sql[];

update_vouch()
{

  short kind;
  static short in0[2]={49,8};
  static short in1[5]={48,4,46,47,8};
  short num0=2,num1=5,flag;
  struct in_data da;
  struct f_voufile vou;
  long day;
  short op_no;
  MYSQL_RES *res;
  MYSQL_ROW row;
   
  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《修改凭证内容》");  
  refresh();
  flag=input_data(num0,in0,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

	sprintf(sql,"SELECT * FROM vou_file WHERE vou_type='%s'",da.vou_code);
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
	set_f_voufile(&vou,row);
  mysql_free_result(res);
  
  if (vou.flag[0]!='A')
  {
    sprintf(sys_err_str,"本凭证已作废!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  strcpy(da.ltype,vou.type);
  strcpy(da.vou_name,vou.vou_name);
  da.sum_flag=vou.sum_flag;
  da.prt_ser=vou.prt_ser;
  flag=input_data(num1,in1,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在修改凭证文件...",60,0);
  
  sprintf(sql,"update vou_file set vtype='%s',vou_name='%s',sum_flag=%ld,prt_ser=%d where vou_type='%s'",
    da.ltype,da.vou_name,da.sum_flag,da.prt_ser,da.vou_code);
	
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"修改凭证文件错,%s",sql);
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  
clear_l22();
  dis_err("修改完毕!",0,0);
  return(0);
}
