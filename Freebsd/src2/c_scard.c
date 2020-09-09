/****************************************************************
������:c_open()
��  ��:���۲���
��  ��:1591
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

c_scard()
{
  struct ss_mast ss;
  struct s_trade tt;
  struct in_data da;
  short flag_k,ymd[3];
  char a_flag[2];
  static short in[9]={70,66,72,8};
  short num=4,flag;
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

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"�����۲�����");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)return(0);

  init_s(&ss);
  init_t(&tt);

  da.open_d=sys_work_d;
  
  note("��鱾���Ƿ����!");
  
	sprintf(sql,"SELECT * FROM s_mast WHERE account=%ld and open_br=%d",da.account,da.open_br);
	mysql_query(conn,sql);
	
	if(mysql_errno(conn))
	{
	  sprintf(sys_err_str,"%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}

	res=mysql_store_result(conn);
	row=mysql_fetch_row(res);
  
  if(row==NULL){
    dis_str(22,22,"���ʺŲ�����!",0,0);
    getch();
    return(-1);
  }
  set_ss_mast(&ss,row);
  mysql_free_result(res);
  
  note("����й����Ƿ���ֻ�һ��!");
  if (ss.acc_flag[0]!='A')
  {
    dis_str(22,22,"���ʻ��ѷ�������!",0,0);
    getch();
    return(-1);
  }

  cry_pwd(da.pwd_new1,da.pwd_new2,1); 
  da.pwd_new2[6]='\0';
  if(strcmp(ss.passwd,da.pwd_new2)!=0) 
  {
    dis_err("���벻��!");
    return(-1);
  }
  if(strcmp(ss.card_acc,"")!=0) 
  {
    dis_err("�����Ѱ쿨!");
    return(-1);
  }
  
  mvprintw(7+num,30,"��    ��:[%12s]",ss.customer);
  mvprintw(8+num,30,"��    ַ:[%30s]",ss.address);
  mvprintw(9+num,30,"֤������:[%s]",ss.zj_num);
  dis_str(22,22,"��˶Ի���/��ַ��,�� q �˳�,����������!",0,0);
	refresh();
  yy=getch();
  if (yy=='q')
    return(-1);
  dis_str(22,22,"                                       ",0,0);
  

  /***************************************/

  
  if(mysql_query(conn,"START TRANSACTION")){
    dis_err("������������");
    return(-1);
  }
  
  note("ȡ��ˮ��!");
  tt.ser_num = get_ser(da.open_br);
  if (tt.ser_num < 0)
  {
    sprintf(sys_err_str,"ȡ��־�Ŵ�!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  sprintf(sql,"update s_mast set card_acc='%s',card_open_d=%ld where open_br=%d and account=%ld",
    da.card_acc,da.open_d,da.open_br,da.account);
    
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"д���ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
  }
  
  tt.branch=da.open_br;
  tt.operator=sys_operator;
  tt.tr_op=sys_operator;
  tt.tr_op=da.tr_op;
  tt.tr_code=1591;
  tt.account=ss.account;
  tt.amount=da.amount;
  strcpy(tt.correct,"A");
  strcpy(tt.type,"K");
  tt.open_br=da.open_br;
  strcpy(tt.card_acc,da.card_acc);
  strcpy(tt.acc_type,ss.acc_type);
  strcpy(tt.customer,ss.customer);
  strcpy(tt.abstract,"0");
  tt.open_d=da.open_d;
  
  if (update_vou(da.open_br, "K0", 'O')<0)
  {
    sprintf(sys_err_str,"�޸Ŀ���ƾ֤��!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mvprintw(22,20,"���ڶ�����ָʾ�����󻮿�д����!");
  refresh();
  sprintf(str2,"%19s'%09ld'%05d",da.card_acc, ss.balance, ss.det_num);
  
  if(write_msr(str2,1)!=0) {
    dis_str(22,22,"д������!",60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mysql_commit(conn);
  mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
  refresh();

  mvprintw(22,20,"�����ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1591,1);
  mvprintw(22,20,"���ٲ���ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1591,2);

  return(0);
}
