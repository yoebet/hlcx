/****************************************************************
������:check_sum.ec
������:check_sum()
��  ��:���������ֻ���һ����
��  ��:  
      2.��ָ���ı�data/def_check_sumȡ������˶ԵĿ�Ŀ
        ��Ŀ��  ��Ӧ����
      3.�ӿ�Ŀ�ļ���ȡ��Ŀ���Ʋ�����Ŀ�Ϸ���
      4.���ݿ�Ŀ�Ŷ�Ӧ���ִ�ָ�������ۼƱ���Ŀ�Ļ����ͽ��
      5.������90��ȡ����Ŀ���໧,���
      6.��4,5����Ľ��4,5������бȽ�
˵  ��:���ڼ����ʺ�,��ʼ��ǰִ�б�����.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[];
extern char sys_op_name[9];
extern short sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

short count=0;
FILE *fp,*fpe;
check_sum()
{
  char item_flag[2];
  char yy,cmd_str[50],file[50];
  long item_num;
  long item_num_1,item[100];
  short ymd[3],i,nn,debug=0,type,item_type[100];
  short branch,balind,homeind;
  double yes_bal;
  long bal;
  short yes_home,home_sum;

  static short in[2]={27,8};
  short num=2,flag;

  MYSQL_RES *res;
  MYSQL_ROW row;

  rjulmdy(sys_today,ymd);

    clear_scr(5,22,22,77);
    mvaddstr(5,30,"��������ֻ��˶ԡ�");
    refresh();
    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

    branch=100;

    if (debug==1)
    {
      mvprintw(21,22,"���ڴ�data/def_check_sum�ж���Ŀ�ż������Ϣ!");
      refresh();
      getch();
    }
    fp=fopen("./data/def_check_sum","r");
    if(fp==NULL) 
    {
      dis_err("���ܴ��ļ�/usr/hlcx/data/def_check_sum");
      return(0);
    }
    nn=0;
    while(fscanf(fp,"%1d %06ld\n",&type,&item_num_1)==2)
    {
      nn++;
      item[nn]=item_num_1;
      item_type[nn]=type;
    }
    fclose(fp);
    note("���ɳ����ļ�!");
    sprintf(file,"./tmp/check_sum_err");
    if((fpe=fopen(file,"w"))==NULL)
    {
      dis_str(22,22,"�������ɳ����ļ�!",60,0);
      getch();
      return(-1);
    }
    fprintf(fpe,"         �ֺܷ˶Գ����嵥  \n");
    fprintf(fpe,"������������������������������������������������������\n");
    fprintf(fpe,"��Ŀ��  ����ԭ��         ������   �ֻ�����    ��������\n");

    note("���δ���ÿ����Ŀ�ż������Ϣ!");
    for(i=1;i<=nn;i++)
    {
      item_num=item[i];
      type=item_type[i];
      sprintf(sys_err_str,"���ڴ����Ŀ:[%ld].",item_num);
      dis_str(22,22,sys_err_str,60,0);

      note("��item_file�ж���Ŀ�Ų���������Ϣ!");
      
	    sprintf(sql,"select item_flag from item_file where item_num=%ld",item_num);
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"ȡ��Ŀ��¼",mysql_errno(conn),0,0,0);
        continue;
      }
	    res=mysql_store_result(conn);
	    row=mysql_fetch_row(res);
      if(row==NULL)
      {
        write_e(item_num,"ȡ��Ŀ��¼",-1,0,0,0);
        continue;
      }
      strcpy(item_flag,row[0]);
      mysql_free_result(res);
      if (item_flag[0]!='A')
      {
        write_e(item_num,"��Ŀ������",0,0,0,0);
        continue;
      }

      note("���ݿ�Ŀ�Ŷ�Ӧ���ִ�ָ�������ۼƱ���Ŀ�Ļ����ͽ��");
      if (type==1)
      {
	      sprintf(sql,"select sum(balance),count(*) from s_mast\
	       where open_br=%d and item_num=%ld and acc_flag!='K' and acc_flag!='C'",branch,item_num);
      }
      else if (type==2)
      {
	      sprintf(sql,"select sum(balance),count(*) from f_mast\
	       where open_br=%d and item_num=%ld and acc_flag!='K' and acc_flag!='C'",branch,item_num);
      }
      else
      {
        write_e(item_num,"���ֶ����",0,0,0,0);
        continue;
      }
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"ȡ�����ۼ�����",mysql_errno(conn),0,0,0);
        continue;
      }
      res=mysql_store_result(conn);
      row=mysql_fetch_row(res);
      if(row!=NULL)
      {
	    	bal=atol(row[0]);
	    	home_sum=atoi(row[1]);
	      mysql_free_result(res);
      }
      else
      {
        bal = 0;
        home_sum = 0;
      }
      
      note("������90��ȡ����Ŀ���໧,���!");
	    sprintf(sql,"select balance,rest_que from sumtable where kind = 90\
	     and branch=%d and item_num=%ld",branch,item_num);
      if(mysql_query(conn,sql))
      {
        write_e(item_num,"ȡ�����ۼ�����",mysql_errno(conn),0,0,0);
        continue;
      }
      res=mysql_store_result(conn);
      row=mysql_fetch_row(res);
      if(row==NULL)
      {
        write_e(item_num,"ȡ�����ۼ�����",-1,0,0,0);
        continue;
      }
    	yes_bal=atof(row[0]);
    	yes_home=atoi(row[1]);
      mysql_free_result(res);
      
      if (yes_bal!=bal)
      {
        write_e(item_num,"����",0,bal,yes_bal,1);
      }
      if (yes_home!=home_sum)
      {
        write_e(item_num,"�໧����",0,home_sum,yes_home,0);
      }
    }
    fprintf(fpe,"������������������������������������������������������\n");
    fprintf(fpe,"   ��[%d]�ʳ����¼!",count);
    fprintf(fpe,"   ��ӡ����:%4d��%2d��%2d��\n",ymd[2],ymd[0],ymd[1]);
    fclose(fpe);
    clear_l22();
    dis_str(22,22,"�Ƿ��ӡ(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"��׼���ô�ӡ������һ����ʼ��ӡ",60,0);
      getch();
      sprintf(cmd_str,"pr %s > "Printer, file);
      system(cmd_str);
    }
  return(0);
}
/*************************************************
������:write_e(item_num,err_str,err_code,mast_bal,sum_bal,flag)
��  ��:д����г��ֵĴ�����Ϣ
��  ��:  
       item_num ��Ŀ��
       err_str  ����ԭ��
       err_code ������ 
       mast_bal �ֻ�����
       sum_bal  ��������
       flag     ��־ 0_��������  1_С������
*************************************************/
write_e(item_num,err_str,err_code,mast_bal,sum_bal,flag)
long item_num,mast_bal;
double sum_bal;
short err_code;
char err_str[16];
short flag;
{
  if (flag==1)
    fprintf(fpe,"%6ld  %-15s  %6ld %10.2lf  %10.2lf\n",item_num,err_str,err_code,mast_bal/100.00,sum_bal/100.00); 
  else
    fprintf(fpe,"%6ld  %-15s  %6ld     %6ld      %6ld\n",item_num,err_str,err_code,mast_bal,sum_bal); 
   count++;
}
