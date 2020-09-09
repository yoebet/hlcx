/****************************************************************
  程序名:query_vou.ec 
  函数名:query_vou() 
  功  能:查询凭证号
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

query_vou()
{

  struct f_voufile vou;
  short kk,pp;

  static short in[2]={49,8};
  short num=2,flag;
  struct in_data da;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《查询凭证号》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

  if (atoi(da.vou_code)!=99)
  {
    sprintf(sql,"select * from vou_file where vou_type='%s'",da.vou_code);
  } else
  {
    sprintf(sql,"select * from vou_file");
  }
  
  mysql_query(conn,sql);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"打开凭证文件错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  res=mysql_store_result(conn);
  
  clear_scr(6,22,22,77);
  mvprintw(8,30,"====   第 1 页   ====");
  mvprintw(9,12,"凭证类型    凭证名   储种  状态   统计标志   序号");
  refresh();
  kk=10;
  pp=1;
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
    {
      sprintf(sys_err_str,"取凭证错!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      mysql_free_result(res);
      return -1;
     }
	  set_f_voufile(&vou,row);
    mvprintw(kk,16,"%2s",vou.vou_type);
    mvprintw(kk,24,"%10s",vou.vou_name);
    mvprintw(kk,35,"%1s",vou.type);
    mvprintw(kk,41,"%1s",vou.flag);
    mvprintw(kk,48,"%6ld",vou.sum_flag);
    mvprintw(kk,57,"%3d",vou.prt_ser);
    refresh();
    kk=kk+1;
    if (kk==19)
    {
      kk=9;
      pp++;
      dis_str(21,22,"按任一键,查询下一页!",0,0);
      getch();
      clear_scr(6,22,22,77);
      mvprintw(8,30,"====   第 %d 页   ====",pp);
      mvprintw(9,12,"凭证类型    凭证名   储种  状态   统计标志   序号");
      refresh();
    }
  }
  mysql_free_result(res);
  clear_l22();
  dis_err("查询完毕!");
  return(0);
}

