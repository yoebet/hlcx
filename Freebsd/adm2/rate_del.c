/**************************************************************
程序名:rate_del.ec
函数名:rate_del()
功  能:利率表
**************************************************************/              
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];


rate_del()
{

  struct f_rate rat;

  static short in[4]={50,30,12,8};
  short num=4,flag;
  struct in_data da;

  long cnt,rate,rdateinf;
  char yy;
  short  op_no;
  char type[2];
  long   day;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《删除利率》");  
    refresh();
    da.k_date=sys_today;
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);
    dis_str(22,22,"正在处理,请稍侯......",0,0);  

    if (da.rate_type[0]=='1')
      strcpy(type,"H");
    else if (da.rate_type[0]=='2')
      strcpy(type,"D");
	  
	  /*
    $select * from ratefile
      where ltype=$type and term=$da.term and int_date=$da.k_date;
    if(sqlca.sqlcode!=0)
    {
      sprintf(sys_err_str,"取本利率错!code=%d",SQLCODE);
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	*/
	
	sprintf(sql,"DELETE FROM ratefile WHERE ltype='%s' AND term=%d AND int_date=%ld",type,da.term,da.k_date);
	
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"删除本利率错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
    dis_err("处理完毕! ");  
  return(0);
}
