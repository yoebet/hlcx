/****************************************************************
������:s_query.ec
������:s_query()
��  ��:���ڲ�ѯ
��  ��:1561
��  ��:  
      1.����
        ��ѯ��ʽ(q_flag)
        ��ʼ�ʺ� (account1)
        ��ֹ�ʺ� (account2)
        ���� (name)
        ��� (balance)
      2.��鱾����Ա�Ƿ�ල����
      3.ȡ���ʺ�/�����ķֻ���
      4.��ʾ��������ɷֻ�
           
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
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

MYSQL *conn2;

void _clscon_s(){
	if(conn2!=NULL)mysql_close(conn2);
}

s_query()
{
  struct ss_mast *ss;
  struct in_data da;
  char yy;
  static short in[1]={35};
  static short in0[3]={70,8};
  static short in1[2]={16,8};
  static short in2[2]={10,8};
  short num=1,num1=2,num0=2,flag;
  short op_no;
  char tmp_name[9];
  short i,j,l;
  
  //ά��һ����̬���飬ģ��ɹ����α�
  struct ss_mast *(*pssa)[];
  int pssa_capacity=0,pssa_count_max=0,pssa_count=0,pssa_cur=0;
  int no_more;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.open_br=sys_branch;
  da.open_d=sys_work_d;
  da.tr_br=sys_branch;
while(1)
{
  clear_scr(5,22,22,77);
  mvaddstr(5,30,"�������ʻ���ѯ��");  
  refresh();
  da.open_br=sys_branch;
  da.tr_br=sys_branch;

  flag=input_data(num,in,&da);
  if (flag<0)
    return(0);

  clear_scr(6,22,22,77);
  if (da.q_flag[0]=='1')
  {
    flag=input_data(num0,in0,&da);
    if (flag<0) return(0);
  }
  else if (da.q_flag[0]=='2')
  {
    flag=input_data(num1,in2,&da);
    if (flag<0) return(0);
  }
  else if (da.q_flag[0]=='3')
  {
    flag=input_data(num1,in1,&da);
    if (flag<0) return(0);
  }
  
  if(pssa_capacity==0){
    int siz=sizeof(struct ss_mast *(*)[]);
  	pssa=(struct ss_mast *(*)[])malloc(siz*5);
  	pssa_capacity=5;
  }

  if (atoi(da.q_flag)==1)
  {
    note("���ʺŲ�ѯ�ʻ�!");
    sprintf(sql,"SELECT * FROM s_mast WHERE open_br=%d AND account=%ld",da.open_br,da.account);
  }
  else if (atoi(da.q_flag)==2)
  {
    note("������ѯ�ʻ�!");
    sprintf(sql,"SELECT * FROM s_mast WHERE open_br=%d AND balance=%ld",da.open_br,da.balance);
  }
  else if (atoi(da.q_flag)==3)
  {
    note("��������ѯ�ʻ�!");
    sprintf(sql,"SELECT * FROM s_mast WHERE open_br=%d AND acc_type='P' AND customer like '%%%s%%'",da.open_br,da.customer);
  }
  
	if(mysql_query(conn,sql))
	{
	  sprintf(sys_err_str,"��ѯ�˻���!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    continue;
	}

	res=mysql_use_result(conn);
	
	row=mysql_fetch_row(res);
	if(row==NULL){
    dis_str(22,22,"�����������ļ�¼!",0,0);
    getch();
    continue;
	}
	no_more=0;
  pssa_cur=0;
  pssa_count=1;
  if(pssa_count_max==0){
  	pssa_count_max=1;
  }
  ss=(struct ss_mast *)malloc(sizeof(struct ss_mast));
  (*pssa)[pssa_cur]=ss;
  set_ss_mast(ss,row);
  
  for(;;)
  {
    disp_s_acc(ss,0);
    dis_str(22,12,"�����ʻ�,��n_��� p_��ǰ d_����ϸ q_�˳�!",0,0);
    yy=getch();
    if (yy=='q' || yy=='Q')
      break;
    else if (yy=='p' || yy=='P')
    {
    	if(pssa_cur==0){
        printf("\007\007");
        dis_str(22,22,"�ѵ���һ��¼!",0,0);
        getch();
    	}else{
    		pssa_cur--;
    		ss=(*pssa)[pssa_cur];
    	}
    }
    else if (yy=='n' || yy=='N')
    {
    	if(pssa_cur==pssa_count-1){
    		if(no_more){
          printf("\007\007");
          dis_str(22,22,"�ѵ�����¼!",0,0);
          getch();
    		}else{
	        row=mysql_fetch_row(res);
	        if(row==NULL){
            printf("\007\007");
            dis_str(22,22,"�ѵ�����¼!",0,0);
            no_more=1;
            getch();
	        }else{
  	        pssa_cur++;
  	        pssa_count++;
  	        if(pssa_count>pssa_capacity){
  	        	//��չ����
  	        	pssa=(struct ss_mast *(*)[])realloc(pssa,sizeof(struct ss_mast)*(pssa_capacity*2));
  	        	pssa_capacity*=2;
  	        }
  	        if(pssa_count_max<pssa_count){
  	        	pssa_count_max=pssa_count;
  	        }
  	        ss=(struct ss_mast*)malloc(sizeof(struct ss_mast));
 	          (*pssa)[pssa_cur]=ss;
 	          set_ss_mast(ss,row);
	        }
    		}
    	}else{
    		pssa_cur++;
    		ss=(*pssa)[pssa_cur];
    	}
    }
    else if (yy=='d' || yy=='D')
    {
      if(conn2==NULL){
      	//����conn��ѯ��û�����������ٴ�һ������
        conn2=mysql_init(NULL);
        atexit(_clscon_s);
        if(!connectdb(conn2)){
	        sprintf(sys_err_str,"ȡ��ϸ��!%s",mysql_error(conn2));
          dis_str(22,22,sys_err_str,60,0);
          getch();
        }
      }
      if(conn2!=NULL){
        disp_s_det(ss->account,ss->open_br,conn2);
      }
    }
  }
  mysql_free_result(res);
  
  //dis_err("�������! ");  
  break;
}

  if(pssa_capacity>0) {
    int si=0;
    for(;si<pssa_count_max;si++){
    	free((*pssa)[si]);
    }
    free(pssa);
  }

  return(0);
}


/***************************
������:disp_s_det(acc,br)
��  ��:��ʾ������ϸ
��  ��:
       short br  ����
       long acc  �ʺ�
����ֵ:
***************************/
disp_s_det(acc,br,conn)
long acc;
short br;
MYSQL *conn;
{
  struct ss_detail *sd;
  char yyy,str[20];
  short ll=8;
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  //ά��һ����̬���飬ģ��ɹ����α�
  struct ss_detail *(*psda)[];
  int psda_capacity=0;
  int psda_count,psda_cur;
  int no_more;

  clear_scr(5,22,22,77);
  
  sprintf(sql,"SELECT * FROM s_detail WHERE account=%ld AND open_br=%d order by det_num",acc,br);
  
	if(mysql_query(conn,sql))
	{
	  sprintf(sys_err_str,"ȡ��ϸ��!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}

	res=mysql_use_result(conn);
	row=mysql_fetch_row(res);
	if(row==NULL){
	  sprintf(sys_err_str,"ȡ��ϸ��!%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
	}
	
  int siz=sizeof(struct ss_detail *(*)[]);
  psda=(struct ss_detail *(*)[])malloc(siz*5);
  psda_capacity=5;
	
	no_more=0;
  psda_cur=0;
  psda_count=1;
  sd=(struct ss_detail *)malloc(sizeof(struct ss_detail));
  (*psda)[psda_cur]=sd;
  set_ss_detail(sd,row);
  
  for(;;)
  {
    mvprintw(ll+ 2,32,"��    ��=[%ld]",sd->account);
    rfmtdate(sd->tr_date,"yyyy/mm/dd",str);
    mvprintw(ll+ 3,32,"ҵ �� ��=[%s]",str);	
    mvprintw(ll+ 4,32,"�� �� ��=[%8.2lf]",sd->amount/100.00);
    if (sd->flag[0]=='O')
      strcpy(str,"����");
    else if (sd->flag[0]=='D')
      strcpy(str,"����");
    else if (sd->flag[0]=='W')
      strcpy(str,"֧ȡ");
    else if (sd->flag[0]=='C')
      strcpy(str,"����");
    else if (sd->flag[0]=='K')
      strcpy(str,"����");
    else
      strcpy(str,"δ����");
    mvprintw(ll+ 6,32,"ҵ������=[%s]",str);
    mvprintw(ll+ 7,32,"ҵ���Ա=[%d]",sd->tr_op);
    mvprintw(ll+ 8,32,"��ϸ����=[%d]",sd->det_num);
    refresh();
    dis_str(22,22,"������ϸ,��n_��� p_��ǰ q_����!",0,0);
    yyy=getch();
    if (yyy=='q' || yyy=='Q')
      break;
    else if (yyy=='p' || yyy=='P')
    {
    	if(psda_cur==0){
        printf("\007\007");
        dis_str(22,22,"�ѵ���һ��¼!",0,0);
        getch();
    	}else{
    		psda_cur--;
    		sd=(*psda)[psda_cur];
    	}
    }
    else if (yyy=='n' || yyy=='N')
    {
    	if(psda_cur==psda_count-1){
    		if(no_more){
          printf("\007\007");
          dis_str(22,22,"�ѵ�����¼!",0,0);
          getch();
    		}else{
	        row=mysql_fetch_row(res);
	        if(row==NULL){
            printf("\007\007");
            dis_str(22,22,"�ѵ�����¼!",0,0);
            no_more=1;
            getch();
	        }else{
  	        psda_cur++;
  	        psda_count++;
  	        if(psda_count>psda_capacity){
  	        	//��չ����
  	        	psda=(struct ss_detail *(*)[])realloc(psda,sizeof(struct ss_detail)*(psda_capacity*2));
  	        	psda_capacity*=2;
  	        }
  	        sd=(struct ss_detail *)malloc(sizeof(struct ss_detail));
 	          (*psda)[psda_cur]=sd;
 	          set_ss_detail(sd,row);
	        }
    		}
    	}else{
    		psda_cur++;
    		sd=(*psda)[psda_cur];
    	}
    }
  }
  mysql_free_result(res);
  
  if(psda_capacity>0) {
    int si=0;
    for(;si<psda_count;si++){
    	free((*psda)[si]);
    }
    free(psda);
  }
  
  return(0);
}
