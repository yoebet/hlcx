/****************************************************************
������:zh_quer_sum.ec
������:zh_quer_sum()
��  ��:��ѯ����
��  ��:1961
��  ��:  
      1.����
        ��Ŀ�� (item)
        ��־ (item_kind)
      2.����Ŀ�Ƿ����
      3.��ָ����Ŀ������
      4.��ʾָ����Ŀ������
           
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

zh_quer_sum()
{
  struct f_sumtable *sum;
  struct in_data da;
  char yy;
  static short in[3]={24,39,8};
  short num=3,flag;
  long item1,item2;
  
  //ά��һ����̬���飬ģ��ɹ����α�
  struct f_sumtable *(*pssa)[];
  int pssa_capacity=0;
  int pssa_count_max,pssa_count,pssa_cur;
  int no_more;
   
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
    mvaddstr(5,30,"�����ʲ�ѯ��");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    if (da.item==999999)
    {
      item1=100000;
      item2=999999;
    }
    else
    {
      item1=da.item;
      item2=da.item;
    }
   
    if(pssa_capacity==0){
      int siz=sizeof(struct f_sumtable *(*)[]);//4   
      pssa=(struct f_sumtable *(*)[])malloc(siz*5);
      pssa_capacity=5;
    }
  
    sprintf(sql,"SELECT * FROM sumtable WHERE branch=%d and kind=%d\
      and item_num>=%ld and item_num <=%ld",da.open_br,da.item_kind,item1,item2);
  
  	if(mysql_query(conn,sql))
	  {
	    sprintf(sys_err_str,"��ѯ���˳���!%s",mysql_error(conn));
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
    sum=(struct f_sumtable *)malloc(sizeof(struct f_sumtable));
    (*pssa)[pssa_cur]=sum;
    set_f_sumtable(sum,row);
	
    for(;;)
    {
      disp_sumtable(sum);
      dis_str(22,22,"�����¼����,��n_��� p_��ǰ q_�˳�!",0,0);
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
      		sum=(*pssa)[pssa_cur];
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
  	          	pssa=(struct f_sumtable *(*)[])realloc(pssa,sizeof(struct f_sumtable)*(pssa_capacity*2));
  	          	pssa_capacity*=2;
  	          }
  	          if(pssa_count_max<pssa_count){
  	          	pssa_count_max=pssa_count;
  	          }
  	          sum=(struct f_sumtable*)malloc(sizeof(struct f_sumtable));
 	            (*pssa)[pssa_cur]=sum;
 	            set_f_sumtable(sum,row);
	          }
      		}
      	}else{
      		pssa_cur++;
      		sum=(*pssa)[pssa_cur];
      	}
      }
    }
    mysql_free_result(res);
  
    if(pssa_capacity>0) {
      int si=0;
      for(;si<pssa_count;si++){
      	free((*pssa)[si]);
      }
      free(pssa);
    }
    dis_err("�������! ");  
    break;
  }
  return(0);
}
