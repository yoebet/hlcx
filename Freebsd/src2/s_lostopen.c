/****************************************************************
程序名:s_lostopenopen.ec
函数名:s_lostopen()
功  能:折挂失补开
代  码:1511
流  程:  
      1.输入
        卡号 (card_acc)
        
      2.取指定卡号的分户
      3.检查分户状态及上述录入各项
      4.修改分户状态
      5.写特殊操作表
      6.写日志
           
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[96];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

s_lostopen()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  static short in[2]={1,8};
  short num=2,flag;
  short br,op,op_no;
  long day;
  char qq;
  char t_str[7];
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  strcpy(da.acc_type,"2");
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,22,"《存折帐户挂失补开》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  br=sys_branch;
  op=sys_operator;

  note("取对应分户记录!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE card_acc='%s' and open_br=%d",da.account,da.open_br);
	mysql_query(conn,sql);
	
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"原存折帐户不存在!",0,0);
    getch();
    continue;
  }

  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"对应分户非正式户!",0,0);
    getch();
    continue;
  }

  mvprintw(7+num,30,"户    名:[%12s]",ss.customer);
  mvprintw(8+num,30,"地    址:[%30s]",ss.address);
  mvprintw(9+num,30,"证件号码:[%s]",ss.zj_num);

  dis_str(22,22,"请核对户名/地址后,按 q 退出,其它键继续!",0,0);
	refresh();
  qq=getch();
  if (qq=='q')
    continue;
  
  dis_str(22,22,"                                       ",0,0);

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }

  if((da.account1=get_acc(da.open_br,'H'))<0)
   {
    sprintf(sys_err_str,"取开户帐号错!");
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
   }
   chk=get_chk_no(da.open_br,da.account1);
   if (chk<0)
   {
     dis_err("计算校验位错!");  
     mysql_rollback(conn);
  	 return(0);
   }
  mvprintw(7,30,"新开帐号:[%ld_%1d]",da.account1,chk);
  refresh();
  
  note("为活期分户修改项赋值!");
  
	sprintf(sql,"update s_mast set acc_flag='C',transfer=%ld WHERE card_acc='%s' and open_br=%d",
		da.account1,da.card_acc,da.open_br);

  if (mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"修改帐户错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("为日志赋值!");

  tt.account=da.account1;
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1511;
  tt.amount=da.amount;
  strcpy(tt.type,"H");
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc1);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  tt.open_d=ss.acc_open_d;
  tt.balance=ss.balance;
  tt.interest=ss.interest;
  tt.item_num=ss.item_num;
  tt.det_num=1;
  note("取流水号!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"取日志号错!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
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
  ss.account=da.account1;
  ss.det_num=1;

  ss.transfer= da.account;
  strcpy(ss.card_acc,da.card_acc1);
  strcpy(ss.acc_flag,"A");
  ss.open_op=sys_operator;
  ss.acc_close_d=0;
  ss.close_br=0;
  ss.close_op=0;
  note("写分户!");
  
  set_ss_mast_i_sql(&ss,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写卡文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  note("写活期明细!");
  da.tr_op=sys_operator;
  
  sprintf(sql,"insert into s_detail values(%d,%ld,0,%d,%ld,%ld,'0','O',%d,%d,%ld)",
    ss.open_br,da.account1,ss.det_num,ss.last_day,ss.balance,ss.tr_br,dd.tr_op,da.amount);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写活期明细文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  if (update_vou(da.open_br, "H0", 'O')<0)
  {
    sprintf(sys_err_str,"修改开户凭证错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }



  mysql_commit(conn);
 
  mvprintw(1,68,"流水号:%d",tt.ser_num);
  mvprintw(22,22,"请在读卡器指示灯亮后划存折写磁条!");
  refresh();
  sprintf(str2,"%09ld'%09ld'%05d",ss.account, ss.balance, ss.det_num);
  write_msr(str2);

  mvprintw(22,22,"请插入开户凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  {
  	prt_h_t(&tt,1501,1);
  }
  mvprintw(22,22,"请再插入开户凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  {
  	prt_h_t(&tt,1501,2);
  }
  mvprintw(22,22,"请插入存折后按任一键,q退出!        ");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
    prt_h_z(&tt);


  break;
}
  return(0);
}
