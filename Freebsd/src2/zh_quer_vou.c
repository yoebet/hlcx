/****************************************************************
������:zh_quer_vou.ec
������:zh_quer_vou()
��  ��:��ѯƾ֤
��  ��:1962
��  ��:  
      1.����
        ƾ֤���� (vou_type)
        ��־ (item_kind)
      2.��ָ����Ŀ��ƾ֤
      3.��ʾָ����Ŀ��ƾ֤
           
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

zh_quer_vou()
{
  struct f_vouch *vou;
  struct in_data da;
  char yy;
  static short in[3]={5,39,8};
  short num=3,flag;
  char vou_type[3];
  
  //ά��һ����̬���飬ģ��ɹ����α�
  struct f_vouch *(*pssa)[];
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
    mvaddstr(5,30,"��ƾ֤��ѯ��");  
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);

    note("ת��ƾ֤����!");
    switch (da.vou_type[0])
    {
       case '2':
         strcpy(vou_type,"H0");
         break;
       case '3':
         strcpy(vou_type,"K0");
         break;
       default:
         dis_str(22,22,"����ƾ֤���ʹ�!",0,0); 
         break;
    }
    
   
    if(pssa_capacity==0){
      int siz=sizeof(struct f_vouch *(*)[]);//4   
      pssa=(struct f_vouch *(*)[])malloc(siz*5);
      pssa_capacity=5;
    }
  
    sprintf(sql,"SELECT * FROM vouch WHERE branch=%d and kind=%d\
      and vou_type='%s'",da.open_br,da.item_kind,vou_type);
  
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
    vou=(struct f_vouch *)malloc(sizeof(struct f_vouch));
    (*pssa)[pssa_cur]=vou;
    set_f_vouch(vou,row);
	
    for(;;)
    {
      disp_vouch(vou);
      dis_str(22,12,"�����ʻ�,��n_��� p_��ǰ q_�˳�!",0,0);
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
      		vou=(*pssa)[pssa_cur];
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
    	        	pssa=(struct f_vouch *(*)[])realloc(pssa,sizeof(struct f_vouch)*(pssa_capacity*2));
    	        	pssa_capacity*=2;
    	        }
    	        if(pssa_count_max<pssa_count){
    	        	pssa_count_max=pssa_count;
    	        }
    	        vou=(struct f_vouch*)malloc(sizeof(struct f_vouch));
   	          (*pssa)[pssa_cur]=vou;
   	          set_f_vouch(vou,row);
  	        }
	    		}
      	}else{
      		pssa_cur++;
      		vou=(*pssa)[pssa_cur];
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
