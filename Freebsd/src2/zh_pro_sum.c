/****************************************************************
程序名:zh_pro_sum.ec
函数名:zh_pro_sum()
功  能:调整总帐
代  码:1909
流  程:  
      1.输入
        科目号 (item)
        发生额(amount) 
        发生笔数(sum_que) 
        户数(home_que) 
        调整方向 (sum_flag) 
        摘要 (abstract) 
        操作员号(open_op)
        业务所号 (tr_br 回车取登录所号)
      2.检查科目是否存在
      3.写日志
      4.写总帐
           
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

zh_pro_sum()
{
  struct f_sumtable sum;
  struct s_trade tt;
  struct in_data da;
  char yy;
  static short in[6]={24,2,38,40,36,8};
  short num=6,flag;
  char item_flag[2];
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  strcpy(da.acc_type,"M");
  da.tr_br=sys_branch;
  da.tr_op=sys_operator;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《总帐调整》");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    note("检查科目号是否存在!");
    
	  sprintf(sql,"select flag from sumtable where branch=%d and item_num=%ld and kind=90",da.open_br,da.item);
	  mysql_query(conn,sql);
	  
	  if(mysql_errno(conn))
	  {
      sprintf(sys_err_str,"取科目号错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }

	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      sprintf(sys_err_str,"无此科目!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  strcpy(item_flag,row[0]);
    mysql_free_result(res);
    
    if (item_flag[0]=='K')
    {
      dis_str(22,22,"本科目已作废",60,0);
      getch();
      continue;
    }
    init_t(&tt);
    note("为日志赋值!");
    tt.branch=da.tr_br;
    tt.operator=sys_operator;
    tt.tr_op=da.tr_op;
    tt.tr_code=1909;
    tt.amount=da.amount;
    strcpy(tt.correct,"A");
    strcpy(tt.type,"M");
    tt.open_br=da.open_br;
    tt.account=da.account;
    strcpy(tt.acc_type,da.acc_type);
    note("摘要:0现金 其它转帐");
    strcpy(tt.abstract,"0");
    tt.open_d=da.open_d;
    tt.balance=da.amount;
    tt.item_num=da.item;
    note("调整方向");
    tt.det_num=da.sum_flag;
    note("发生笔数");
    tt.term=da.sum_que;
    note("开/销户数");
    tt.transfer=da.home_que;

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"不能启动事务");
      dis_err(sys_err_str);
      return(-1);
    }
    note("取流水号!");
    if ((tt.ser_num = get_ser(da.open_br)) < 0)
    {
      sprintf(sys_err_str,"取日志号错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
    note("写日志!");
    set_s_trade_i_sql(&tt,sql);
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"写日志文件错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    
    if (da.sum_flag==2)
    {
      note("修改总帐,贷方!");
	    sprintf(sql,"update sumtable set det_amt=det_amt+%ld,dvou_que=dvou_que+%d,\
	      balance=balance+%ld,rest_que=rest_que+%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    else
    {
      note("修改总帐,借方!");
	    sprintf(sql,"update sumtable set cre_amt=cre_amt+%ld,cvou_que=cvou_que+%d,\
	      balance=balance-%ld,rest_que=rest_que-%d\
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
