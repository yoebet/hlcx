/****************************************************************
������:zh_pro_sum.ec
������:zh_pro_sum()
��  ��:��������
��  ��:1909
��  ��:  
      1.����
        ��Ŀ�� (item)
        ������(amount) 
        ��������(sum_que) 
        ����(home_que) 
        �������� (sum_flag) 
        ժҪ (abstract) 
        ����Ա��(open_op)
        ҵ������ (tr_br �س�ȡ��¼����)
      2.����Ŀ�Ƿ����
      3.д��־
      4.д����
           
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
    mvaddstr(5,22,"�����ʵ�����");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    note("����Ŀ���Ƿ����!");
    
	  sprintf(sql,"select flag from sumtable where branch=%d and item_num=%ld and kind=90",da.open_br,da.item);
	  mysql_query(conn,sql);
	  
	  if(mysql_errno(conn))
	  {
      sprintf(sys_err_str,"ȡ��Ŀ�Ŵ�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }

	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL)
	  {
      sprintf(sys_err_str,"�޴˿�Ŀ!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
	  }
	  strcpy(item_flag,row[0]);
    mysql_free_result(res);
    
    if (item_flag[0]=='K')
    {
      dis_str(22,22,"����Ŀ������",60,0);
      getch();
      continue;
    }
    init_t(&tt);
    note("Ϊ��־��ֵ!");
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
    note("ժҪ:0�ֽ� ����ת��");
    strcpy(tt.abstract,"0");
    tt.open_d=da.open_d;
    tt.balance=da.amount;
    tt.item_num=da.item;
    note("��������");
    tt.det_num=da.sum_flag;
    note("��������");
    tt.term=da.sum_que;
    note("��/������");
    tt.transfer=da.home_que;

/***********************************/

    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }
    note("ȡ��ˮ��!");
    if ((tt.ser_num = get_ser(da.open_br)) < 0)
    {
      sprintf(sys_err_str,"ȡ��־�Ŵ�!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
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
    
    if (da.sum_flag==2)
    {
      note("�޸�����,����!");
	    sprintf(sql,"update sumtable set det_amt=det_amt+%ld,dvou_que=dvou_que+%d,\
	      balance=balance+%ld,rest_que=rest_que+%d\
	       where branch=%d and kind = 90 and item_num = %ld",
	       da.amount,da.sum_que,da.amount,da.home_que,da.open_br,da.item);
    }
    else
    {
      note("�޸�����,�跽!");
	    sprintf(sql,"update sumtable set cre_amt=cre_amt+%ld,cvou_que=cvou_que+%d,\
	      balance=balance-%ld,rest_que=rest_que-%d\
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
