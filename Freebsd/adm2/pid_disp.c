
/****************************************************************
  ������:pid_disp.c                                          
  ������:pid_disp()                                         
  ��  ��:��ʾ�����
****************************************************************/
#include<stdio.h>
#include<curses.h>
pid_disp()
{
  FILE *fp;
  short i;
  char pid_str[40];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����ʾ����̡�");  
  refresh();
  system("ps -u hlcx >./tmp/tmp_pid 2>/dev/null");
  fp=fopen("./tmp/tmp_pid","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("�޻����!");
    return(1);
  }
  mvaddstr( 8,25,"  ����  �ն�     ʱ��      ����");
  mvaddstr( 9,25,"======================================");
  refresh();
  i=0;
  while( !feof(fp) )
  {
    fgets(pid_str,40, fp);
    mvaddstr(i+10,25,pid_str);
    refresh();
    i++;
    if (i==6 && !feof(fp) )
    {
      dis_err("����һ������!"); 
      clear_scr(10,22,22,77);
  mvaddstr(5,30,"����ʾ����̡�");  
  mvaddstr( 8,25,"  ����  �ն�     ʱ��      ����");
  mvaddstr( 9,25,"======================================");
  refresh();
      i=0;
    }  
  }
  fclose(fp);
  dis_err("��ն���ʾ���,����һ������!"); 
  return(0);
}
