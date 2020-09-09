/**************************************************************
������:rate_query.ec
������:rate_query()
��  ��:���ʱ��ѯ 
**************************************************************/
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

MYSQL_RES *res;
MYSQL_ROW row;

rate_query()
{

  struct f_rate rat;
  long item_s,item_e;
  short pp,line;
  char f_name[5],str[11];
  char type[2];
  short term1,term2;

  static short in[2]={50,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����ѯ���ʡ�");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  
  switch(da.rate_type[0])
  {
    case '2':
      strcpy(type,"D");
      strcpy(f_name,"����");
      break;
    case '1':
      strcpy(type,"H");
      strcpy(f_name,"����");
      break;
    default:
      dis_str(22,22,"��������ѡ���!",0,0);
      getch();
      return(-1);
      break;
  }

  da.term=99;
  if (da.term==99)
  {
    term1=0;
    term2=99;
  }
  else
  {
    term1=da.term;
    term2=da.term;
  }
  
  sprintf(sql,"SELECT * FROM ratefile WHERE ltype='%s' AND term>=%d AND term <=%d ORDER BY ltype,term,int_date",type,term1,term2);
  
  mysql_query(conn,sql);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  res=mysql_store_result(conn);
  
  mvprintw( 8,25,"����:[%s]  ���ʱ�     ��1ҳ",f_name);
  mvprintw( 9,25,"������������������������������");
  mvprintw(10,25,"����      ��Ϣ��       ������");
  pp=1;
  line=11;
  
  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
	  set_f_rate(&rat,row);
	
    rfmtdate(rat.int_date,"yyyy/mm/dd",str);
    mvprintw(line,25,"%2d     %10s     %7.4lf",rat.term,str,rat.rate/10000.00);
    refresh();
    line++;
    if (line==21)
    {
      line=10;
      pp++;
      dis_str(22,22,"����һ��,��ѯ��һҳ!",0,0);
      getch();
      clear_scr(6,22,22,77);
	    mvprintw( 8,25,"����:[%s]  ���ʱ�     ��1ҳ",f_name);
	    mvprintw( 9,25,"������������������������������");
	    mvprintw(10,25,"����      ��Ϣ��       ������");
      refresh();
    }
  }
  mysql_free_result(res);
  
  clear_l22();
  dis_str(22,22,"����һ������.",0,0);
  getch();
  return(0);
}
