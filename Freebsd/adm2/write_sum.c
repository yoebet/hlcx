/**************************************************************
������:write_sum.ec
������:write_sum()
��  ��:������־��ָ�����ŵ�����
��  ��:  
      2.�ӿ�Ŀ�ļ���ȡ���п�Ŀ��������sum
      3.����־��ȡ���������м�¼
      4.���ݽ�����ͳ�Ƹ���Ŀ���й�����
      5.����Ŀȡ�����໧,���,�뱾�ս��м��ռ���
      6.���ݻ��ܿ�Ŀ�����ļ�ͳ�Ƹ����ܿ�Ŀ������
      7.��������д�����Ŀ��90��־��¼��,
        �ȱ���ִ���ս���ʱ��ת������/����/����ȼ�¼��.
      8.�޸����ļ��еļ����ʱ�־
****************************************************************/

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;
extern MYSQL *conn;
extern char sql[];

MYSQL *conn2;
short item_count,vou_count;

struct to_sum sum[100];

void _clscon_s(){
	if(conn2!=NULL)mysql_close(conn2);
}

write_sum()
{
  struct s_trade tt;
  struct f_item  it;
  struct f_branch  br;
  char yy;
  
  long ii0,ii9,itt,li,ii,nn,ii_s,ii101,ii164;
  long iiint;
  long iiint0;
  short code,co12,co34;
  short abst;
  short debug=0;
  short inttax;
   
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res2;
  MYSQL_ROW row2;

  FILE *fp;
  long saccno,acc1[20];
  short s1,s2;
  long itemno;
  short branch;
  
  double yes_bal;
  short yes_home;

  static short in[2]={27,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

    clear_scr(5,22,22,77);
    mvaddstr(5,22,"�������ʡ�");
    refresh();
    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

    if (debug==1)
    {
      mvprintw(21,22,"���ڳ�ʼ����־��!");
      refresh();
      getch();
    }

    if (debug==1)
    {
      mvprintw(21,22,"���ڴ�item_file�ж���Ŀ�ż������Ϣ!");
      refresh();
      getch();
    }
    da.tr_br=100;
    branch=da.tr_br;

    note("ȡ�����ļ���ָ������¼");
    
    
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",branch);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      dis_str(22,22,"����������!",0,0);  
      getch();
      return(-1);
		}
    set_f_branch(&br,row);
    mysql_free_result(res);
    
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,22,"����������!",0,0);
      getch();
      return(-1);
    }

    note("��item_file�ж���Ŀ�ż������Ϣ!");
    nn=0;
    
	  sprintf(sql,"select * from item_file where item_flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ��Ŀ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"û�п�Ŀ!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    
    if(conn2==NULL){
    	conn2=mysql_init(NULL);
      atexit(_clscon_s);
	    if(!connectdb(conn2)){
	      sprintf(sys_err_str,"%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      getch();
	      return(-1);
	    }
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_item(&it,row);
      nn++;
      sum[nn].item_num=it.item_num;
      strcpy(sum[nn].item_name,it.item_name);
      sum[nn].sum_flag=it.sum_flag;
      sum[nn].prt_ser=it.prt_ser;
      sum[nn].j_x_num=0;
      sum[nn].j_x_amt=0;
      sum[nn].j_z_num=0;
      sum[nn].j_z_amt=0;
      sum[nn].d_x_num=0;
      sum[nn].d_x_amt=0;
      sum[nn].d_z_num=0;
      sum[nn].d_z_amt=0;
      sum[nn].open_num=0;
      sum[nn].close_num=0;
      note("ȡ��������Ŀ�������໧�����!"); 
      
	    sprintf(sql,"select balance,rest_que from sumtable where kind = 91\
	       and branch=%d and item_num=%ld",branch,it.item_num);
	    if(mysql_query(conn2,sql))
	    {
        yes_bal=0;
        yes_home=0;
      }else{
	      res2=mysql_store_result(conn2);
	      row2=mysql_fetch_row(res2);
	      if(row2!=NULL){
      	  yes_bal=atof(row2[0]);
      	  yes_home=atoi(row2[1]);
          mysql_free_result(res2);
	      }
        yes_bal=0;
        yes_home=0;
      }
      sum[nn].yes_bal=yes_bal;
      sum[nn].today_bal=0;
      sum[nn].yes_home=yes_home;
      sum[nn].today_home=0;
    }
    mysql_free_result(res);
    item_count=nn;
   
    if (debug==1)
    {
      mvprintw(21,22,"���ڵõ��ֽ��Ŀ101000�������±�!");
      refresh();
      getch();
    }
    note("�õ��ֽ��Ŀ101000�������±�!");
    ii101=get_i_item_ws(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[101000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    inttax=get_i_item_ws(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"�޿�˰��Ŀ[532001]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    note("�õ�Ͻ��������Ŀ164000�������±�!");
    ii164=get_i_item_ws(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[164000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    if (debug==1)
    {
      mvprintw(21,22,"���ڴ�trade��ȡ��ˮ!");
      refresh();
      getch();
    }
    note("��trade��ȡ��ˮ!");
    
	  sprintf(sql,"select * from trade where branch = %d and correct!='K'",branch);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ��ˮ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"û����ˮ!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
    
    clear_scr(6,22,22,77);
    mvprintw(10,22,"              ��ǰ������ˮ����");
    mvprintw(11,22,"==================================================");
    mvprintw(13,22,"���� ��Ա ���׺� ������ ��־ ��Ŀ��  ���   ��Ϣ  ");
    mvprintw(16,22,"==================================================");
    refresh();
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_s_trade(&tt,row);
      code=tt.tr_code;
      co12=tt.tr_code/100;
      co34=tt.tr_code%100;
      abst=atoi(tt.abstract);
      mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
      refresh();
      note("���������λ����10,Ϊ������ҵ��,���账��!");
      if (co34>10)
      {
        note("������ҵ��!");
        continue;
      }
      if (co12<=15)
      {
        note("����ҵ����õ���ҵ���Ŀ�����±�!");
        ii=get_i_item_ws(tt.item_num);
        if (ii<0)
        {
          sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",tt.item_num);
          dis_str(22,22,sys_err_str,60,0);
          getch();
          continue;
        }
      }
      switch (co34)
      {
        case 1:
          note("����ҵ��!");
          if (abst==0)
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�����ֽ����");
            sum[ii].d_x_num++;
            note("�����ֽ���");
            sum[ii].d_x_amt+=tt.amount;
            note("������");
            sum[ii].open_num++;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("����ת�ʱ���");
            sum[ii].d_z_num++;
            note("����ת�ʽ��");
            sum[ii].d_z_amt+=tt.amount;
            note("������");
            sum[ii].open_num++;

            note("��Ͻ��������Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii164].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 2:
          note("����ҵ��!");

          note("��������Ŀ�Ķ�ת��Ϣ��Ŀ���±�!");
          iiint=found(tt.type,tt.term,tt.item_num);
          if (iiint<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ����Ϣ��Ŀ!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          note("��Ϊ������ǰ֧ȡȡ������Ϣ!");
          if (tt.interest==tt.s_int)
          {
            iiint=get_i_item_ws(521100);
            if (iiint<0)
            {
              sprintf(sys_err_str,"����Ϣ��Ŀ[521100]��Ӧ�������±�!");
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
          }
          else
          {
            iiint0=get_i_item_ws(iiint);
            if (iiint<0)
            {
              sprintf(sys_err_str,"����Ϣ��Ŀ[%d]��Ӧ�������±�!",iiint);
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
            iiint=iiint0;
          }
          if (abst==0)
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum[ii].j_x_num++;
            note("�跽�ֽ���");
            sum[ii].j_x_amt+=tt.amount;
            note("������");
            sum[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�跽��Ϣ����");
            sum[iiint].j_x_num++;
            note("�跽��Ϣ");
            sum[iiint].j_x_amt+=tt.interest;
            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum[ii101].d_x_num++;
            note("�����ֽ���");
            sum[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii].j_z_amt+=tt.amount;
            note("������");
            sum[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�跽ת�ʱ���");
            sum[iiint].j_z_num++;
            note("�跽ת�ʽ��");
            sum[iiint].j_z_amt+=tt.interest;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_z_num++;
            note("�����ֽ���");
            sum[ii164].d_z_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("����ҵ��!");
          if (abst==0)
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�����ֽ����");
            sum[ii].d_x_num++;
            note("�����ֽ���");
            sum[ii].d_x_amt+=tt.amount;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("����ת�ʱ���");
            sum[ii].d_z_num++;
            note("����ת�ʽ��");
            sum[ii].d_z_amt+=tt.amount;

            note("��Ͻ��������Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii164].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 4:
          note("֧ȡҵ��!");
          if (abst==0)
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum[ii].j_x_num++;
            note("�跽�ֽ���");
            sum[ii].j_x_amt+=tt.amount;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum[ii101].d_x_num++;
            note("�����ֽ���");
            sum[ii101].d_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii].j_z_amt+=tt.amount;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_z_num++;
            note("�����ֽ���");
            sum[ii164].d_z_amt+=tt.amount;
          }
          break;
        case 5:
          note("����ҵ��!");
          note("��ԭ���������ٿ��»�����!");

          note("��������Ŀ�Ķ�ת������Ϣ��Ŀ���±�!");
          ii_s=found("S",tt.term,tt.item_num);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"�޻��ڶ�Ӧ����Ϣ��Ŀ!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (abst==0)
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");

            note("ԭ������!");

            note("�跽�ֽ����");
            sum[ii].j_x_num++;
            note("�跽�ֽ���");
            sum[ii].j_x_amt+=tt.balance;
            note("������");
            sum[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�跽�ֽ����");
            sum[ii_s].j_x_num++;
            note("�跽�ֽ���");
            sum[ii_s].j_x_amt+=tt.interest;
            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum[ii101].d_x_num++;
            note("�����ֽ���");
            sum[ii101].d_x_amt+=tt.balance+tt.interest+tt.tax_int;

            note("�»�����!");
            note("�����ֽ����");
            sum[ii].d_x_num++;
            note("�����ֽ���");
            sum[ii].d_x_amt+=tt.balance-tt.amount;
            note("������");
            sum[ii].open_num++;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum[ii101].j_x_amt+=tt.balance-tt.amount;

          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("ԭ������!");

            note("�跽�ֽ����");
            sum[ii].j_z_num++;
            note("�跽�ֽ���");
            sum[ii].j_z_amt+=tt.balance;
            note("������");
            sum[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�����ֽ����");
            sum[ii_s].d_z_num++;
            note("�����ֽ���");
            sum[ii_s].d_z_amt+=tt.interest;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_z_num++;
            note("�����ֽ���");
            sum[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;

            note("�»�����!");
            note("�����ֽ����");
            sum[ii].d_z_num++;
            note("�����ֽ���");
            sum[ii].d_z_amt+=tt.balance-tt.amount;
            note("������");
            sum[ii].open_num++;

            note("��Ͻ��������Ŀҵ��!");
            note("�跽�ֽ����");
            sum[ii164].j_z_num++;
            note("�跽�ֽ���");
            sum[ii164].j_z_amt+=tt.balance-tt.amount;
          }
          break;
        case 9:
          note("���ʵ���!");
          note("���ݵ��������ʿ�Ŀ��������/��������");
          note("���������޸Ķ�Ӧ��Ŀ������!");

          note("�õ���������Ŀ�������±�!");
          ii=get_i_item_ws(tt.item_num);
          if (ii<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (tt.det_num==1)
          {
            note("�跽����!");
            if (tt.abstract[0]=='0')
            {
              note("�ֽ�!");
              note("�跽�ֽ����");
              sum[ii].j_x_num+=tt.term;
              note("�跽�ֽ���");
              sum[ii].j_x_amt+=tt.amount;
            }
            else
            {
              note("ת��!");
              note("�跽ת�ʱ���");
              sum[ii].j_z_num+=tt.term;
              note("�跽ת�ʽ��");
              sum[ii].j_z_amt+=tt.amount;
            }
            note("������");
            sum[ii].close_num+=tt.transfer;
          }
          else
          {
            note("��������!");
            if (tt.abstract[0]=='0')
            {
              note("�ֽ�!");
              note("�����ֽ����");
              sum[ii].d_x_num+=tt.term;
              note("�����ֽ���");
              sum[ii].d_x_amt+=tt.amount;
            }
            else
            {
              note("ת��!");
              note("����ת�ʱ���");
              sum[ii].d_z_num+=tt.term;
              note("����ת�ʽ��");
              sum[ii].d_z_amt+=tt.amount;
            }
            note("������");
            sum[ii].open_num+=tt.transfer;
          }
          break;
        default:
          break;
      }
      note("��ǰ���״������!");
    }
    mysql_free_result(res);

    if (debug==1)
    {
      mvprintw(22,22,"����ͳ�Ƹ���Ŀ�ı����໧�����!");
      refresh();
      getch();
    }
    
    note("ͳ�Ƹ���Ŀ�ı����໧�����!");
    for (ii=1;ii<=item_count;ii++)
    {
      sum[ii].today_bal=sum[ii].yes_bal
                        +sum[ii].d_x_amt+sum[ii].d_z_amt
                        -sum[ii].j_x_amt-sum[ii].j_z_amt;
      sum[ii].today_home=sum[ii].yes_home
                        +sum[ii].open_num-sum[ii].close_num;
      if (debug==1)
      {
        mvprintw(18,22,"item_num=%d,item_name=[%s],balance=%10.2lf,home=%d",sum[ii].item_num,sum[ii].item_name,sum[ii].today_bal,sum[ii].today_home);
        mvprintw(19,22,"d_x_amt=%ld,d_z_amt=%ld,j_x_amt=%ld,j_z_amt=%ld", sum[ii].d_x_amt,sum[ii].d_z_amt,sum[ii].j_x_amt,sum[ii].j_z_amt);
        mvprintw(20,22,"yes_home=%d,open_num=%d,close_num=%d", sum[ii].yes_home,sum[ii].open_num,sum[ii].close_num);
        refresh();
        getch();
      }
    }

    if (debug==1)
    {
      mvprintw(21,22,"���ڸ��ݻ����ļ�ͳ�Ƹ����ܿ�Ŀ������!");
      refresh();
      getch();
    }
    note("���ݻ��ܿ�Ŀ�����ļ�ͳ�Ƹ����ܿ�Ŀ������!");
    fp=fopen("./data/def_sum","r");
    if(fp==NULL) 
    {
      dis_str(22,22,"���ܴ򿪻��ܶ����ļ�data/def_sum",60,0);
      getch();
      return(-1);
    }
    s1=0;
    while(fscanf(fp,"%ld",&saccno)==1)
    {
      if(saccno != 0L)
      {
        note("���ζ�һ���ڸ��ۼӿ�Ŀ!");
        acc1[s1]=saccno;
        s1++;
        continue;
      }
      else
      {
        note("�Ѷ���һ������!");
        if(s1<2)
        {
          s1=0;
          dis_str(22,22,"���ܶ����ļ���ʽ��!���ۼӿ�Ŀ!",60,0);
          getch();
          continue;
        }
        note("���ܿ�Ŀ itemno!");
        itemno=acc1[0];
        note("�õ����ܿ�Ŀ�������±�!");
        ii0=get_i_item_ws(itemno);
        if (ii0<0)
        {
          sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",itemno);
          dis_str(22,22,sys_err_str,60,0);
          getch();
          continue;
        }
          sum[ii0].j_x_num   =0;
          sum[ii0].j_x_amt   =0;
          sum[ii0].j_z_num   =0;
          sum[ii0].j_z_amt   =0;
          sum[ii0].d_x_num   =0;
          sum[ii0].d_x_amt   =0;
          sum[ii0].d_z_num   =0;
          sum[ii0].d_z_amt   =0;
          sum[ii0].open_num  =0;
          sum[ii0].close_num =0;
          sum[ii0].yes_bal   =0;
          sum[ii0].today_bal =0;
          sum[ii0].yes_home  =0;
          sum[ii0].today_home=0;
        note("�����ۼӿ�Ŀ������ܿ�Ŀ������!");
        for(s2=1;s2<s1;s2++)
        {
          itemno=acc1[s2];
          ii9=get_i_item_ws(itemno);
          if (ii9<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",itemno);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          sum[ii0].j_x_num   +=sum[ii9].j_x_num;
          sum[ii0].j_x_amt   +=sum[ii9].j_x_amt;
          sum[ii0].j_z_num   +=sum[ii9].j_z_num;
          sum[ii0].j_z_amt   +=sum[ii9].j_z_amt;
          sum[ii0].d_x_num   +=sum[ii9].d_x_num;
          sum[ii0].d_x_amt   +=sum[ii9].d_x_amt;
          sum[ii0].d_z_num   +=sum[ii9].d_z_num;
          sum[ii0].d_z_amt   +=sum[ii9].d_z_amt;
          sum[ii0].open_num  +=sum[ii9].open_num;
          sum[ii0].close_num +=sum[ii9].close_num;
          sum[ii0].yes_bal   +=sum[ii9].yes_bal;
          sum[ii0].today_bal +=sum[ii9].today_bal;
          sum[ii0].yes_home  +=sum[ii9].yes_home;
          sum[ii0].today_home+=sum[ii9].today_home;
        }
        s1=s2=0;
      }
    }
    fclose(fp);

    
    /***********************************/
    if(mysql_query(conn,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }
    if (debug==1)
    {
      mvprintw(21,22,"���ڽ�ͳ�ƽ��д�뱾��90��¼��!");
      refresh();
      getch();
    }
    note("��ͳ��������д�����Ŀ��90��־��¼��!");

    for (ii=1;ii<=item_count;ii++)
    {
    	sprintf(sql,"update sumtable set det_amt=%ld,cre_amt=%ld,open_que=%d,\
        cls_que=%d,dvou_que=%d,cvou_que=%d,balance=%f,rest_que=%d\
        where branch=%d and kind=90 and item_num = %d",
        sum[ii].d_x_amt+sum[ii].d_z_amt,sum[ii].j_x_amt+sum[ii].j_z_amt,sum[ii].open_num,sum[ii].close_num,
        sum[ii].d_x_num+sum[ii].d_z_num,sum[ii].j_x_num+sum[ii].j_z_num,sum[ii].today_bal,sum[ii].today_home,branch,sum[ii].item_num);
      if(mysql_query(conn,sql)) 
      {
        sprintf(sys_err_str,"�޸����ʴ�!item=%ld.%s",sum[ii].item_num,mysql_error(conn));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn);
        getch();
        return(-1);
      }
    }
    if (debug==1)
    {
      mvprintw(21,22,"�޸����ļ��еļ����ʱ�־!");
      refresh();
      getch();
    }
    
    sprintf(sql,"update br_file set sum_flag=1 where branch=%d",branch);
    if(mysql_query(conn,sql))
    {
      sprintf(sys_err_str,"�޸����ļ���!%s",mysql_error(conn));
      dis_str(23,10,sys_err_str,60,0);
      mysql_rollback(conn);
      getch();
      return(-1);
    }
    mysql_commit(conn);

    note("��ʾ����Ŀͳ�ƽ��!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("��ǰ��Ŀ�޷������������!");
      if(sum[ii].j_z_num+sum[ii].j_x_num+sum[ii].j_z_amt+sum[ii].j_x_amt+sum[ii].d_z_num+sum[ii].d_x_num+sum[ii].d_z_amt+sum[ii].d_x_amt+sum[ii].open_num+sum[ii].close_num==0.00)
        continue;
      clear_scr(li,22,22,77);
      mvprintw(li+1,25,"����[%3d]      ȫ����Ŀ[%ld]�սᵥ ",branch,sum[ii].item_num);
      mvprintw(li+2,25,"===================================================");
      mvprintw(li+3,25,"    ��         ��               ��         ��    ");
      mvprintw(li+4,25,"===================================================");
      mvprintw(li+5,25,"�ֽ����     �ֽ�����      �ֽ����    �ֽ�����");
      mvprintw(li+6,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_x_num,sum[ii].j_x_amt/100.00,sum[ii].d_x_num,sum[ii].d_x_amt/100.00);
      mvprintw(li+7,25,"ת�ʱ���     ת�ʷ�����      ת�ʱ���    ת�ʷ�����");
      mvprintw(li+8,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_z_num,sum[ii].j_z_amt/100.00,sum[ii].d_z_num,sum[ii].d_z_amt/100.00);
      mvprintw(li+9,25,"�ϼƱ���     �ϼƷ�����      �ϼƱ���    �ϼƷ�����");
      mvprintw(li+10,25," %5d       %10.2lf       %5d      %10.2lf ",sum[ii].j_z_num+sum[ii].j_x_num,(sum[ii].j_z_amt+sum[ii].j_x_amt)/100.00,sum[ii].d_z_num+sum[ii].d_x_num,(sum[ii].d_z_amt+sum[ii].d_x_amt)/100.00);
      mvprintw(li+12,25,"  ������:%5d       ������:%5d ",sum[ii].open_num,sum[ii].close_num);
      mvprintw(li+13,25,"===================================================");
      refresh();
    }
    
    dis_err("�������! ");  
  return(0);
}

get_i_item_ws(item)
long item;
{
   short i;
   for (i=1;i<=item_count;i++)
   {
     if (sum[i].item_num==item)
       break;
   }
   if (i>item_count)
     return(-1);
   else
     return(i);
}
