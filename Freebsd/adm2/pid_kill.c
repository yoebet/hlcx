/****************************************************************
  ������:pid_kill.c                                          
  ������:pid_kill()                                         
  ��  ��:ɱ��ĳ�����
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
  mvaddstr(5,30,"����ֹ���̡�");  
  refresh();
  dis_str(22,22,"������̺�(9999�˳�)!",0,0); 
  move(10,25); addstr("��������̺�:[    ]");
  refresh();
  acc_long(10,39,&pid,4);
  if (pid==9999)
    return(0);
  if (pid==0)
  {
    dis_err("ָ�����̺Ŵ���!"); 
    return(0);
  }
  sprintf(tmp_str,"kill -9 %d 2>/dev/null",pid);
  dis_err(tmp_str);
  system(tmp_str);
  clear_l22();
  dis_err("ָ����������ֹ,����һ������!"); 
  return(0);
}
