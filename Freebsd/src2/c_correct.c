/****************************************************************
程序名:s_correct.ec
函数名:s_correct()
功  能:卡本日冲正
代  码:1581
流  程:  
      1.输入
        帐号 (card_acc)
        金额 (amount) 
        交易号(ser_num)
      2.取指定交易号的日志
      3.检查日志状态及上述录入各项
      4.取本帐号对应分户,并检查分户中的上述各项
      5.根据原交易码修改分户
      6.写特殊操作表
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

struct s_trade tt;

c_correct()
{
  struct ss_mast ss;
  struct in_data da;
  static short in[4]={66,2,11,8};
  short num=4,flag;
  short br,op,op_no;
  long day;
  char yy,qq;
  short mday[3];
  char str2[50];
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  strcpy(da.acc_type,"P");
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《卡业务本日冲正》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  note("检查指定交易号是否存在!");
  br=da.open_br;
  op=sys_operator;
  
  sprintf(sql,"SELECT * FROM trade WHERE branch=%d AND card_acc='%s' AND operator=%d AND ser_num=%d",
    br,da.card_acc,op,da.ser_num);
    
	mysql_query(conn,sql);
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"取本交易错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"本交易不存在!",0,0);
    getch();
    continue;
  }
  set_s_trade(&tt,row);
  mysql_free_result(res);
  
  note("检查日志中有关项是否与录入一致!");
  if (tt.correct[0]!='A')
  {
    dis_str(22,22,"日志状态错!",0,0);
    getch();
    continue;
  }

  if (tt.amount!=da.amount)
  {
    dis_str(22,22,"与日志中金额不符!",0,0);
    getch();
    continue;
  }


  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }

  note("修改原日志为冲正!");
  
	sprintf(sql,"update trade set correct='K' where branch=%d and operator=%d and ser_num=%d",
	  br,op,da.ser_num);

  if (mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"修改本交易错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }

  note("取日志对应分户记录!");
  
  sprintf(sql,"SELECT * FROM s_mast WHERE card_acc='%s' AND open_br=%d",tt.card_acc,tt.open_br);
    
	mysql_query(conn,sql);
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"取本帐户错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"本帐号不存在!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]=='L')
  {
    dis_str(22,22,"对应分户已挂失!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  if (ss.acc_flag[0]=='K')
  {
    dis_str(22,22,"对应分户已冲正!",0,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  switch (tt.tr_code)
  {
    case 1501:
      note("开户冲正:将新开户分户状态置为 K");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"对应分户状态错!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }

      sprintf(sql,"delete FROM s_mast WHERE card_acc='%s' AND open_br=%d",tt.card_acc,tt.open_br);
    
	    mysql_query(conn,sql);
	    if(mysql_errno(conn))
	    {
	      sprintf(sys_err_str,"修改帐户错!%s",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
    	}
    	
      note("将新开户对应明细状态置为 K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      if (update_vou(tt.open_br, "K0", 'o')<0)
      {
        sprintf(sys_err_str,"修改开户凭证错!%s",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1502:
      note("销户冲正:将新销户分户状态置为 A");
      if (ss.acc_flag[0]!='C')
      {
        dis_str(22,22,"对应分户状态错!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      sprintf(sql,"update s_mast set acc_flag='A' where card_acc='%s' and open_br=%d",tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1503:
      note("续存冲正");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"对应分户状态错!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      rjulmdy(ss.last_day,mday);
      ss.balance=ss.balance-tt.amount;
      ss.interest=ss.interest-s_int(mday,tt.amount); 
      
      sprintf(sql,"update s_mast set balance=%ld,interest=%ld where card_acc='%s' and open_br=%d",
        ss.balance,ss.interest,tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      note("将对应明细状态置为 K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    case 1504:
      note("支取冲正");
      if (ss.acc_flag[0]!='A')
      {
        dis_str(22,22,"对应分户状态错!",0,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      rjulmdy(ss.last_day,mday);
      ss.balance=ss.balance+tt.amount;
      ss.interest=ss.interest+s_int(mday,tt.amount); 
      
      sprintf(sql,"update s_mast set balance=%ld,interest=%ld where card_acc='%s' and open_br=%d",
        ss.balance,ss.interest,tt.card_acc,tt.open_br);
      if (mysql_query(conn,sql))
      {
        sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      
      note("将对应明细状态置为 K");
      if(update_s_detail()!=0) 
      {
        sprintf(sys_err_str,"修改帐户错!");
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        continue;
      }
      break;
    default:
      dis_str(22,22,"本交易不能冲正!",0,0);
        mysql_rollback(conn);
      getch();
      continue;
  }
  mysql_commit(conn);
  mvprintw(22,22,"请在读卡器指示灯亮后划卡写磁条!");
  refresh();
 
  sprintf(str2,"%19s'%09ld'%05d",ss.card_acc, ss.balance, ss.det_num);
  write_msr(str2,1);
 
  mvprintw(22,22,"请插入原凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_ct();

  break;
}
  return(0);
}
