/**************************************************************
  ������:op_main.c 
  ������:main()    
**************************************************************/              
#include<stdio.h>
#include<string.h>
#include<curses.h>
#include<ctype.h>
#include<time.h>
#include<signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/apt.h"
#include "../include/sh_table.h"
#include "mysql.h"

char sys_err_str[1024];
char sys_op_name[9],sys_op_level[2];
char in_flag;

MYSQL *conn=NULL;
char sql[1024];

char database[7]="hlcxdb";
char dbserv[16]="localhost";
char dbuser[10]="h";
char dbpass[10]="0";

short sys_branch,sys_operator,sys_code,sys_sernum;    
long sys_today,sys_work_d,sys_loc_no;
struct in_data da;


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
  struct f_branch br;

  short op_br,op_no;
  char pwdsi[9],pwdli[7],pwdso[9];
  char ttyno[7],op_d[11];
  char *tty_name,yn[2],tmp_ymd[11];
  long sys_d,tmp_var;
  short ymd[3],flag,kk,l;
  long pid;
  char yy;
  char scname[12],scvalue[20];
  FILE *scfp;
  
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
  
	op_br=100;
  while(1)
  {
    clear_scr(0,24,0,79);
    mvaddstr(6,15,"�����������й�̨ҵ��ϵͳǩ������");
    mvaddstr(11,24,"����Ա��:[  ]");
    mvaddstr(12,24,"��    ��:[      ]");
    mvaddstr(13,24,"ҵ������:[          ]");
    mvaddstr(15,24,"ȷ    ��:[ ]");
    refresh();
    kk=1;
    while(kk!=5&&kk!=0)/** ��Աǩ�� **/
    {
      tmp_var=0;
      switch(kk)
      {
   	case 1:
          mvaddstr(11,24,"����Ա��:[  ]");
          dis_str(21,18,"���������Ա��(99�˳�).",0,0); 
          while((flag=acc_long(11,34,&tmp_var,2))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          op_no=(short)tmp_var;
          if (op_no==99)
          {
            keypad(stdscr,FALSE);echo();noraw();endwin();
            system("clear");
            exit(0);
          }
          if (op_no==0)
          {
            dis_str(22,18,"�������Ա�Ŵ�!",0,0); 
            getch();
            break;
          }
          sys_operator=op_no;
          kk++;
          break;
   	case 2:
          mvaddstr(12,24,"��    ��:[      ]");
          dis_str(21,18,"���������Ա����.",0,0); 
          pwdli[0]='\0';
          while((flag=acc_pwd(12,34,pwdli,6))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(pwdli[0]=='\0') 
            break;
          kk++;
          break;
        case 3:
          mvaddstr(13,24,"ҵ������:[          ]");
          dis_str(21,18,"������ҵ������(��������/����/����).",0,0); 
          op_d[0]=0;
          while((flag=acc_date(13,34,op_d,10))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(op_d[0]==0)
            break;
          kk++;
          break;
        case 4:
          mvaddstr(15,24,"ȷ    ��:[ ]");
          dis_str(21,18,"������ y �� n.",0,0); 
          yn[0]=0;
          while((flag=acc_str(15,34,yn,1))==Err);
          if(flag==KEY_F(1)||flag==ESC)
            break;
          if(flag==KEY_LEFT||flag==KEY_UP)
          {
            kk--;
            break;
          }
          if(yn[0]==0)
            break;
          if(yn[0]=='y'||yn[0]=='Y')
            kk++;
          else
          {
            mvaddstr(11,24,"����Ա��:[  ]");
            mvaddstr(12,24,"��    ��:[      ]");
            mvaddstr(13,24,"ҵ������:[          ]");
            mvaddstr(15,24,"ȷ    ��:[ ]");
            kk=1;
          }
      }
      if(flag==KEY_F(1)||flag==ESC)
      {
        keypad(stdscr,FALSE);echo();noraw();endwin();
        system("clear");
        exit(0);
      }
    }
    if(kk!=5)
      continue;

    note("������źϷ���");
    
    
		sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",op_br);
		
  	if(mysql_query(conn,sql))
	  {
      sprintf(sys_err_str,"select branch err,%s",mysql_error(conn));
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
	  }
		
		res=mysql_store_result(conn);
    row=mysql_fetch_row(res);

		if(row==NULL)
		{
      dis_str(22,10,"ָ����������!",0,0);  
      getch();
      continue;
		}
		else {
		  set_f_branch(&br,row);
		}
		mysql_free_result(res);
    
    if(br.br_flag[0]!='A') 
    {
      dis_str(22,18,"����������!",0,0); 
      getch();
      continue;
    }
    if(br.init_flag==1) 
    {
      dis_str(22,18,"�����ѳ�ʼ��!",0,0); 
      getch();
      continue;
    }
    rjulmdy(br.work_d,ymd); 
    sprintf(tmp_ymd,"%04d/%02d/%02d",ymd[2],ymd[0],ymd[1]);
    if(strcmp(tmp_ymd,op_d)!=0) 
    {
      dis_str(21,18,"����ҵ�����뱾�������ղ���!",0,0); 
      sprintf(sys_err_str,"�ֹ�����Ϊ:%04d��%02d��%02d��",ymd[2],ymd[0],ymd[1]);
      dis_str(22,18,sys_err_str,0,0);
      getch();
      continue;
    }
    sys_work_d=br.work_d;

    note("������Ա�Ϸ���");
    
    
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
      dis_str(22,10,"ָ������Ա������!",0,0);  
      getch();
      continue;
    }
    if(mysql_errno(conn)!=0) 
    {
      sprintf(sys_err_str,"����Ա������");
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
    }
    if(op.op_flag[0]!='A') 
    {
      dis_str(22,18,"������Ա������!",0,0); 
      getch();
      continue;
    }
    if(op.op_level[0]!='1' &&  op.op_level[0]!='4') 
    {
      dis_str(22,18,"������Ա���ܰ���ҵ��!",0,0); 
      getch();
      continue;
    }
    cry_pwd(pwdli,pwdsi,op_no); 
    if(strncmp(op.password,pwdsi,8)!=0) 
    {
      dis_str(22,18,"�������!",0,0); 
      getch();
      continue;
    }
    
    note("���±�����Ա���й�����"); 
    pid=getpid();
    tty_name = ttyname(1);
    l=strlen(tty_name);
    //strncpy(ttyno,&tty_name[5],l-5);
    strncpy(ttyno,&tty_name[5],sizeof(ttyno)-1);
	  ttyno[sizeof(ttyno)-1]='\0';
    op_no=sys_operator;
    
	
	  sprintf(sql,"UPDATE operfile SET tty_no = '%s',tty_pid= %d WHERE operator= %d ",ttyno,pid,op_no);
	
    if(mysql_query(conn,sql)||mysql_errno(conn)){
      sprintf(sys_err_str,"�޸Ĳ���Ա��!%s",mysql_error(conn));
      dis_str(22,18,sys_err_str,60,0);
      getch();
      continue;
    }
    
    sys_branch=op_br;
    sys_operator=op_no;
    strcpy(sys_op_name,op.op_name);
    strcpy(sys_op_level,op.op_level);
    sys_today=br.work_d;
    sys_sernum=br.ser_num+1;
    clear_scr(5,22,18,77);
    main_menu();
  }
}

main_menu() 
{
  static cenitem mainmenu[]=
  {
     3,70,"�˳�", 
     3, 4,"����ҵ��", 
     3,14,"��ҵ��", 
     3,24,"�ۺ�ҵ��", 
     3,34,"ҵ������",
     3,44,"��ˮ��ѯ",
     3,54,"������",
  };
  short choice,ymd[3];
  short op_br,op_no,sernum;

  disp_main(2);
  op_br=sys_branch;
  op_no=sys_operator;
  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    
    sernum=1;
    
    mvprintw(1,68,"��ˮ��:%d",sys_sernum);
    refresh();
    init_in(&da);
    choice=menuctl(mainmenu,6,choice,0);
    switch(choice)
    { 
      case 1:
        ss_p(mainmenu[choice].x);
        break;
      case 2:
        card(mainmenu[choice].x);
        break;
      case 3:
        zh_p(mainmenu[choice].x);
        break;
      case 4:
        total_p(mainmenu[choice].x);
        break;
      case 5:
        query(mainmenu[choice].x);
        break;
      case 6:
        server(mainmenu[choice].x);
        break;
      case 0:
        op_no=sys_operator;
        sprintf(sql,"UPDATE operfile SET tty_no= '%s' ,tty_pid = %d WHERE operator = %d","��",0,op_no);
  
        if(mysql_query(conn,sql)){
          sprintf(sys_err_str,"�޸Ĳ���Ա��! %s",mysql_error(conn));
          dis_str(22,18,sys_err_str,60,0);
          getch();
          continue;
        }
        mysql_close(conn);//����������ط���Ҫ�ر�����
        keypad(stdscr,FALSE); echo(); noraw(); endwin();
        system("clear");
        exit(0);
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}


ss_p(no)
short no;
{
  static cenitem s_menu[]=
  {
   19,30,"0_��    ��",
    8,30,"1_��    ��",
    9,30,"2_��    ��",
   10,30,"3_ȡ    ��",
   11,30,"4_��    ��",
   12,30,"5_��    ʧ",
   13,30,"6_��    ��",
   14,30,"7_��    ��",
   15,30,"8_��������",
   16,30,"9_���ճ���",
   17,30,"10��    ѯ",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(s_menu,10,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      case 1:
        sys_code=1501;
        s_open();
        break;
      case 2:
        sys_code=1503;
        s_dep();
        break;
      case 3:
        sys_code=1504;
        s_withdraw();
        break;
      case 4:
        sys_code=1502;
        s_close();
        break;
      case 5:
        sys_code=1511;
        s_lost();
        break;
      case 6:
        sys_code=1521;
        s_unlost();
        break;
      case 7:
        sys_code=1592;
        s_chghb();
        break;
      case 8:
        s_pwreset();
        break;
      case 9:
        sys_code=1581;
        s_correct();
        break;
      case 10:
        sys_code=1561;
        s_query();
        break;
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

query(no)
short no;
{
  static cenitem query_menu[]=
  {
   11,30,"0_��          ��",
    8,30,"1_�����׺Ų���ˮ",
    9,30,"2_������Ա����ˮ",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(query_menu,2,choice,no);
    switch(choice)
    { 
      case 1:
        sys_code=1611;
        qtrade_ser();
        break;
      case 2:
        sys_code=1613;
        qtrade_op();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

zh_p(no)
short no;
{
  static cenitem zh_menu[]=
  {
   15,30,"0_��    ��",
    8,30,"1_��������",
    9,30,"2_���ʳ���",
   10,30,"3_����ƾ֤",
   11,30,"4_ƾ֤����",
   12,30,"5_��ѯ����",
   13,30,"6_��ѯƾ֤",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(zh_menu,6,choice,no);
    switch(choice)
    { 
      case 1:
        zh_pro_sum();
        break;
      case 2:
        zh_cro_sum();
        break;
      case 3:
        zh_pro_vou();
        break;
      case 4:
        zh_cro_vou();
        break;
      case 5:
        zh_quer_sum();
        break;
      case 6:
        zh_quer_vou();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}

server(no)
short no;
{
  static cenitem server_menu[]=
  {
   12,30,"0_��    ��",
    8,30,"1_��У��λ",
    9,30,"2_�� �� ��",
   10,30,"3_�޸�����",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(server_menu,3,choice,no);
    switch(choice)
    { 
      case 1:
        query_chk();
        break;
      case 2:
        server_cal();
        break;
      case 3:
        update_pwd();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}


total_p(no)
short no;
{
  static cenitem server_menu[]=
  {
   11,30,"0_��        ��",
    8,30,"1_��ȫ��������",
    9,30,"2_������Ա����",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(server_menu,2,choice,no);
    switch(choice)
    { 
      case 1:
        sys_code=1811;
        total_br();
        break;
      case 2:
        sys_code=1812;
        total_op();
        break;
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}



card(no)
short no;
{
  static cenitem s_menu[]=
  {
   18,30,"0_��    ��",
    8,30,"1_��    ��",
    9,30,"2_���۲���",
   10,30,"3_��    ��",
   11,30,"4_ȡ    ��",
   12,30,"5_��    ��",
   13,30,"6_��    ʧ",
   14,30,"7_��    ��",
   15,30,"8_���ճ���",
   16,30,"9_��    ѯ",
  };
  short choice;

  choice=0;
  while(1)
  {
    clear_scr(5,22,18,77);
    choice=menuctl(s_menu,9,choice,no);
    switch(choice)
    { 
      case 0:
        clear_scr(5,22,18,77);
        refresh();
        return(0);
      case 1:
        sys_code=1501;
        c_open();
        break;
      case 2:
        sys_code=1591;
        c_scard();
        break;
      case 3:
        sys_code=1503;
        c_dep();
        break;
      case 4:
        sys_code=1504;
        c_withdraw();
        break;
      case 5:
        sys_code=1502;
        c_close();
        break;
      case 6:
        sys_code=1511;
        c_lost();
        break;
      case 7:
        sys_code=1521;
        c_unlost();
        break;
      case 8:
        sys_code=1581;
        c_correct();
        break;
      case 9:
        sys_code=1561;
        c_query();
        break;
      default:
        dis_str(22,18,"����ѡ��!",0,0);
        getch();
        break;
    }
  }
}
