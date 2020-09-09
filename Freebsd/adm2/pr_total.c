/****************************************************************
������:pr_total.ec
������:pr_total()
��  ��:����ָ�����ŵĿ�Ŀ�սᵥ
****************************************************************/

#include <stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

short item_count,vou_count;

struct to_sum sum[100];

struct to_vou vou[50];


pr_total()
{
  FILE *fp;
  struct s_trade tt;
  struct f_item  it;
  struct f_branch  br;
  struct f_voufile  vo;
  struct in_data da;
  char yy;
  static short in[2]={27,8};
  short num=2,flag;
 
  short itt,li,ii,nn,ii_s,ii101,ii164,iiint;
  short code,co12,co34;
  char cro[2],abst[2];
  short debug=0;
  char file[55],cmd_str[100];
  short branch_id;
  short inttax;
  long int_item;
  
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  while(1)//TODO: continue�������ѭ��
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"����ӡ��Ŀ�սᵥ��");
    refresh();
    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

    da.tr_br=100;
    branch_id=da.tr_br;

    note("ȡ�����ļ���ָ������¼");
	
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",branch_id);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	  set_f_branch(&br,row);
    }
    mysql_free_result(res);
  
    if(row==NULL) 
    {
      dis_str(22,22,"����������!",0,0);  
      getch();
      continue;
    }
    if(mysql_errno(conn))
    {
      sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,22,"����������!",0,0);
      getch();
      continue;
    }
	  
    if (debug==1)
    {
      mvprintw(21,22,"���ڳ�ʼ��ƾ֤����!");
      refresh();
      getch();
    }
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
	
	  sprintf(sql,"SELECT * FROM item_file WHERE  item_flag != 'K'");
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      getch();
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"���α��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
  
    nn=0;
	
    row=mysql_fetch_row(res);
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(mysql_errno(conn)!=0)
        break;
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
      sum[nn].yes_bal=0;
      sum[nn].today_bal=0;
	  //����С��100��
    }
    mysql_free_result(res);
	
    item_count=nn;
   
    note("��vou_file�ж�ƾ֤�������Ϣ!");
    nn=0;
	
	  sprintf(sql,"SELECT * FROM vou_file WHERE  flag != 'K'");
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"���α��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
  
    nn=0;
	
    row=mysql_fetch_row(res);
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(mysql_errno(conn)!=0)
        break;
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
    ii101=get_i_item_pt(101000);
    if (ii101<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[101000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    inttax=get_i_item_pt(532001);
    if (inttax<0)
    {
      sprintf(sys_err_str,"�޿�˰��Ŀ[532001]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    note("�õ�Ͻ��������Ŀ164000�������±�!");
    ii164=get_i_item_pt(164000);
    if (ii164<0)
    {
      sprintf(sys_err_str,"���ֽ��Ŀ[164000]��Ӧ�������±�!");
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }

    note("��trade��ȡ��ˮ!");
    sprintf(sql,"SELECT * FROM trade\
       WHERE branch = %d AND operator>=%d AND correct='A'\
       ORDER BY operator,ser_num",da.tr_br);	
	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      continue;
    }
	
    res=mysql_use_result(conn);

    if(mysql_errno(conn)!=0)
    {
      snprintf(sys_err_str,95,"���α��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      continue;
    }
	
  clear_scr(6,20,22,77);
  mvprintw(10,22,"              ��ǰ������ˮ����");
  mvprintw(11,22,"==================================================");
  mvprintw(13,22,"���� ��Ա ���׺� ������ ��־ ��Ŀ��  ���   ��Ϣ  ");
  mvprintw(16,22,"==================================================");
  refresh();
  
  row=mysql_fetch_row(res);
  for(;row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
	set_s_trade(&tt,row);
    mvprintw(14,22,"%3d   %2d   %4d   %4d   %1s   %6ld %7.2lf %6.2lf ", tt.branch,tt.tr_op,tt.ser_num,tt.tr_code,tt.correct,tt.item_num,tt.amount/100.00,tt.interest/100.00);
    refresh();
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

      note("���������λ����10,Ϊ������ҵ��,���账��!");
      if (co34>10)
      {
        note("������ҵ��!");
        continue;
      }
      if (co12<=15)
      {
        note("����ҵ����õ���ҵ���Ŀ�����±�!");
        ii=get_i_item_pt(tt.item_num);
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
          else if (abst[0]=='1')
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
          iiint=get_i_item_pt(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ����Ϣ��Ŀ1!",tt.item_num);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }

          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum[ii].j_x_num++;
            note("�跽�ֽ���");
            sum[ii].j_x_amt+=tt.amount;
            note("������");
            sum[ii].close_num++;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("�����ֽ����");
            sum[iiint].d_x_num++;
            note("�����ֽ���");
            sum[iiint].d_x_amt+=tt.interest;

            sum[inttax].j_x_num++;
            sum[inttax].j_x_amt+=tt.tax_int ;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum[ii101].d_x_num++;
            note("�����ֽ���");
            sum[ii101].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          else if (abst[0]=='1')
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii].j_z_amt+=tt.amount;
            note("������");
            sum[ii].close_num++;

            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;

            note("�Ǳ���Ŀ��Ӧ��Ϣ��Ŀ!");
            note("����ת�ʱ���");
            sum[iiint].d_z_num++;
            note("����ת�ʽ��");
            sum[iiint].d_z_amt+=tt.interest;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_x_num++;
            note("�����ֽ���");
            sum[ii164].d_x_amt+=tt.amount+tt.interest+tt.tax_int;
          }
          break;
        case 3:
          note("����ҵ��!");
          if (abst[0]=='0')
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
          else if (abst[0]=='1')
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
          if (abst[0]=='0')
          {
            note("��ԭ��Ŀ�ֽ�ҵ��!");
            note("�跽�ֽ����");
            sum[ii].j_x_num++;
            note("�跽�ֽ���");
            sum[ii].j_x_amt+=tt.amount;
            note("������");
            sum[ii].close_num++;

            note("���ֽ��Ŀҵ��!");
            note("�����ֽ����");
            sum[ii101].d_x_num++;
            note("�����ֽ���");
            sum[ii101].d_x_amt+=tt.amount;
          }
          else if (abst[0]=='1')
          {
            note("��ԭ��Ŀת��ҵ��!");
            note("�跽ת�ʱ���");
            sum[ii].j_z_num++;
            note("�跽ת�ʽ��");
            sum[ii].j_z_amt+=tt.amount;

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_x_num++;
            note("�����ֽ���");
            sum[ii164].d_x_amt+=tt.amount;
          }
          break;
        case 5:
          note("����ҵ��!");
          note("��ԭ���������ٿ��»�����!");

          note("��������Ŀ�Ķ�ת������Ϣ��Ŀ���±�!");
          int_item=found("S",tt.term,tt.item_num);
          if (ii_s<0)
          {
            sprintf(sys_err_str,"�޻��ڶ�Ӧ����Ϣ��Ŀ!");
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          ii_s=get_i_item_pt(int_item);
          if (iiint<0)
          {
            sprintf(sys_err_str,"�޿�Ŀ[%ld]��Ӧ����Ϣ��Ŀ!",int_item);
            dis_str(22,22,sys_err_str,60,0);
            getch();
            continue;
          }
          if (abst[0]=='0')
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
            note("�����ֽ����");
            sum[ii_s].d_x_num++;
            note("�����ֽ���");
            sum[ii_s].d_x_amt+=tt.interest;

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
          else if (abst[0]=='1')
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

            note("��Ͻ��������Ŀҵ��!");
            note("�����ֽ����");
            sum[ii164].d_z_num++;
            note("�����ֽ���");
            sum[ii164].d_z_amt+=tt.balance+tt.interest+tt.tax_int;
            sum[inttax].j_z_num++;
            sum[inttax].j_z_amt+=tt.tax_int ;


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
          ii=get_i_item_pt(tt.item_num);
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
	

    sprintf(file,"./tmp/tmp_total_%d",branch_id);
    if((fp=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"�������ɿ�Ŀ�սᵥ!",60,0);
      refresh();
      getch();       
      return(-1);
    }
    fprintf(fp,"                          ��Ŀ�սᵥ\n");
    fprintf(fp,"���������ש����������������������������������������������������������ש�����������������������������������������������������������\n");
    fprintf(fp,"��  ��  ��                  ��                  ��                  ��                  ��                  ��                  ��\n");
    fprintf(fp,"��      �ǩ������������������ש������������������ש������������������贈�����������������ש������������������ש�������������������\n");
    fprintf(fp,"��  Ŀ  ��     ��    ��     ��     ת    ��     ��     ��    ��     ��      ��    ��    ��      ת    ��    ��     ��    ��     ��\n");
    fprintf(fp,"��      �ǩ����ש������������贈���ש������������贈���ש������������贈���ש������������贈���ש������������贈���ש�������������\n");
    fprintf(fp,"��  ��  ��������   ������   ��������   ������   ��������   ������   ��������   ������   ��������   ������   ��������   ������   ��\n");

    note("��ʾͳ�ƽ��!");
    note("��ʾ����Ŀͳ�ƽ��!");
    for(ii=1;ii<=item_count;ii++)
    {
      li=6;
      note("��ǰ��Ŀ�޷������������!");
      if(sum[ii].j_z_num+sum[ii].j_x_num+sum[ii].j_z_amt+sum[ii].j_x_amt+sum[ii].d_z_num+sum[ii].d_x_num+sum[ii].d_z_amt+sum[ii].d_x_amt+sum[ii].open_num+sum[ii].close_num==0.00)
        continue;
      fprintf(fp,"�ǩ������贈���贈�����������贈���贈�����������贈���贈�����������贈���贈�����������贈���贈�����������贈���贈������������\n");
      fprintf(fp,"��%6ld��%4d��%12.2lf��%4d��%12.2lf��%4d��", sum[ii].item_num,sum[ii].j_x_num,sum[ii].j_x_amt/100.00,sum[ii].j_z_num,sum[ii].j_z_amt/100.00, sum[ii].j_x_num+sum[ii].j_z_num);
      fprintf(fp,"%12.2lf��%4d��%12.2lf��%4d��%12.2lf��%4d��%12.2lf��\n",(sum[ii].j_x_amt+sum[ii].j_z_amt)/100.00, sum[ii].d_x_num,sum[ii].d_x_amt/100.00,sum[ii].d_z_num,sum[ii].d_z_amt/100.00, sum[ii].d_x_num+sum[ii].d_z_num,(sum[ii].d_x_amt+sum[ii].d_z_amt)/100.00);

    }
    fprintf(fp,"���������ߩ����ߩ������������ߩ����ߩ������������ߩ����ߩ������������ߩ����ߩ������������ߩ����ߩ������������ߩ����ߩ�������������\n");
    note("��ʾ��ƾ֤ͳ�ƽ��!");
    fprintf(fp,"\n\n            ȫ��ƾ֤ʹ��ͳ��");
    fprintf(fp,"\n===================================================");
    fprintf(fp,"\n����  ��    ��  ʣ���� ʹ���� ������ ������ ������");
    fprintf(fp,"\n===================================================");
    for(ii=1;ii<=vou_count;ii++)
    {
/**
      if(vou[ii].rest_que+vou[ii].use_que+vou[ii].cancel+vou[ii].chg_book+vou[ii].takeback==0)
        continue;
**/
      fprintf(fp,"\n %2s %10s   %4d   %4d   %4d   %4d   %4d ",vou[ii].vou_type,vou[ii].vou_name,vou[ii].rest_que,vou[ii].use_que,vou[ii].cancel,vou[ii].chg_book,vou[ii].takeback);
    }
    fprintf(fp,"\n===================================================\n");
    fclose(fp);
    dis_str(22,22,"�Ƿ��ӡ(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"��׼���ô�ӡ������һ����ʼ��ӡ",60,0);
      getch();       
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
    break;
  }
  return(0);
}

/*****************************************************************
������:up_vou(type,flag,num)
��  ��:���ݴ����޸Ķ�Ӧƾ֤����
��  ��:
       char type[2];����: D_�� H_�� 
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

/***************************************
������:found(type,term,item)
��  ��:��������Ŀ�Ķ�ת��Ϣ��Ŀ
��  ��:
       char  type[2];����
       long  item;   ��Ŀ��
       short term;   ����
***************************************/
found(type,term,item)
char type[2];
long item;
short term;
{
  long int_item;
  short ii;

  note("���ݴ��ֺʹ��ھ�����Ŀ");

  switch(type[0])
  {
    case 'H':
      note("����"); 

      int_item=521100;
      break;
    case 'D':
      note("����"); 

      if (term<=6)
        int_item=521300;
      else
        int_item=521400;
      break;

    default:
      int_item=-1;
      break;
  }
  if (int_item<0)
    return(-1);
  else
    return(int_item);
}

get_i_item_pt(item)
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



