/****************************************************************
������:total_br.ec
������:total_br()
��  ��:����ָ�����ŵĿ�Ŀ�սᵥ
��  ��:1811
��  ��:  
      1.����
      2.�ӿ�Ŀ�ļ���ȡ���п�Ŀ��������sum_br
      3.����־��ȡ���������м�¼
      4.���ݽ�����ͳ�Ƹ���Ŀ����ƾ֤���й�����
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

short item_count,vou_count;
struct to_sum sum_br[100];
struct to_vou vou[50];

total_br()
{
  struct s_trade tt;
  struct f_item  it;
  struct f_voufile  vo;
  struct in_data da;
  FILE *fp,*fp1,*fopen();//fopen?
  char ddd[150];
  char yy;
  static short in[2]={27,8};
  short num=2,flag;
   
  MYSQL_RES *res;
  MYSQL_ROW row;
 
  short itt,li,ii,nn,ii_s,ii101,ii164,iiint;
  short code,co12,co34;
  char cro[2],abst[2];
  short debug=0;
  long int_item;
  short inttax;

  init_in(&da);
  da.tr_br=sys_branch;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"����ȫ�������ʡ�");
    refresh();

    init_t(&tt);

    note("��ʼ��ƾ֤����!");
    for (ii=1;ii<=9;ii++)
    {
      strcpy(vou[ii].vou_type," ");
      vou[ii].rest_que=0;
      vou[ii].use_que=0;
      vou[ii].cancel=0;
      vou[ii].chg_book=0;
      vou[ii].takeback=0;
    }
    
    note("��item_file�ж���Ŀ�ż������Ϣ!");
    nn=0;
    
	  sprintf(sql,"select * from item_file where item_flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ��Ŀ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break; //����û���û����룬continue ����ѭ�������¾���
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"û�п�Ŀ!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_item(&it,row);
      nn++;
      sum_br[nn].item_num=it.item_num;
      strcpy(sum_br[nn].item_name,it.item_name);
      sum_br[nn].sum_flag=it.sum_flag;
      sum_br[nn].prt_ser=it.prt_ser;
      sum_br[nn].j_x_num=0;
      sum_br[nn].j_x_amt=0;
      sum_br[nn].j_z_num=0;
      sum_br[nn].j_z_amt=0;
      sum_br[nn].d_x_num=0;
      sum_br[nn].d_x_amt=0;
      sum_br[nn].d_z_num=0;
      sum_br[nn].d_z_amt=0;
      sum_br[nn].open_num=0;
      sum_br[nn].close_num=0;
      sum_br[nn].yes_bal=0;
      sum_br[nn].today_bal=0;
    }
    mysql_free_result(res);
    item_count=nn;
   
    
    note("��vou_file�ж�ƾ֤�������Ϣ!");
    nn=0;
    
	  sprintf(sql,"select * from vou_file where flag!='K'");
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡƾ֤��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"û��ƾ֤!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break; 
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_voufile(&vo,row);
      nn++;
      strcpy(vou[nn].vou_type,vo.vou_type);
      strcpy(vou[nn].vou_name,vo.vou_name);
      strcpy(vou[nn].type,vo.type);
      vou[nn].rest_que=0;
      vou[nn].use_que=0;
      vou[nn].cancel=0;
      vou[nn].chg_book=0;
      vou[nn].takeback=0;
    }
    mysql_free_result(res);
    vou_count=nn;

    note("�õ��ֽ��Ŀ101000�������±�!");
    ii101=get_i1_item(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[101000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
     // break; //����û���û����룬continue ����ѭ�������¾���
    }

    inttax=get_i1_item(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"�޿�˰��Ŀ[532001]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      //break;
    }
    note("�õ�Ͻ��������Ŀ164000�������±�!");
    ii164=get_i1_item(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[164000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      //break;
    }

    note("��trade��ȡ��ˮ!");
    
	  sprintf(sql,"select * from trade where branch=%d order by operator,ser_num",da.tr_br);
	  if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"ȡ��ˮ��!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
	  }
	  res=mysql_store_result(conn);
	  row=mysql_fetch_row(res);
    if(row==NULL)
    {
      sprintf(sys_err_str,"û����ˮ!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
    }
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_s_trade(&tt,row);
      code=tt.tr_code;
      co12=tt.tr_code/100;
      co34=tt.tr_code%100;
      strcpy(abst,tt.abstract);
      strcpy(cro,tt.correct);

      note("����������ͳ��ʹ��ƾ֤��!");
      if (cro[0]=='K' && co34==1)
      {
        note("���ݴ����޸Ķ�Ӧƾ֤ʹ������!");
        up_vou(tt.type,"U",1);
        continue;
      }

      if (cro[0]=='K')
        continue;

      note("���������λ����10,Ϊ������ҵ��,���账��!");
      if (co34>10)
      {
        note("������ҵ��!");
        continue;
      }

      if (co12<=15)
      {
        note("����ҵ����õ���ҵ���Ŀ�����±�!");
        ii=get_i1_item(tt.item_num);
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
          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�����ֽ����");
            sum_br[ii].d_x_num++;
            note("�����ֽ���");
            sum_br[ii].d_x_amt+=tt.amount;
            note("������");
            sum_br[ii].open_num++;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum_br[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("����ת�ʱ���");
            sum_br[ii].d_z_num++;
            note("����ת�ʽ��");
            sum_br[ii].d_z_amt+=tt.amount;
            note("������");
            sum_br[ii].open_num++;

            note("��Ͻ��������Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum_br[ii164].j_z_num++;
            note("�跽ת�ʽ��");
            sum_br[ii164].j_z_amt+=tt.amount;
          }
          note("���ݴ����޸Ķ�Ӧƾ֤ʹ������!");
          up_vou(tt.type,"U",1);
          break;
        case 2:
          note("����ҵ��!");

          note("��������Ŀ�Ķ�ת��Ϣ��Ŀ���±�!");
          int_item=found(tt.type,tt.term,tt.item_num);
          if (int_item<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ����Ϣ��Ŀ!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          iiint=get_i1_item(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          note("��Ϊ������ǰ֧ȡȡ������Ϣ!");
          if (tt.interest==tt.s_int)
          {
            iiint=get_i1_item(521100);
            if (iiint<0)
            {
              sprintf(sys_err_str,"����Ϣ��Ŀ[521100]��Ӧ�������±�!");
              dis_str(22,22,sys_err_str,60,0);
              getch();
              continue;
            }
          }

          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum_br[ii].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii].j_x_amt+=tt.amount;
            note("������");
            sum_br[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�����ֽ����");
            sum_br[iiint].j_x_num++;
            note("�����ֽ���");
            sum_br[iiint].j_x_amt+=tt.interest;

            sum_br[inttax].j_x_num++;
            sum_br[inttax].j_x_amt+=tt.tax_int ;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii101].d_x_num++;
            note("�����ֽ���");
            sum_br[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum_br[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum_br[ii].j_z_amt+=tt.amount;
            note("������");
            sum_br[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("����ת�ʱ���");
            sum_br[iiint].j_z_num++;
            note("����ת�ʽ��");
            sum_br[iiint].j_z_amt+=tt.interest;
            sum_br[inttax].j_z_num++;
            sum_br[inttax].j_z_amt+=tt.tax_int ;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii164].d_z_num++;
            note("�����ֽ���");
            sum_br[ii164].d_z_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("����ҵ��!");
          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�����ֽ����");
            sum_br[ii].d_x_num++;
            note("�����ֽ���");
            sum_br[ii].d_x_amt+=tt.amount;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum_br[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii101].j_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("����ת�ʱ���");
            sum_br[ii].d_z_num++;
            note("����ת�ʽ��");
            sum_br[ii].d_z_amt+=tt.amount;

            note("��Ͻ��������Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum_br[ii164].j_z_num++;
            note("�跽ת�ʽ��");
            sum_br[ii164].j_z_amt+=tt.amount;
          }
          break;
        case 4:
          note("֧ȡҵ��!");
          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum_br[ii].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii].j_x_amt+=tt.amount;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii101].d_x_num++;
            note("�����ֽ���");
            sum_br[ii101].d_x_amt+=tt.amount;
          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum_br[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum_br[ii].j_z_amt+=tt.amount;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii164].d_z_num++;
            note("�����ֽ���");
            sum_br[ii164].d_z_amt+=tt.amount;
          }
          break;
        case 5:
          note("����ҵ��!");
          note("��ԭ���������ٿ��»�����!");

          note("��������Ŀ�Ķ�ת������Ϣ��Ŀ���±�!");
          int_item=found("S",tt.term,tt.item_num);
          if (int_item<0)
          {
            sprintf(sys_err_str,"�޻��ڶ�Ӧ����Ϣ��Ŀ!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          ii_s=get_i1_item(int_item);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ�������±�!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");

            note("ԭ������!");

            note("�跽�ֽ����");
            sum_br[ii].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii].j_x_amt+=tt.balance;
            note("������");
            sum_br[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�����ֽ����");
            sum_br[ii_s].j_x_num++;
            note("�����ֽ���");
            sum_br[ii_s].j_x_amt+=tt.interest;
            sum_br[inttax].j_x_num++;
            sum_br[inttax].j_x_amt+=tt.tax_int ;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii101].d_x_num++;
            note("�����ֽ���");
            sum_br[ii101].d_x_amt+=tt.balance+tt.interest+tt.tax_int;

            note("�»�����!");
            note("�����ֽ����");
            sum_br[ii].d_x_num++;
            note("�����ֽ���");
            sum_br[ii].d_x_amt+=tt.balance-tt.amount;
            note("������");
            sum_br[ii].open_num++;

            note("���ֽ��Ŀҵ��!");
            note("�跽�ֽ����");
            sum_br[ii101].j_x_num++;
            note("�跽�ֽ���");
            sum_br[ii101].j_x_amt+=tt.balance-tt.amount;

          }
          else
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("ԭ������!");

            note("�跽�ֽ����");
            sum_br[ii].j_z_num++;
            note("�跽�ֽ���");
            sum_br[ii].j_z_amt+=tt.balance;
            note("������");
            sum_br[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�����ֽ����");
            sum_br[ii_s].j_z_num++;
            note("�����ֽ���");
            sum_br[ii_s].j_z_amt+=tt.interest;
            sum_br[inttax].j_z_num++;
            sum_br[inttax].j_z_amt+=tt.tax_int ;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum_br[ii164].d_z_num++;
            note("�����ֽ���");
            sum_br[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;

            note("�»�����!");
            note("�����ֽ����");
            sum_br[ii].d_z_num++;
            note("�����ֽ���");
            sum_br[ii].d_z_amt+=tt.balance-tt.amount;
            note("������");
            sum_br[ii].open_num++;

            note("��Ͻ��������Ŀҵ��!");
            note("�跽�ֽ����");
            sum_br[ii164].j_z_num++;
            note("�跽�ֽ���");
            sum_br[ii164].j_z_amt+=tt.balance-tt.amount;
          }
          note("���ݴ����޸Ķ�Ӧƾ֤ʹ������!");
          up_vou(tt.type,"U",1);
          break;
        case 6:
          note("��ʧ����!");
          note("���ݴ����޸Ķ�Ӧƾ֤ʹ������!");
          up_vou(tt.type,"U",1);
          break;
        case 8:
          note("ƾ֤����!");
          note("���ݵ�����ƾ֤����,���������������޸Ķ�Ӧƾ֤ʹ������!");
          note("�޸����� R_��� U_ʹ�� K_���� C_���� T_�Ͻ�");
          if (tt.det_num==1)
          {
            note("���");
            up_vou(tt.abstract,"R",tt.term);
          }
          else if (tt.det_num==2)
          {
            note("ʹ��");
            up_vou(tt.abstract,"U",tt.term);
          }
          else if (tt.det_num==3)
          {
            note("����");
            up_vou(tt.abstract,"K",tt.term);
          }
          else if (tt.det_num==4)
          {
            note("�Ͻ�");
            up_vou(tt.abstract,"T",tt.term);
          }
          else if (tt.det_num==5)
          {
            note("����");
            up_vou(tt.abstract,"C",tt.term);
          }
          break;
        case 9:
          note("���ʵ���!");
          note("���ݵ��������ʿ�Ŀ��������/��������");
          note("���������޸Ķ�Ӧ��Ŀ������!");

          note("�õ���������Ŀ�������±�!");
          ii=get_i1_item(tt.item_num);
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
              sum_br[ii].j_x_num+=tt.term;
              note("�跽�ֽ���");
              sum_br[ii].j_x_amt+=tt.amount;
            }
            else
            {
              note("ת��!");
              note("�跽ת�ʱ���");
              sum_br[ii].j_z_num+=tt.term;
              note("�跽ת�ʽ��");
              sum_br[ii].j_z_amt+=tt.amount;
            }
            note("������");
            sum_br[ii].close_num+=tt.transfer;
          }
          else
          {
            note("��������!");
            if (tt.abstract[0]=='0')
            {
              note("�ֽ�!");
              note("�����ֽ����");
              sum_br[ii].d_x_num+=tt.term;
              note("�����ֽ���");
              sum_br[ii].d_x_amt+=tt.amount;
            }
            else
            {
              note("ת��!");
              note("����ת�ʱ���");
              sum_br[ii].d_z_num+=tt.term;
              note("����ת�ʽ��");
              sum_br[ii].d_z_amt+=tt.amount;
            }
            note("������");
            sum_br[ii].open_num+=tt.transfer;
            itt=tt.item_num/100;
            if (itt==2151)
              strcpy(tt.type,"F");
            else if (itt==2111)
              strcpy(tt.type,"S");
            else if (itt==2153)
              strcpy(tt.type,"P");
            else
              strcpy(tt.type,"M");
            note("���ݴ����޸Ķ�Ӧƾ֤ʹ������!");
            up_vou(tt.type,"U",tt.transfer);
          }
          break;
        default:
          break;
      }
      note("��ǰ���״������!");
    }
    mysql_free_result(res);

    note("��ʾͳ�ƽ��!");
    note("��ʾ����Ŀͳ�ƽ��!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("��ǰ��Ŀ�޷������������!");
      if(sum_br[ii].j_z_num+sum_br[ii].j_x_num+sum_br[ii].j_z_amt+sum_br[ii].j_x_amt+sum_br[ii].d_z_num+sum_br[ii].d_x_num+sum_br[ii].d_z_amt+sum_br[ii].d_x_amt+sum_br[ii].open_num+sum_br[ii].close_num==0.00)
        continue;
      if (sum_br[ii].item_num==532001)
        continue;
      clear_scr(li,22,22,77);
      mvprintw(li+1,15,"      ȫ������Ŀ[%ld]�սᵥ ",sum_br[ii].item_num);
      mvprintw(li+2,15,"===================================================");
      mvprintw(li+3,15,"    ��         ��               ��         ��    ");
      mvprintw(li+4,15,"===================================================");
      mvprintw(li+5,15,"�ֽ����     �ֽ�����      �ֽ����    �ֽ�����");
      mvprintw(li+6,15," %5d       %10.2lf       %5d      %10.2lf ",sum_br[ii].j_x_num,sum_br[ii].j_x_amt/100.00,sum_br[ii].d_x_num,sum_br[ii].d_x_amt/100.00);
      //mvprintw(li+7,15,"ת�ʱ���     ת�ʷ�����      ת�ʱ���    ת�ʷ�����");
      //mvprintw(li+8,15," %5d       %10.2lf       %5d      %10.2lf ",sum_br[ii].j_z_num,sum_br[ii].j_z_amt/100.00,sum_br[ii].d_z_num,sum_br[ii].d_z_amt/100.00);
      //mvprintw(li+9,15,"�ϼƱ���     �ϼƷ�����      �ϼƱ���    �ϼƷ�����");
      //mvprintw(li+10,15," %5d       %10.2lf       %5d      %10.2lf ",sum_br[ii].j_z_num+sum_br[ii].j_x_num,(sum_br[ii].j_z_amt+sum_br[ii].j_x_amt)/100.00,sum_br[ii].d_z_num+sum_br[ii].d_x_num,(sum_br[ii].d_z_amt+sum_br[ii].d_x_amt)/100.00);
      mvprintw(li+12,15,"  ������:%5d       ������:%5d ",sum_br[ii].open_num,sum_br[ii].close_num);
      mvprintw(li+13,15,"===================================================");
      refresh();
      dis_str(22,22,"����,�� q_�˳�,����������!",0,0);
      yy=getch();
      if (yy=='q' || yy=='Q')
        break;
    }
    li=7;
    clear_scr(li,22,22,77);
    mvprintw(li+1,15,"         ȫ����ƾ֤ʹ��ͳ��");
    mvprintw(li+2,15,"===================================================");
    mvprintw(li+3,15,"����  ��    ��  ʣ���� ʹ���� ������ ������ ������");
    mvprintw(li+4,15,"===================================================");
    refresh();
    for(ii=1;ii<=vou_count;ii++)
    {
      mvprintw(li+4+ii,15," %2s %10s   %4d   %4d   %4d   %4d   %4d ",vou[ii].vou_type,vou[ii].vou_name,vou[ii].rest_que,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    mvprintw(li+4+ii,15,"===================================================");
    refresh();
  mvprintw(22,20,"  �Ƿ��ӡ(Y/y_��ӡ,�������˳�)!");
  refresh();
  yy=getch();
  if (yy !='Y' && yy!='y')
	{
	
    dis_err("�������! ");  
      break;
	}
     	fp=fopen("/tmp/zzd","w");
	if(fp==NULL)
        {
		dis_err("���ܴ���ʱ�ļ�/tmp/dzd!");
		return(-1);
   	}

    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      if(sum_br[ii].j_z_num+sum_br[ii].j_x_num+sum_br[ii].j_z_amt+sum_br[ii].j_x_amt+sum_br[ii].d_z_num+sum_br[ii].d_x_num+sum_br[ii].d_z_amt+sum_br[ii].d_x_amt+sum_br[ii].open_num+sum_br[ii].close_num==0.00)
        continue;
      if (sum_br[ii].item_num==532001)
        continue;
      fprintf(fp,"      ȫ������Ŀ[%ld]�սᵥ\n ",sum_br[ii].item_num);
      fprintf(fp,"===================================================\n");
      fprintf(fp,"    ��         ��               ��         ��    \n");
      fprintf(fp,"===================================================\n");
      fprintf(fp,"�ֽ����     �ֽ�����      �ֽ����    �ֽ�����\n");
      fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_br[ii].j_x_num,sum_br[ii].j_x_amt/100.00,sum_br[ii].d_x_num,sum_br[ii].d_x_amt/100.00);
      //fprintf(fp,"ת�ʱ���     ת�ʷ�����      ת�ʱ���    ת�ʷ�����\n");
      //fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_br[ii].j_z_num,sum_br[ii].j_z_amt/100.00,sum_br[ii].d_z_num,sum_br[ii].d_z_amt/100.00);
      //fprintf(fp,"�ϼƱ���     �ϼƷ�����      �ϼƱ���    �ϼƷ�����\n");
      //fprintf(fp," %5d       %10.2lf       %5d      %10.2lf \n",sum_br[ii].j_z_num+sum_br[ii].j_x_num,(sum_br[ii].j_z_amt+sum_br[ii].j_x_amt)/100.00,sum_br[ii].d_z_num+sum_br[ii].d_x_num,(sum_br[ii].d_z_amt+sum_br[ii].d_x_amt)/100.00);
      fprintf(fp,"  ������:%5d       ������:%5d \n",sum_br[ii].open_num,sum_br[ii].close_num);
      fprintf(fp,"===================================================\n");
    }

    fprintf(fp,"         ȫ����ƾ֤ʹ��ͳ��\n");
    fprintf(fp,"===================================================\n");
    fprintf(fp,"����  ��    ��  ʣ���� ʹ���� ������ ������ ������\n");
    fprintf(fp,"===================================================\n");
    for(ii=1;ii<=vou_count;ii++)
    {
      fprintf(fp," %2s %10s   %4d   %4d   %4d   %4d   %4d \n",vou[ii].vou_type,vou[ii].vou_name,vou[ii].rest_que,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    fprintf(fp,"===================================================\n");
    fclose(fp);

     fp1=fopen("/tmp/zzd","r");
     openlp();
     newline(3);
     while ( !feof(fp1)) 
     {
     	fgets(ddd,150,fp1);
       	printf("%s",ddd);
       	sleep(1);
     }
     newline(1);
     backpage();
     closelp(); 
     fclose(fp1);
      break;
  }
  return(0);
}

/*********************************************
������:get_i1_item(item)
��  ��:���ݿ�Ŀ�ŵõ���Ӧ�����±�
��  ��:
       long item; ��Ŀ��
**********************************************/
get_i1_item(item)
long item;
{
   short i;

   for (i=1;i<=item_count;i++)
   {
     if (sum_br[i].item_num==item)
       break;
   }
   if (i>item_count)
     return(-1);
   else
     return(i);
}

