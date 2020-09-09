/****************************************************************
  程序名:pid_kill.c                                          
  函数名:pid_kill()                                         
  功  能:杀死某活动进程
****************************************************************/
#include<stdio.h>
#include<curses.h>
#include "../include/apt.h"
pid_kill()
{
  FILE *fp;
  long pid;
  char tmp_str[100];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《终止进程》");  
  refresh();
  dis_str(22,22,"输入进程号(9999退出)!",0,0); 
  move(10,25); addstr("请输入进程号:[    ]");
  refresh();
  acc_long(10,39,&pid,4);
  if (pid==9999)
    return(0);
  if (pid==0)
  {
    dis_err("指定进程号错误!"); 
    return(0);
  }
  sprintf(tmp_str,"kill -9 %d 2>/dev/null",pid);
  dis_err(tmp_str);
  system(tmp_str);
  clear_l22();
  dis_err("指定进程已终止,按任一键返回!"); 
  return(0);
}
