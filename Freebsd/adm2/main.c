/*******************
������:adm_main.c
������:main()
��  ��:�������س���
*******************/              
#include<stdio.h>
#include<curses.h>
#include<ctype.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

char sys_err_str[1024];
char sys_op_name[9];
short sys_operator;    
long sys_today;
short br_id[150];

MYSQL *conn=NULL;
char sql[1024];

char database[7]="hlcxdb";
char dbserv[16]="localhost";
char dbuser[10]="h";
char dbpass[10]="0";


MYSQL *connectdb(MYSQL *conn){

  char dbpass2[10];
  cry_num_r(dbpass,dbpass2);
  mysql_real_connect(conn,dbserv,dbuser,dbpass2,database,0,NULL,0);
  
  if(mysql_query(conn,"set names gb2312")){
    fprintf(stderr,"%s\n",mysql_error(conn));
    return(0);
  }
  return conn;
}


main() 
{
  struct f_operator op;
  char pwdsi[12],pwdli[7];
  char ttyno[7],pwdso[12];
  long cnt;
  short l;
  short op_no,pid;
  char confirm[2];
  long tmp_var;
  char *tty_name;
  FILE *scfp;
  char scname[12],scvalue[20];
  
  
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  conn=mysql_init(NULL);
  
  system("stty sane");
  signal(SIGINT,SIG_IGN);
  initscr();raw();noecho();keypad(stdscr,TRUE); 
  rtoday(&sys_today);
  
  scfp=fopen("./sc","r");
  if(scfp==(FILE *)NULL)
  {
    dis_str(22,22,"�����ô�!",0,0);
    keypad(stdscr,FALSE);echo();noraw();endwin();
    return(1);
  }
  while(fscanf(scfp,"%6s=%s\n",&scname,&scvalue)==2)
  {
  	if(strcmp(scname,"dbserv")==0){
  		strcpy(dbserv,scvalue);
  	}else if(strcmp(scname,"dbuser")==0){
  		strcpy(dbuser,scvalue);
  	}else if(strcmp(scname,"dbpass")==0){
  		strcpy(dbpass,scvalue);
  	}
  }
  fclose(scfp);
  
  if(!connectdb(conn)){
    sprintf(sys_err_str,"�򿪿��!%s",mysql_error(conn));
    dis_str(22,21,sys_err_str,60,0);
    keypad(stdscr,FALSE);echo();noraw();endwin();
    return(1);
  }
  
  while(1)
  {
    clear_scr(0,24,0,79);
    move( 6,24); addstr("    ҵ���̨ǩ������");
    move( 7,24); addstr("=========================");
    move(10,24); addstr("    ����Ա��: [  ]");
    move(12,24); addstr("    ��    ��: [      ]");
    move(14,24); addstr("    ȷ    ��: [ ]");
    touchwin(stdscr);
    refresh();

    tmp_var=0;
    while(tmp_var<=0||tmp_var>99) 
    {
      dis_str(22,18,"���ڹ�괦�������Ա��(99�˳���.",0,0); 
      acc_long(10,39,&tmp_var,2);
    }
    sys_operator=tmp_var;
    op_no=sys_operator;
    if (sys_operator==99)
    {
      mysql_close(conn);
      keypad(stdscr,FALSE);echo();noraw();endwin();
      system("clear"); 
      exit(0);
    }

    pwdli[0]='\0';
    while(pwdli[0]=='\0') 
    {
      dis_str(22,18,"���ڹ�괦��������Ա����.",0,0);  
      acc_pwd(12,39,pwdli,6);
    }
    pwdli[6]='\0';

    confirm[0]='\0';
    dis_str(22,18,"������y(n�˳�).",0,0);
    while(confirm[0]!='Y'&&confirm[0]!='y'&&confirm[0]!='N'&&confirm[0]!='n') 
    { 
      while(acc_str(14,39,confirm,1)==Err);
    }
    if(confirm[0]!='Y'&&confirm[0]!='y') 
    {
      mysql_close(conn);
      keypad(stdscr,FALSE);echo();noraw();endwin();
      system("clear"); 
      exit(0);
    }
    clear_line(22,10,78);
	
	  sprintf(sql,"SELECT * FROM operfile WHERE  operator= %d ",op_no);
  	
    if(mysql_query(conn,sql)){
      fprintf(stderr,"%s\n",mysql_error(conn));
      mysql_close(conn);//����������ط���Ҫ�ر�����
      return(-2);
    }
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	    set_f_operator(&op,row);
    }

    mysql_free_result(res);

    if(row==NULL) 
    {
      sprintf(sys_err_str,"ָ������Ա������");
      dis_str(22,24,sys_err_str,60,0);
      getch();
      continue;
    }
    if(mysql_errno(conn)!=0) 
    {
      dis_str(22,24,"������Ա��¼��!",0,0); 
      getch();
      continue;
    }
	
    if(op.op_flag[0]!='A') 
    {
      dis_str(22,24,"������Ա������!",0,0); 
      getch();
      continue;
    }
    if(op.op_level[0]!='7') 
    {
      dis_str(22,24,"Ȩ�޴���!",0,0); 
      getch();
      continue;
    }
	  cry_pwd(pwdli,pwdsi,op_no); 
    if(strncmp(op.password,pwdsi,8)!=0) 
    {
      dis_str(22,24,"�������!",0,0); 
      getch();
      continue;
    }
    strcpy(sys_op_name,op.op_name);
    pid=getpid();
    tty_name = ttyname(1);
    l=strlen(tty_name);
    //strncpy(ttyno,&tty_name[5],l-5);
    strncpy(ttyno,&tty_name[5],sizeof(ttyno)-1);
	  ttyno[sizeof(ttyno)-1]='\0';
	
	  sprintf(sql,"UPDATE operfile SET tty_no = '%s',tty_pid= %d WHERE operator= %d ",ttyno,pid,op_no);
	
    if(mysql_query(conn,sql)||mysql_errno(conn)){
      sprintf(sys_err_str,"�޸Ĳ���Ա��!%s",mysql_error(conn));
      dis_str(22,24,sys_err_str,60,0);
      getch();
      continue;
    }
	
    main_menu(0);
  }
}

