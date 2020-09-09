/**************************************************************
������:rate_add.ec
������:rate_add()
��  ��:���ʱ����ӻ��޸�
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

rate_add()
{

  struct f_rate rat;

  static short in[5]={50,30,12,51,8};
  short num=5,flag;
  struct in_data da;

  long cnt,rate,rdateinf;
  char yy;
  char type[2];
  short  op_no;
  long   day;
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"�����ӻ��޸����ʡ�");  
    refresh();
    da.k_date=sys_today;

    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

    if (da.rate_type[0]=='1')
      strcpy(type,"H");
    else if (da.rate_type[0]=='2')
      strcpy(type,"D");
	  
	 /*************************************/
	  
	  sprintf(sql,"SELECT count(*) FROM ratefile WHERE ltype='%s' AND term=%d",type,da.term);
	  
	  mysql_query(conn,sql);
	  
	  if(mysql_errno(conn))
	  {
		sprintf(sys_err_str,"ȡ���ʼ�¼��,%s.",mysql_error(conn));
		dis_str(22,22,sys_err_str,60,0);
		getch();
		return(-1);
	  }

	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  cnt=atol(row[0]);
	  
	  /***************************************/
	  
    if(cnt==0)
    {
      sprintf(sys_err_str,"�޴˴��ּ�¼!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
	  sprintf(sql,"SELECT count(*) FROM ratefile WHERE ltype='%s' AND term=%d AND int_date=%ld",type,da.term,da.k_date);
	  
	  mysql_query(conn,sql);
	  
	  if(mysql_errno(conn))
	  {
		sprintf(sys_err_str,"ȡ���ʼ�¼��,%s.",mysql_error(conn));
		dis_str(22,22,sys_err_str,60,0);
		getch();
		return(-1);
	  }

	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  cnt=atol(row[0]);
	  
    if(cnt==0)
	  sprintf(sql,"INSERT INTO ratefile VALUES('%s',%d,%ld,%ld)",type,da.term,da.k_date,da.rate);
    else
	  sprintf(sql,"UPDATE ratefile SET rate=%ld WHERE ltype='%s' AND term=%d AND int_date=%ld",type,da.term,da.k_date);
		
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"�޸������ļ���!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    dis_err("�������! ");  
  return(0);
}
