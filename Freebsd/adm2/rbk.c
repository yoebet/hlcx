/****************************************************************                                           
  ��  ��:���ݵ�����
****************************************************************/              
#include<stdio.h>
#include<curses.h>
extern long sys_today;
rbk()
{
  char kk,_Inform[200];
  char str_date[9];
  extern char dbpass[];
  char dbpass2[10];

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"��������ݡ�");  
  refresh();

  dis_str(22,18,"Ҫ����ִ��Ӳ�̱��ݡ��س���ʼ�������,�� q ���˳�!",0,0);
  kk=getch();
  if (kk=='q' || kk=='Q')
    return(0);
  keypad(stdscr,FALSE);echo();noraw();endwin();
  system("clear");
  system("echo;echo;echo ���ڽ���,���Ժ�...");

  //���뱸������
  rfmtdate(sys_today, "yyyymmdd",str_date);
  cry_num_r(dbpass,dbpass2);
  
  sprintf(_Inform,"./rbk.sh %s",str_date); 
  if(system(_Inform) != 0)
  {
    system("echo;echo;echo ������ݳ���!");
    getch();
    system("stty sane");
    initscr();raw();noecho();keypad(stdscr,TRUE); 
    main_menu(7);
    return(-1);
  }

  system("echo;echo;echo ����������,����һ������.");
  getch();
  system("stty sane");
  initscr();raw();noecho();keypad(stdscr,TRUE);
	
  main_menu(7);
  return(1);
}
