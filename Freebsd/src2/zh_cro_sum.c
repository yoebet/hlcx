/****************************************************************
������:zh_cro_sum.ec
������:zh_cro_sum()
��  ��:�������ʳ���
��  ��:1981
��  ��:  
      1.����
        ��Ŀ�� (item)
        ������(amount) 
        ��������(sum_que) 
        ����(home_que) 
        ���׺� (ser_num) 
      2.ȡָ�����׺ŵ���ˮ
      3.�����ˮ״̬������¼�����
      4.�޸���ˮΪ����
      5.д���������
           
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
    mvaddstr(5,22,"�����ʵ���������");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    init_t(&tt);

    note("���ָ�����׺��Ƿ����!");
    br=da.open_br;
    op=sys_operator;
    
	  sprintf(sql,"select * from trade where branch=%d and account=%ld and operator=%d and ser_num=%d",
	    da.open_br,da.account,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ�����״�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      dis_str(22,22,"�����ײ�����!",0,0);
      getch();
      continue;
	  }
	  set_s_trade(&tt,row);
    mysql_free_result(res);
	  
    note("�����ˮ���й����Ƿ���¼��һ��!");
    if (tt.correct[0]!='A')
    {
      dis_str(22,22,"��ˮ״̬��!",0,0);
      getch();
      continue;
    }
    if (tt.tr_code!=1909)
    {
      dis_str(22,22,"����ˮ�в����벻��!",0,0);
      getch();
      continue;
    }
    if (tt.amount!=da.amount)
    {
      dis_str(22,22,"����ˮ�з������!",0,0);
      getch();
      continue;
    }
    if (tt.term!=da.sum_que)
    {
      dis_str(22,22,"����ˮ��ҵ���������!",0,0);
      getch();
      continue;
    }

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }

    note("�޸�ԭ��ˮΪ����!");
    
	  sprintf(sql,"update trade set correct='K' where branch=%d and operator=%d and ser_num=%d",
	    da.open_br,op,da.ser_num);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"�޸ı����״�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      continue;
	  }

    note("�޸�����!");
    da.amount=tt.amount/100.00;
    
    if (da.sum_flag==2)
    {
      note("�޸�����,����!");
	    sprintf(sql,"update sumtable set det_amt=det_amt-%ld,dvou_que=dvou_que-%d,\
	      balance=balance-%ld,rest_que=rest_que-%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    else
    {
      note("�޸�����,�跽!");
	    sprintf(sql,"update sumtable set cre_amt=cre_amt-%ld,cvou_que=cvou_que-%d,\
	      balance=balance+%ld,rest_que=rest_que+%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    
    if(mysql_query(conn,sql)||(mysql_errno(conn)))
    {
      sprintf(sys_err_str,"�޸����ʴ�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      continue;
    }
    mysql_commit(conn);
    
    mvprintw(1,68,"��ˮ��:%d",tt.ser_num);
    refresh();
    dis_err("�������! ");  
      break;
  }
  return(0);
}
