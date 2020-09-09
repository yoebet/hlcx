/**************************************************************
程序名:rate_query.ec
函数名:rate_query()
功  能:利率表查询 
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
  mvaddstr(5,30,"《查询利率》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  
  switch(da.rate_type[0])
  {
    case '2':
      strcpy(type,"D");
      strcpy(f_name,"定期");
      break;
    case '1':
      strcpy(type,"H");
      strcpy(f_name,"活期");
      break;
    default:
      dis_str(22,22,"利率类型选择错!",0,0);
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
  
  mvprintw( 8,25,"储种:[%s]  利率表     第1页",f_name);
  mvprintw( 9,25,"━━━━━━━━━━━━━━━");
  mvprintw(10,25,"期限      调息日       年利率");
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
      dis_str(22,22,"按任一键,查询下一页!",0,0);
      getch();
      clear_scr(6,22,22,77);
	    mvprintw( 8,25,"储种:[%s]  利率表     第1页",f_name);
	    mvprintw( 9,25,"━━━━━━━━━━━━━━━");
	    mvprintw(10,25,"期限      调息日       年利率");
      refresh();
    }
  }
  mysql_free_result(res);
  
  clear_l22();
  dis_str(22,22,"按任一键返回.",0,0);
  getch();
  return(0);
}
