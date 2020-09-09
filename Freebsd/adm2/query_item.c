/****************************************************************
  程序名:query_item.ec 
  函数名:query_item() 
  功  能:查询科目号
****************************************************************/    
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

query_item()
{

  struct f_item it;
  long item_s,item_e;
  short kk,pp;

  static short in[2]={24,8};
  short num=2,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《查询科目号》");  
  refresh();
  da.item=999999;

  flag=input_data(num,in,&da);
  if (flag<0)
    return(0);

  clear_line(22,22,77);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

  if (da.item==999999)
  {
    item_s=100000;
    item_e=999999;
  }
  else
  {
    item_s=da.item;
    item_e=da.item;
  }
  
  /*************************/
  
  sprintf(sql,"SELECT * FROM item_file WHERE item_num>=%ld AND item_num <=%ld",item_s,item_e);
  
  mysql_query(conn,sql);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  res=mysql_store_result(conn);
  
  /*************************/
  
  clear_scr(6,22,22,77);
  mvprintw(7,12,"科目号   科  目  名         储种 性质 状态 统计标志 序号");
  mvprintw(6,30,"====   第 1 页   ====");
  refresh();
  kk=9;
  pp=1;
  
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
    {
      sprintf(sys_err_str,"取科目错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      mysql_free_result(res);
      return -1;
     }
	  set_f_item(&it,row);
	
    mvprintw(kk,12,"%6ld",it.item_num);
    mvprintw(kk,19,"%20s",it.item_name);
    mvprintw(kk,42,"%1s",it.type);
    mvprintw(kk,47,"%1s",it.acc_type);
    refresh();
    mvprintw(kk,52,"%1s",it.item_flag);
    mvprintw(kk,57,"%6ld",it.sum_flag);
    mvprintw(kk,65,"%2d",it.prt_ser);
    refresh();
    kk=kk+1;
    if (kk==19)
    {
      kk=9;
      pp++;
      dis_str(22,22,"按任一键,查询下一页!",0,0);
      getch();
      clear_scr(6,22,22,77);
      mvprintw(6,30,"====   第 %d 页   ====",pp);
      mvprintw(7,12,"科目号   科  目  名         储种 性质 状态 统计标志 序号");
      refresh();
    }
  }
  mysql_free_result(res);
  
  dis_err("查询完毕!");
  return(0);
}

