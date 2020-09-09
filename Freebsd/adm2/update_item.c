/****************************************************************
  程序名:update_item.ec 
  函数名:update_item() 
  功  能:修改科目号
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

update_item()
{

  short kind;
  static short in0[2]={24,8};
  static short in1[7]={13,4,32,30,46,47,8};
  short num0=2,num1=7,flag;
  struct in_data da;
  struct f_item it;
  long day;
  short op_no;
  MYSQL_RES *res;
  MYSQL_ROW row;
   
  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《修改科目内容》");  
  refresh();
  flag=input_data(num0,in0,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

	sprintf(sql,"SELECT * FROM item_file WHERE item_num=%ld",da.item);
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
	set_f_item(&it,row);
  mysql_free_result(res);

  if (it.item_flag[0]!='A')
  {
    dis_str(22,22,"本科目已作废!",60,0);
    getch();
    return(-1);
  }

  strcpy(da.ltype,it.type);
  strcpy(da.acc_type,it.acc_type);
  da.term=it.term;
  strcpy(da.item_name,it.item_name);
  da.sum_flag=it.sum_flag;
  da.prt_ser=it.prt_ser;
  if (it.type[0]=='H')
    strcpy(da.ltype,"1");
  else if (it.type[0]=='D')
    strcpy(da.ltype,"2");
  else if (it.type[0]=='K')
    strcpy(da.ltype,"3");

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《修改科目内容》");  
  refresh();
  flag=input_data(num1,in1,&da);
  if (flag<0)
    return(0);

  dis_str(22,22,"正在修改科目表文件...",60,0);
  
  sprintf(sql,"update item_file set ltype='%s',acc_type='%s',term=%ld,item_name='%s',sum_flag=%ld,prt_ser=%d where item_num=%ld",
    da.ltype,da.acc_type,da.term,da.item_name,da.sum_flag,da.prt_ser,da.item);
	
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"%s",sql);
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  
  clear_l22();
  dis_err("修改完毕!");
  return(0);
}
