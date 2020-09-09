/**************************************************************
程序名:qtrade_ser.ec                                        *
函数名:qtrade_ser()                                         *
功  能:按交易号查询交易                                     *
代  码:1611
流  程:  
      1.输入
        起始交易号(ser_num1)
        终止交易号(ser_num2)
      3.从日志中取指定所的指定范围记录
      4.显示各记录
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "../include/sh_table.h"
#include "mysql.h"

extern char sys_err_str[96];
extern char sys_op_name[9];
extern short sys_branch,sys_operator;    
extern long sys_today,sys_work_d;

extern MYSQL *conn;
extern char sql[];

qtrade_ser()
{
  struct s_trade *tt;
  struct in_data da;
  char yy;
  static short in[3]={42,43,8};
  short num=3,flag;
  
  struct s_trade *(*pssa)[];
  int pssa_capacity=0;
  int pssa_count_max,pssa_count,pssa_cur;
  int no_more;
   
  MYSQL_RES *res;
  MYSQL_ROW row;

  init_in(&da);
  da.tr_op=sys_operator;
  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,22,"《按流水号查询流水》");
    refresh();
    flag=input_data(num,in,&da);
    if (flag<0)
     return(0);


    if (da.ser_num1>da.ser_num2)
    {
      dis_str(22,22,"起始交易号大于终止交易号",60,0);
      getch();
      continue;
    }

    if(pssa_capacity==0){
      int siz=sizeof(struct s_trade *(*)[]);//4   
      pssa=(struct s_trade *(*)[])malloc(siz*5);
      pssa_capacity=5;
    }
  
    sprintf(sql,"SELECT * FROM trade where branch=%d and ser_num>=%d and ser_num<=%d",
      da.tr_br,da.ser_num1,da.ser_num2);
  
  	if(mysql_query(conn,sql))
	  {
	    sprintf(sys_err_str,"查询流水出错!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      break;
	  }

	  res=mysql_use_result(conn);
	  row=mysql_fetch_row(res);
	  if(row==NULL){
      dis_str(22,22,"无满足条件的记录!",0,0);
      getch();
      break;
	  }
	
    no_more=0;
    pssa_cur=0;
    pssa_count=1;
    if(pssa_count_max==0){
    	pssa_count_max=1;
    }
    tt=(struct s_trade *)malloc(sizeof(struct s_trade));
    (*pssa)[pssa_cur]=tt;
    set_s_trade(tt,row);
	
    for(;;)
    {
      mvprintw( 6,40,"交 易 号=[%d]",tt->ser_num);
      disp_t(tt);
      dis_str(22,12,"请检查内容,按n_向后 p_向前 q_退出!",0,0);
      yy=getch();
      if (yy=='q' || yy=='Q')
        break;
      else if (yy=='p' || yy=='P')
      {
      	if(pssa_cur==0){
          printf("\007\007");
          dis_str(22,22,"已到第一记录!",0,0);
          getch();
      	}else{
      		pssa_cur--;
      		tt=(*pssa)[pssa_cur];
      	}
      }
      else if (yy=='n' || yy=='N')
      {
      	if(pssa_cur==pssa_count-1){
      		if(no_more){
            printf("\007\007");
            dis_str(22,22,"已到最后记录!",0,0);
            getch();
      		}else{
  	        row=mysql_fetch_row(res);
		        if(row==NULL){
	            printf("\007\007");
              dis_str(22,22,"已到最后记录!",0,0);
              no_more=1;
              getch();
  	        }else{
	  	        pssa_cur++;
    	        pssa_count++;
    	        if(pssa_count>pssa_capacity){
    	        	//扩展数组
    	        	pssa=(struct s_trade *(*)[])realloc(pssa,sizeof(struct s_trade)*(pssa_capacity*2));
    	        	pssa_capacity*=2;
    	        }
    	        if(pssa_count_max<pssa_count){
    	        	pssa_count_max=pssa_count;
    	        }
    	        tt=(struct s_trade*)malloc(sizeof(struct s_trade));
   	          (*pssa)[pssa_cur]=tt;
   	          set_s_trade(tt,row);
  	        }
	    		}
      	}else{
      		pssa_cur++;
      		tt=(*pssa)[pssa_cur];
      	}
      }
    }
    mysql_free_result(res);
    
    dis_err("处理完毕! ");  
    break;
  }
  
  if(pssa_capacity>0) {
    int si=0;
    for(;si<pssa_count;si++){
    	free((*pssa)[si]);
    }
    free(pssa);
  }
  return(0);
}
