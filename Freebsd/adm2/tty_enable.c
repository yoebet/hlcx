/****************************************************************
  ������:tty_enable.c                                          
  ������:tty_enable()                                         
  ��  ��:�����ն�
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
  mvaddstr(5,30,"�����նˡ�");  
  refresh();
  move(10,25); addstr("�������ն˺�:[   ]");
  dis_str(22,12,"���ն˺�Ϊ tty21a ������ 21a,999�˳�!",0,0); 
  refresh();
  strcpy(tmp_tty," ");
  acc_str(10,39,&tmp_tty,3);
  if (atoi(tmp_tty)==999)
    return(0);
  if (atoi(tmp_tty)==0)
  {
    dis_err("�������!"); 
    return(-1);
  }
_CutSpace(tmp_tty);
  sprintf(tmp_str,"enable /dev/tty%s 2>/dev/null",tmp_tty);
  system(tmp_str);
  clear_l22();
  dis_err("ָ���ն��Ѵ�,����һ������!"); 
  return(0);
}
