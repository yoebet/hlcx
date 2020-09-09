/****************************************************************
������:s_chghb()
��  ��:����
��  ��:1592
           
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

s_chghb()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  char yy;
  short flag_k,s_day,ed[3],fd[3],sd[3];
  long s_rate,e_day,interest;
  char a_flag[2];
  char str2[100];
  static short in[4]={70,72,8};
  short num=3,flag;
	char qq;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
  
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"�����桷");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)
   return(0);

  init_s(&ss);
  init_t(&tt);

  note("��鱾���Ƿ����!");
  
  /*****************************/
  
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
    dis_str(22,22,"���ʺŲ�����!",0,0);
    getch();
    continue;
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"���ʻ��ѷ�������!",0,0);
    getch();
    continue;
  }
  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';
  if(strcmp(ss.passwd,da.pwd_new2)!=0) 
  {
    dis_err("���벻��!");
    continue;
  }

  mvprintw(7+num,30,"��    ��:[%8s]",ss.customer);
  mvprintw(8+num,30,"��    ַ:[%20s]",ss.address);
   mvprintw(9+num,30,"֤������:[%s]",ss.zj_num);

  dis_str(22,22,"��˶Ի���/��ַ��,�� q �˳�,����������!",0,0);
	refresh();
  yy=getch();
  if (yy=='q')
    continue;
  dis_str(22,22,"                                       ",0,0);

  tt.close_d=sys_work_d;
  tt.operator=sys_operator;
  
/***********************************/

  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }

  note("Ϊ���ڷֻ��޸��ֵ!");
  ss.last_day=sys_work_d;	
  
  ss.det_num=1;

  note("�޸Ļ��ڷֻ�!");
  
  set_ss_mast_u_sql(&ss,sql);
  char cond[40];
	sprintf(cond," open_br=%d and account=%ld",ss.open_br,ss.account);
	strcat(sql,cond);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"�޸Ļ����ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  note("Ϊ��־��ֵ!");
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_code=1592;
  tt.amount=da.amount;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"H");
  tt.open_br=da.open_br;
  tt.account=da.account;
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  strcpy(tt.abstract,da.abstract);
  tt.open_d=ss.acc_open_d;
  tt.balance=ss.balance;
  tt.item_num=ss.item_num;
  tt.det_num=ss.det_num;
  tt.interest=ss.interest;

  note("ȡ��ˮ��!");
  if ((tt.ser_num = get_ser(da.open_br)) < 0)
  {
    sprintf(sys_err_str,"ȡ��ˮ�Ŵ�!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  note("д��־!");
  
  set_s_trade_i_sql(&tt,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д��־�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  note("д������ϸ!");
  da.tr_op=sys_operator;
  
  sprintf(sql,"insert into s_detail values(%d,%ld,0,%d,%ld,0,'%s','D',%d,%d,%ld)",
    da.open_br,da.account,ss.det_num,ss.last_day,da.abstract,da.tr_br,da.tr_op,ss.balance);
    
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"д������ϸ�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    continue;
  }
  
  mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
  refresh();
  mvprintw(22,22,"���ڶ�����ָʾ�������´���д����!");
  refresh();
  sprintf(str2,"%09ld'%09ld'%05d",ss.account, ss.balance, ss.det_num);
  
  
  if(write_msr(str2,0)!=0) {
    dis_str(22,22,"д������!",60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mysql_commit(conn);
  
  
  mvprintw(22,22,"����뻻��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_t(&tt,1592,1);
  mvprintw(22,22,"���ٲ��뻻��ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q')
  	prt_h_t(&tt,1592,2);
  	
  mvprintw(22,22,"������´��ۺ���һ��,q�˳�!     ");
  refresh();
  qq=getch();
  if (qq!='Q' && qq!='q') {
    prt_h_nz(&tt);
  }
  prt_bdcz(ss.card_acc,da.tr_br);

  break;
}
  return(0);
}
