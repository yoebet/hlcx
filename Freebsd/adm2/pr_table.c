/******************************************************************
  程序名:pr_table.ec  
  函数名:pr_table()
  功  能:打印日报,月报,年报
  代  码:2504
  流  程:
         1.输入所号(branch),报表类型(日/月/年报)
         2.检查本所是否存在
         3.调用打印函数
  参  数:
         sflag:从程序计算得出(总帐表中的KIND)
                0_上年
            1～12_某月
               90_本日
               99_本年
*******************************************************************/
#include<stdio.h>

#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;

pr_table()
{
  short sflag;
  short br_flag;
  char yy;

  static short in0[2]={53,8};
  static short in1[2]={54,8};
  short num0=2,num1=2,flag;
  struct in_data da;

  init_in(&da);
  
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"《打印营业表》");  
    refresh();
    flag=input_data(num0,in0,&da);
    if (flag<0)
     return(0);

    if(da.bal_flag==1)
      sflag=90;
    else if(da.bal_flag==2)
    {
      clear_scr(5,22,22,77);
      refresh();
      flag=input_data(num1,in1,&da);
      if (flag<0)
        return(0);
      sflag=da.month;
    }
    else if(da.bal_flag==3)
      sflag=99;
    else
    {
      dis_str(22,22,"报表类型错!",60,0);
      getch();
      return(-1);
    }

    da.open_br=100;
    pr_tab_d(da.open_br,sflag);
    pr_vou_d(da.open_br,sflag);
    
    clear_scr(5,22,22,77);
  return(0);
}
