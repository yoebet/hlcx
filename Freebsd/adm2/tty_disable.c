/****************************************************************
  程序名:tty_disable.c                                           
  函数名:tty_disable()                                          
  功  能:关闭终端
****************************************************************/
#include<stdio.h>
#include<string.h>
#include<curses.h>
#include "../include/apt.h"
tty_disable()
{
  FILE *fp;
  short pid,i,j,flag,n;
  char tmp_str[100],tmp_tty[3];
  char tty_str[40],pid_str[7],sub_str[7];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《关闭终端》");  
  refresh();
  move(10,25); addstr("请输入终端号:[   ]");
  refresh();
  dis_str(22,10,"若终端号为 tty21a 则输入 21a !",0,0); 
  strcpy(tmp_tty," ");
  acc_str(10,39,&tmp_tty,3);
  if (atoi(tmp_tty)==0)
  {
    dis_err("输入错误!"); 
    return(-1);
  }
  sprintf(tmp_str,"disable /dev/tty%s 2>/dev/null",tmp_tty);
  system(tmp_str);
  clear_l22();
  dis_err("指定终端已关闭,按任一键返回!"); 
  return(0);
}
