/****************************************************************
  ������:query_vou.ec 
  ������:query_vou() 
  ��  ��:��ѯƾ֤��
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
  mvaddstr(5,30,"����ѯƾ֤�š�");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

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
    sprintf(sys_err_str,"��ƾ֤�ļ���!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }
  res=mysql_store_result(conn);
  
  clear_scr(6,22,22,77);
  mvprintw(8,30,"====   �� 1 ҳ   ====");
  mvprintw(9,12,"ƾ֤����    ƾ֤��   ����  ״̬   ͳ�Ʊ�־   ���");
  refresh();
  kk=10;
  pp=1;
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
    {
      sprintf(sys_err_str,"ȡƾ֤��!%s.",mysql_error(conn));
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
      dis_str(21,22,"����һ��,��ѯ��һҳ!",0,0);
      getch();
      clear_scr(6,22,22,77);
      mvprintw(8,30,"====   �� %d ҳ   ====",pp);
      mvprintw(9,12,"ƾ֤����    ƾ֤��   ����  ״̬   ͳ�Ʊ�־   ���");
      refresh();
    }
  }
  mysql_free_result(res);
  clear_l22();
  dis_err("��ѯ���!");
  return(0);
}

