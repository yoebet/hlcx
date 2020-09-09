/****************************************************************
程序名:s_close.ec
函数名:s_close()
功  能:活期销户
代  码:1502
流  程:  
      1.输入
        帐号 (account)
      2.取本帐号的分户帐
      3.核对
        户名 (customer)
        地址 (address)
      4.检查分户中的上述各项
      5.计算利息,并核对
      6.取流水号
      7.修改分户
      8.写日志
      9.回显流水号
      A.写明细
           
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

s_close()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  struct s_tax  tax;
  char yy,qq;
  char str2[100];
  short s_day,ed[3];
  long interest;
   double int_d;
  static short in[3]={70,72,8};
  short num=3,flag;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.tr_br=sys_branch;
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《活期销户》");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);
  dis_str(22,22,"正在处理,请稍侯......",0,0);  

  init_s(&ss);
  init_t(&tt);

  note("检查本户是否存在!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE account=%ld and open_br=%d",da.account,da.open_br);
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
    dis_str(22,22,"本帐号不存在!",0,0);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"本帐户已非正常户!",0,0);
    getch();
    continue;
  }
  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';
  if(strcmp(ss.passwd,da.pwd_new2)!=0) 
  {
    dis_err("密码不符!");
    continue;
  }

  mvprintw(7+num,30,"户    名:[%12s]",ss.customer);
  mvprintw(8+num,30,"地    址:[%30s]",ss.address);
  mvprintw(9+num,30,"余    额:[%ld]",ss.balance/100);
 mvprintw(10+num,30,"证件号码:[%s]",ss.zj_num);

  dis_str(22,22,"请核对户名/地址后,按 q 退出,其它键继续!",0,0);
  refresh();
  yy=getch();
  if (yy=='q')
    continue;
  dis_str(22,22,"                                       ",0,0);
  
  note("计算销户利息!");
  rjulmdy(sys_work_d,ed);
  tt.amount=ss.balance;
  interest=s_int(ed,tt.amount);
  interest=ss.interest-interest;
  int_d=(double)(interest)*(double)(get_rate('H',0))/3600000;
  interest=(long)(int_d);
  tt.s_int=interest;
  tt.interest=ss.interest;
  ss.tax_int=0;
  tt.tax_int=ss.tax_int;
  tt.close_d=sys_work_d;
  tt.operator=sys_operator;

  da.amount=ss.balance;
  note("为活期分户修改项赋值!");
  ss.tax_int=da.tax_int;
  ss.acc_close_d=sys_work_d;	
  strcpy(ss.acc_flag,"C");
  ss.close_br=da.tr_br;
  ss.close_op=sys_operator;
  ss.det_num++;

  note("为日志赋值!");
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1502;
  tt.amount=ss.balance;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"H");
  tt.term=0;
  tt.open_br=da.open_br;
  tt.account=da.account;
  strcpy(tt.card_acc,"  ");
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  strcpy(tt.abstract,"0");
  tt.open_d=ss.acc_open_d;
  tt.close_d=sys_work_d;
  tt.balance=ss.balance;
  tt.item_num=ss.item_num;
  tt.det_num=ss.det_num;
  tt.transfer=0;

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"不能启动事务");
    dis_err(sys_err_str);
    return(-1);
  }

  note("取流水号!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"取日志号错!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
  }
  note("修改活期分户!");
  
  set_ss_mast_u_sql(&ss,sql);
  char cond[40];
	sprintf(cond," open_br=%d and account=%ld",ss.open_br,ss.account);
	strcat(sql,cond);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"修改活期文件错!%s",mysql_error(conn));
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
  
  note("写活期明细!");
  da.tr_op=sys_operator;
  
  
  sprintf(sql,"insert into s_detail values(%d,%ld,0,%d,%ld,%ld,'%s','C',%d,%d,%ld)",
    da.open_br,da.account,ss.det_num,ss.last_day,da.amount,da.abstract,da.tr_br,da.tr_op,ss.balance);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"写活期明细文件错!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }

  mysql_commit(conn);
  
  mvprintw(1,68,"流水号:%d",tt.ser_num);
  refresh();
  mvprintw(22,20,"请在读卡器指示灯亮后划存折写磁条!");
  refresh();
  sprintf(str2,"%09ld'%09ld'%05d",ss.account, ss.balance, ss.det_num);
  write_msr(str2,0);
  
  mvprintw(22,20,"请插入取款凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_t(&tt,1502,1);
  mvprintw(22,20,"请再插入取款凭条后按任一键,q退出!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_t(&tt,1502,2);
	
  mvprintw(22,20,"请插入存折后按任一键,q退出!          ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
    prt_h_cq(&tt,1502);
   
  /*
  mvprintw(22,20,"请插入利息凭条后按任一键,q退出!          ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
    prt_h_lx(&tt,1502,1);
  mvprintw(22,20,"请再插入利息凭条后按任一键,q退出!          ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
    prt_h_lx(&tt,1502,2);
  */

  break;
}

return(0);

}