main_menu(x) 
short x;
{
  static cenitem mainmenu[]=
  {
     3, 74,"�˳�", 
     3,  2,"ҵ�����",
     3, 12,"��Ա����",
     3, 22,"��Ŀά��",
     3, 32,"��ӡ����",
     3, 42,"���ʹ���",
     3, 52,"���ݱ���",
     3, 62,"�ն˹���",
  };
  short xx,choice;
  char date_str[11];

  choice=x;
  clear_scr(0,23,0,79);
  disp_main(1);
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(mainmenu,7,choice,0);
    xx=mainmenu[choice].x ;
    clear_scr(5,22,22,77);
    switch(choice)
    { 
      case 0:
        op_exit(xx);
        mysql_close(conn);//����������ط���Ҫ�ر�����
        keypad(stdscr,FALSE); echo(); noraw(); endwin();
        system("clear");
        exit(0);
      case 1:
        day_end(xx);
        break;
      case 2:
        sys_manage(xx);
        break;
      case 3:
        item_manage(xx);
        break;
      case 4:
        print_manage(xx);
        break;
      case 5:
        int_manage(xx);
        break;
      case 6:
        server(xx);
        break;
      case 7:
        tty_manage(xx);
        break;
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

day_end(no)
short no;
{
  static cenitem day_menu[]=
  {
   11,30,"0_��    ��",
    8,30,"1_ҵ��ʼ",
    //9,30,"2_�� �� ��",
    9,30,"3_ҵ�����",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(day_menu,2,choice,no);
    switch(choice)
    { 
      case 1:
        set_work_d();
        break;
      case 2:
        write_sum();
        break;
      case 3:
        end_br();
        break;
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

sys_manage(no)
short no;
{
  static cenitem sys_menu[]=
  {
   12,30,"0_��      ��",
    8,30,"1_���Ӳ���Ա",
    9,30,"2_ɾ������Ա",
   10,30,"3_����������",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(sys_menu,3,choice,no);
    switch(choice)
    { 
      case 1:
        add_oper();
        break;
      case 2:
        del_oper();
        break;
      case 3:
        update_pwd();
        break;
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}



item_manage(no)
short no;
{
  static cenitem item_menu[]=
  {
   17,30,"0_��      ��",
    8,30,"1_��ѯ��Ŀ��",
    9,30,"2_���ӿ�Ŀ��",
   10,30,"3_ɾ����Ŀ��",
   11,30,"4_�޸Ŀ�Ŀ��",
   12,30,"5_��ѯƾ֤��",
   13,30,"6_����ƾ֤��",
   14,30,"7_ɾ��ƾ֤��",
   15,30,"8_�޸�ƾ֤��",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(item_menu,8,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      case 1:
        query_item();
        break;
      case 2:
        add_item();
        break;
      case 3:
        del_item();
        break;
      case 4:
        update_item();
        break;
      case 5:
        query_vou();
        break;
      case 6:
        add_vou();
        break;
      case 7:
        del_vou();
        break;
      case 8:
        update_vouch();
        break;
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}



read_me(no)
short no;
{
  static cenitem re_menu[]=
  {
   10,30,"0_��    ��",
    8,30,"1_ҵ������",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(re_menu,1,choice,no);
    switch(choice)
    { 
       /*
      case 1:
        readme_code();
        break;
        */
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

print_manage(no)
short no;
{
  static cenitem print_menu[]=
  {
   17,30,"0_��      ��",
    8,30,"1_��ӡ��ˮ��",
    9,30,"2_��ӡ�սᵥ",
   10,30,"3_��ӡ����",
   11,30,"4_��ӡӪҵ��",
   12,30,"5_��ӡ��ϸ��",
   13,30,"6_��ӡ�ֻ���",
   14,30,"7_��ӡ������",
   15,30,"8_��ӡ������",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,5,77);
    choice=menuctl(print_menu,8,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      case 1:
        pr_daily();
        break;
      case 2:
        pr_total();
        break;
      case 3:
        pr_balance();
        break;
      case 4:
        pr_table();
        break;
      case 5:
        pr_detail();
        break;
      case 6:
        pr_account();
        break;
      case 7:
        pr_open();
        break;
      case 8:
        pr_close();
        break;
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

int_manage(no)
short no;
{
  static cenitem int_menu[]=
  {
   16,30,"0_��    ��",
    8,30,"1_���ʲ�ѯ",
    9,30,"2_��������",
   10,30,"3_����ɾ��",
   11,30,"4_���ʴ�ӡ",
   12,30,"5_���ڽ�Ϣ",
   13,30,"6_���Ϣ��",
   14,30,"7_��ͳ�Ƶ�",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(int_menu,7,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      case 1:
        rate_query();
        break;
      case 2:
        rate_add();
        break;
      case 3:
        rate_del();
        break;
      case 4:
        rate_print();
        break;
      case 5:
        rate_proc();
        break;
      case 6:
        rate_s_prt();
        break;
      case 7:
        rate_sum_prt();
        break;
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}


server(no)
short no;
{
  static cenitem int_menu[]=
  {
   12,30,"0_��        ��",
   8 ,30,"1_Ӳ�̱�������",
   10,30,"2_�����������",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(int_menu,2,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      case 1:
        backup_disk1();
        break;
      case 2:
        rbk();
        break;
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}


tty_manage(no)
short no;
{
  static cenitem int_menu[]=
  {
   15,30,"0_��        ��",
    8,30,"1_��ʾ��ն�",
    9,30,"2_��ֹ��ն�"
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,22,77);
    choice=menuctl(int_menu,2,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,22,77);
        return(0);
      case 1:
        tty_disp();
        break;
      case 2:
        tty_kill();
        break;
      /*
      case 3:
        tty_enable();
        break;
      case 4:
        tty_disable();
        break;
      case 5:
        pid_disp();
        break;
      case 6:
        pid_kill();
        break;
      */
      default:
        dis_str(22,22,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}



