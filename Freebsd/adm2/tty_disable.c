/****************************************************************
  ������:tty_disable.c                                           
  ������:tty_disable()                                          
  ��  ��:�ر��ն�
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
  mvaddstr(5,30,"���ر��նˡ�");  
  refresh();
  move(10,25); addstr("�������ն˺�:[   ]");
  refresh();
  dis_str(22,10,"���ն˺�Ϊ tty21a ������ 21a !",0,0); 
  strcpy(tmp_tty," ");
  acc_str(10,39,&tmp_tty,3);
  if (atoi(tmp_tty)==0)
  {
    dis_err("�������!"); 
    return(-1);
  }
  sprintf(tmp_str,"disable /dev/tty%s 2>/dev/null",tmp_tty);
  system(tmp_str);
  clear_l22();
  dis_err("ָ���ն��ѹر�,����һ������!"); 
  return(0);
}