/*****************************************************************
������:up_vou(type,flag,num)
��  ��:���ݴ����޸Ķ�Ӧƾ֤����
��  ��:
       char type[2];����: F_�� S_�� P_����
       char flag[2];�޸����� R_��� U_ʹ�� K_���� C_���� T_����
       short num;   �޸�����
******************************************************************/
up_vou(type,flag,num)
char type[2],flag[2];
short num;
{
  short i;
  note("���ݶ�Ӧ�����ҵ�ƾ֤�����еĶ�Ӧ�±�!");
  for(i=1;i<=vou_count;i++)
  {
    if (vou[i].type[0]==type[0])
      break;
  }
  if (i>vou_count)
  {
    dis_str(22,22,"�޶�Ӧƾ֤����!",60,0);
    getch();
    return(-1);
  }

  switch (flag[0])
  {
    case 'R':
      vou[i].rest_que+=num;
      break;
    case 'U':
      vou[i].use_que+=num;
      vou[i].rest_que-=num;
      break;
    case 'K':
      vou[i].cancel+=num;
      vou[i].rest_que-=num;
      break;
    case 'C':
      vou[i].chg_book+=num;
      vou[i].rest_que-=num;
      break;
    case 'T':
      vou[i].takeback+=num;
      vou[i].rest_que-=num;
      break;
  }
  return(0);
}
