/**************************************************************
������:rate_s_prt.ec 
������:rate_s_prt() 
��  ��:���ڽ�Ϣ�嵥��ӡ 
**************************************************************/              
#include <stdio.h>
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern short sys_operator;    
extern long sys_today;

rate_s_prt()
{
  static short in[2]={26,8};
  short num=2,flag;
  struct in_data da;
  char yy,str[100];

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,35,"����ӡ���ڽ�Ϣ�嵥��");
  refresh();
  dis_str(22,22,"��׼���ô�ӡ������һ����ʼ��ӡ",60,0);
  getch();
  //da.open_br=100;

  sprintf(str,"pr ./tmp/s_int_* > "Printer" 2>/dev/null");
  if(system(str)!=0) 
  {
    dis_err("��Ϣ�嵥������");
    return(1);
  }
  
  clear_l22();
  dis_err("�������! ");
  
  return(0);
}
