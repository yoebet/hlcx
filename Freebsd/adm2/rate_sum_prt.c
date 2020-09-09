/**************************************************************
程序名:rate_sum_prt.ec 
函数名:rate_sum_prt() 
功  能:活期结息统计清单打印 
**************************************************************/              
#include <stdio.h>
#include "../include/apt.h"
#include "../include/sh_table.h"
extern char sys_err_str[96];
extern short sys_operator;    
extern long sys_today;

rate_sum_prt()
{
  static short in[2]={26,8};
  short num=2,flag;
  struct in_data da;
  char yy,str[100];

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《打印活期结息统计清单》");
  refresh();
  dis_str(22,22,"请准备好打印机后按任一键开始打印",60,0);
  getch();
  da.open_br=100;
  if(da.open_br!=999)
  {
    sprintf(str,"pr ./tmp/s_int_sum_%03d > "Printer" 2>/dev/null",da.open_br);
    if(system(str)!=0) 
    {
      dis_err("结息清单不存在");
      return(1);
    }
  }
  else
  {
    sprintf(str,"pr ./tmp/s_int_sum_* > "Printer" 2>/dev/null");
    if(system(str)!=0) 
    {
      dis_err("结息清单不存在!");
      return(1);
    }
  }
  clear_l22();
  dis_err("处理完毕!");
  
  return(0);
}
