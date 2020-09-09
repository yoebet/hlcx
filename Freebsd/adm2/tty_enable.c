/****************************************************************
  程序名:tty_enable.c                                          
  函数名:tty_enable()                                         
  功  能:激活终端
****************************************************************/
#include<stdio.h>
#include<string.h>
#include<curses.h>

  clear_l22()
{
  move(22,1); addstr("                                                                           ");
  refresh();
}

tty_enable()
{
  FILE *fp;
  short pid,i,j,flag,n;
  char tmp_str[100],tmp_tty[3];
  char tty_str[40],pid_str[7],sub_str[7];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"《打开终端》");  
  refresh();
  move(10,25); addstr("请输入终端号:[   ]");
  dis_str(22,12,"若终端号为 tty21a 则输入 21a,999退出!",0,0); 
  refresh();
  strcpy(tmp_tty," ");
  acc_str(10,39,&tmp_tty,3);
  if (atoi(tmp_tty)==999)
    return(0);
  if (atoi(tmp_tty)==0)
  {
    dis_err("输入错误!"); 
    return(-1);
  }
_CutSpace(tmp_tty);
  sprintf(tmp_str,"enable /dev/tty%s 2>/dev/null",tmp_tty);
  system(tmp_str);
  clear_l22();
  dis_err("指定终端已打开,按任一键返回!"); 
  return(0);
}
