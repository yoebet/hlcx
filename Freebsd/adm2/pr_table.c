/******************************************************************
  ������:pr_table.ec  
  ������:pr_table()
  ��  ��:��ӡ�ձ�,�±�,�걨
  ��  ��:2504
  ��  ��:
         1.��������(branch),��������(��/��/�걨)
         2.��鱾���Ƿ����
         3.���ô�ӡ����
  ��  ��:
         sflag:�ӳ������ó�(���ʱ��е�KIND)
                0_����
            1��12_ĳ��
               90_����
               99_����
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
    mvaddstr(5,30,"����ӡӪҵ��");  
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
      dis_str(22,22,"�������ʹ�!",60,0);
      getch();
      return(-1);
    }

    da.open_br=100;
    pr_tab_d(da.open_br,sflag);
    pr_vou_d(da.open_br,sflag);
    
    clear_scr(5,22,22,77);
  return(0);
}
