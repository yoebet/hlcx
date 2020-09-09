/****************************************************************
������:c_open.ec
������:c_open()
��  ��:������
��  ��:1501
��  ��:  
      1.����
        ���� (customer)
        ��ַ (address)
        ��� (amount) 
        ��Ϣ����(interest) 
        �ʻ�����(acc_type P���� G����)
      2.��鱾�����Ƿ����.
      3.�������
      4.����¼����Ϣȡ������Ŀ��/����/��ˮ��
      5.Ϊ���ֻ�/��־���й��ֵ
      6.д���ֻ�/��־��
      7.������ˮ��
      8.д��ϸ
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
  mvaddstr(5,30,"����������");  
  refresh();
  flag=input_data(num,in,&da);
  if (flag<0)return(0);

  init_s(&ss);
  init_t(&tt);

  note("��鱾���Ƿ����!");

  da.open_br=100;
  
  note("���ſ�������Ϊ1��!");
  if (da.amount < 1)
  {
    dis_err("��������С��1Ԫ!");
    return(0);
  }

  da.open_d=sys_work_d;
  rjulmdy(da.open_d,ymd);
  interest=s_int(ymd,da.amount);
  tt.operator=sys_operator;
  
  note("Ϊ���ŷֻ���ֵ!");
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
    sprintf(sys_err_str,"ȡ������Ŀ�Ŵ�!code=%ld.",ss.item_num);
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(0);
  }
  note("Ϊ��־��ֵ!");
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
    sprintf(sys_err_str,"������������");
    dis_err(sys_err_str);
    return(-1);
  }
  mvprintw(7,30,"��������:[%s]",da.card_acc);
  refresh();
	
  strcpy(ss.card_acc,da.card_acc);
  strcpy(tt.card_acc,da.card_acc);
  ss.account=0;
  tt.account=0;
  
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
  note("д���ֻ�!");
  
  set_ss_mast_i_sql(&ss,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д���ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
 
  note("д��־!");
  
  set_s_trade_i_sql(&tt,sql);
  
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д��־�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  note("д����ϸ!");
  da.tr_op=sys_operator;
  
  sprintf(sql,"insert into s_detail values(%d,0,'%s',%d,%ld,%ld,'0','O',%d,%d,%ld)",
    da.open_br,da.card_acc,ss.det_num,ss.last_day,da.amount,da.tr_br,da.tr_op,da.amount);
    
  if(mysql_query(conn,sql)||(mysql_errno(conn)))
  {
    sprintf(sys_err_str,"д����ϸ�ļ���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
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
  sprintf(str2,"%19s'%09ld'%05d",ss.card_acc, ss.balance, ss.det_num);
  
  if(write_msr(str2,1)!=0) {
    dis_str(22,22,"д������!",60,0);
    mysql_rollback(conn);
    getch();
    return(0);
  }
  
  mysql_commit(conn);
  mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
  refresh();
  
  mvprintw(22,20,"�������ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1501,1);
  mvprintw(22,20,"���ٲ�����ƾ������һ��,q�˳�!");
  refresh();
  qq=getch();
  if (qq !='Q' && qq!='q')
  	prt_h_card(&tt,1501,2);
  	
  return(0);
}
