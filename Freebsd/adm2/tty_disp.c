
/****************************************************************
  ������:tty_disp.c  
  ������:tty_disp() 
  ��  ��:��ʾ��ն�
****************************************************************/
#include<stdio.h>
#include<curses.h>
tty_disp()
{
  FILE *fp;
  short i;
  char tty_str[60];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����ʾ��նˡ�");  
  refresh();
  system("who|sed 's/       tty/tty/' >./tmp/tmp_tty 2>./tmp/tmp_core");
  fp=fopen("./tmp/tmp_tty","r");
  if(fp==(FILE *)NULL)
  {
    dis_err("��ָ���Ļ�ն�!");
    return(1);
  }
  mvaddstr( 8,15,"  ��¼�û�  ��¼�ն�        ��¼ʱ��");
  mvaddstr( 9,15,"===========================================");
  refresh();
  i=0;
  while( !feof(fp) )
  {
    fgets(tty_str,60, fp);
    mvaddstr(i+10,18,tty_str);
    refresh();
    i++;
    if (i==9 && !feof(fp) )
    {
      mvprintw(22,22,"����һ������!");
      i=0;
		  clear_scr(5,22,22,77);
		  mvaddstr(5,35,"����ʾ��նˡ�");  
		  mvaddstr( 8,15,"  ��¼�û�  ��¼�ն�        ��¼ʱ��");
		  mvaddstr( 9,15,"===========================================");
		  refresh();
    }  
  }
  fclose(fp);
  mvprintw(22,22,"��ʾ���,����һ������!");
    refresh();
  getchar();
  
  return(0);
}
