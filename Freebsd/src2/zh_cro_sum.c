/****************************************************************
程序名:zh_cro_sum.ec
函数名:zh_cro_sum()
功  能:调整总帐冲正
代  码:1981
流  程:  
      1.输入
        科目号 (item)
        发生额(amount) 
        发生笔数(sum_que) 
        户数(home_que) 
        交易号 (ser_num) 
      2.取指定交易号的流水
      3.检查流水状态及上述录入各项
      4.修改流水为作废
      5.写特殊操作表
           
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

zh_cro_sum()
{
  struct f_sumtable sum;
  struct s_trade tt;
  struct in_data da;
  char yy;
  static short in[6]={24,2,38,40,11,8};
  short num=6,flag;
  char item_flag[2];
  short br,op_no,op;
  long day;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  strcpy(da.acc_type,"M");
  da.tr_br=sys_branch;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《总帐调整冲正》");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    init_t(&tt);

    note("检查指定交易号是否存在!");
    br=da.open_br;
    op=sys_operator;
    
	  sprintf(sql,"select * from trade where branch=%d and account=%ld and operator=%d and ser_num=%d",
	    da.open_br,da.account,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"取本交易错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      dis_str(22,22,"本交易不存在!",0,0);
      getch();
      continue;
	  }
	  set_s_trade(&tt,row);
    mysql_free_result(res);
	  
    note("检查流水中有关项是否与录入一致!");
    if (tt.correct[0]!='A')
    {
      dis_str(22,22,"流水状态错!",0,0);
      getch();
      continue;
    }
    if (tt.tr_code!=1909)
    {
      dis_str(22,22,"与流水中操作码不符!",0,0);
      getch();
      continue;
    }
    if (tt.amount!=da.amount)
    {
      dis_str(22,22,"与流水中发生额不符!",0,0);
      getch();
      continue;
    }
    if (tt.term!=da.sum_que)
    {
      dis_str(22,22,"与流水中业务笔数不符!",0,0);
      getch();
      continue;
    }

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"不能启动事务");
      dis_err(sys_err_str);
      return(-1);
    }

    note("修改原流水为冲正!");
    
	  sprintf(sql,"update trade set correct='K' where branch=%d and operator=%d and ser_num=%d",
	    da.open_br,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"修改本交易错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      continue;
	  }

    note("修改总帐!");
    da.amount=tt.amount/100.00;
    
    if (da.sum_flag==2)
    {
      note("修改总帐,贷方!");
	    sprintf(sql,"update sumtable set det_amt=det_amt-%ld,dvou_que=dvou_que-%d,\
	      balance=balance-%ld,rest_que=rest_que-%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    else
    {
      note("修改总帐,借方!");
	    sprintf(sql,"update sumtable set cre_amt=cre_amt-%ld,cvou_que=cvou_que-%d,\
	      balance=balance+%ld,rest_que=rest_que+%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"修改总帐错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    mysql_commit(conn);
    
    mvprintw(1,68,"流水号:%d",tt.ser_num);
    refresh();
    dis_err("处理完毕! ");  
      break;
  }
  return(0);
}
