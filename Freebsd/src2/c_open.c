/****************************************************************
程序名:c_open.ec
函数名:c_open()
功  能:卡开户
代  码:1501
流  程:  
      1.输入
        户名 (customer)
        地址 (address)
        金额 (amount) 
        利息积数(interest) 
        帐户性质(acc_type P个人 G工资)
      2.检查本卡号是否存在.
      3.检查起存额
      4.根据录入信息取本户科目号/利率/流水号
      5.为卡分户/日志表有关项赋值
      6.写卡分户/日志表
      7.返回流水号
      8.写明细
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

char qq;

c_open()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  short flag_k,ymd[3];
  char a_flag[2];
  static short in[9]={66,16,28,2,63,64,72,73,8};
  short num=9,flag;
  long interest;
  long account;
  char str2[100];
  char yy;
  short mday[3];
  short chk;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;

  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  strcpy(da.acc_type,"1");

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《卡开户》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)return(0);

  init_s(&ss);
  init_t(&tt);

  note("检查本户是否存在!");

  da.open_br=100;
  
  note("卡号开户起存额为1分!");
  if (da.amount < 1)
  {
    dis_err("开户起存额小于1元!");
    return(0);
  }

  da.open_d=sys_work_d;
  rjulmdy(da.open_d,ymd);
  interest=s_int(ymd,da.amount);
  tt.operator=sys_operator;
  
  note("为卡号分户赋值!");
  ss.open_br=da.open_br;
  strcpy(ss.customer,da.customer);
  strcpy(ss.address,da.address);
  strcpy(ss.zj_kind,da.zj_kind);
  strcpy(ss.zj_num,da.zj_num);
  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';
  strcpy(ss.passwd,da.pwd_new2);
  ss.balance=da.amount;
  ss.acc_open_d=da.open_d;	
  rjulmdy(ss.acc_open_d,mday);
  ss.interest=s_int(mday,ss.balance);
  ss.det_num=1;
  ss.last_day=da.open_d;
  ss.card_open_d=da.open_d;	
  strcpy(ss.acc_type,"K");
  strcpy(ss.card_type,"K");
  strcpy(ss.acc_flag,"A");
  ss.open_op=sys_operator;

  if((ss.item_num = get_item('H', ss.acc_type[0],0)) < 0)
  {
    sprintf(sys_err_str,"取本户科目号错!code=%ld.",ss.item_num);
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(0);
  }
  note("为日志赋值!");
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_op=da.tr_op;
  tt.tr_code=1501;
  tt.amount=da.amount;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"H");
  tt.open_br=da.open_br;
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,da.customer);
  strcpy(tt.abstract,"0");
  tt.open_d=da.open_d;
  tt.balance=da.amount;
  tt.interest=ss.interest;
  tt.item_num=ss.item_num;
  tt.det_num=1;

/***********************************/

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }
  mvprintw(7,30,"开户卡号:[%s]",da.card_acc);
  refresh();
	
  strcpy(ss.card_acc,da.card_acc);
  strcpy(tt.card_acc,da.card_acc);
  ss.account=0;
  tt.account=0;
  
  note("取流水号!");
  tt.ser_num = get_ser(da.open_br);
  if (tt.ser_num < 0)
  {
    sprintf(sys_err_str,"取日志号错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  note("写卡分户!");
  
  set_ss_mast_i_sql(&ss,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写卡文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
 
  note("写日志!");
  
  set_s_trade_i_sql(&tt,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写日志文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  note("写卡明细!");
  da.tr_op=sys_operator;
  
  sprintf(sql,"insert into s_detail values(%d,0,'%s',%d,%ld,%ld,'0','O',%d,%d,%ld)",
    da.open_br,da.card_acc,ss.det_num,ss.last_day,da.amount,da.tr_br,da.tr_op,da.amount);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写卡明细文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  if (update_vou(da.open_br, "K0", 'O')<0)
  {
    sprintf(sys_err_str,"修改开户凭证错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mvprintw(22,20,"请在读卡器指示灯亮后划卡写磁条!");
  refresh();
  sprintf(str2,"%19s'%09ld'%05d",ss.card_acc, ss.balance, ss.det_num);
  
  if(write_msr(str2,1)!=0) {
    dis_str(22,22,"写磁条错!",60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mysql_commit(conn);
  mvprintw(1,68,"流水号:%d",tt.ser_num);
  refresh();
  
  mvprintw(22,20,"请插入存款凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1501,1);
  mvprintw(22,20,"请再插入存款凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1501,2);
  	
  return(0);
}
