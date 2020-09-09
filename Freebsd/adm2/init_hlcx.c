/****************************************************************
������:init_hlcx.e
��  ��:main()
��  ��:����ϵͳ��ʼ��
��  ��:*����Ƿ��ѳ�ʼ��(������Ա�Ѵ���)
       *��ʼ�����ʱ�
       *��ʼ����Ŀ��
       *���ӵ�һ����Ա
       *���ӵ�һ����
****************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<curses.h>
#include<ctype.h>
#include<signal.h>
#include "mysql.h"
#include "../include/apt.h"

long appid,comid;
long stype,qid_key,loc_country;
long smachineno;
char sys_err_str[1024];

MYSQL *conn=NULL;
char sql[1024];
short item_count=0;
long item_nums[100];
short vou_count=0;
char vou_types[100][3];

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

own_exit()
{
  mysql_rollback(conn);
  mysql_close(conn);
  getch();
  keypad(stdscr,FALSE);
  echo();
  noraw();
  endwin();
  exit(0);
}

err_me(char *str)
{
    sprintf(sys_err_str,"%s,%s",str,mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    own_exit();
}

main() 
{
  FILE *fp;
  short mdy[3];
  long datey,datem,dated,termr,flag;
  short br=100;
  short term;
  short kind;
  long cnt,rate,ratedate,item_num;
  char type,acc_type,item_name[21],vou_type[3],vou_name[11];
  char init_pwd[9];
  long sum_flag;
  short prt_ser;
  char scname[12],scvalue[20];
  FILE *scfp;
  
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  conn=mysql_init(NULL);

  system("stty sane");
  initscr();raw();noecho();keypad(stdscr,TRUE); 
  
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
	  getch();
	  keypad(stdscr,FALSE);
	  echo();noraw();endwin();
	  exit(0);
  }
  
  dis_str(22,22,"���ڴ���,���Ժ� ...",0,0);
  
  sprintf(sql,"select count(*) from operfile");
  if(mysql_query(conn,sql)){
    dis_str(21,22,"ȡ�û�����",0,0);
	  getch();
	  keypad(stdscr,FALSE);
	  echo();noraw();endwin();
	  exit(0);
  }
  res=mysql_store_result(conn);
  row=mysql_fetch_row(res);
	if(row!=NULL){
		cnt=atoi(row[0]);
    mysql_free_result(res);
	}
  if(cnt!=0)
  {
    dis_str(21,22,"������ϵͳ��ʼ��",0,0);
	  getch();
	  keypad(stdscr,FALSE);
	  echo();noraw();endwin();
	  exit(0);
  }
  
  /******************************************/
  if(mysql_query(conn,"START TRANSACTION")){
    sprintf(sys_err_str,"������������");
    dis_str(22,21,sys_err_str,60,0);
	  getch();
	  keypad(stdscr,FALSE);
	  echo();noraw();endwin();
    return(-1);
  }
  
  dis_str(22,22,"���ڳ�ʼ���ʱ�!",0,0);
  fp=fopen("./data/init_rate","r");
  if(fp==(FILE *)NULL)
  {
    dis_str(22,22,"�������ı���!",0,0);
    own_exit();
  }
  while(fscanf(fp,"%1c %02ld %04ld %02ld %02ld %06ld\n",
    &type,&termr,&datey,&datem,&dated,&rate)==6)
  {
    mdy[2]=datey;
    mdy[0]=datem;
    mdy[1]=dated;
    rmdyjul(mdy,&ratedate);
    term=termr;
    sprintf(sql,"insert into ratefile values('%c',%d,%ld,%ld)",type,term,ratedate,rate);
    if(mysql_query(conn,sql))err_me("д���ʱ��");
  }
  fclose(fp);

  dis_str(22,22,"��ʼ����Ŀ��!",0,0);
  fp=fopen("./data/init_item_file","r");
  if(fp==(FILE *)NULL)
  {
    dis_str(22,22,"��Ŀ�ı��򿪴�!",0,0);
    own_exit();
  }
  note("���� ���� ���� ��Ŀ�� ��־ ͳ�Ʊ�־ ��ӡ˳�� ��Ŀ��");
  while(fscanf(fp,"%1c %1c %02ld %ld %01ld %6ld %2d %s\n",
    &type,&acc_type,&termr,&item_num,&flag,&sum_flag,&prt_ser,item_name)==8)
  {
    sprintf(sql,"insert into sumtable values(0,0,%ld,'A',0,0,0,0,0,0,0,0)",item_num);
    if(mysql_query(conn,sql))err_me("д���ʴ�");
    term=termr;
    sprintf(sql,"insert into item_file values('%c','%c',%d,%ld,'%s','A',%ld,%d)",
      type,acc_type,term,item_num,item_name,sum_flag,prt_ser);
    if(mysql_query(conn,sql))err_me("д��Ŀ���");
    item_nums[item_count]=item_num;
    item_count++;
    if(item_count>100)
    {
      dis_str(22,22,"item_count>100",60,0);
      own_exit();
    }
  }
  fclose(fp);
	
  dis_str(22,22,"��ʼ��ƾ֤��!",0,0);
  fp=fopen("./data/init_vou_file","r");
  if(fp==(FILE *)NULL)
  {
    dis_str(22,22,"ƾ֤�ļ���!",0,0);
    getch();
    own_exit();
  }
  note("���� ���� ͳ�Ʊ�־ ��ӡ˳�� ������");
  while(fscanf(fp,"%2s %1c %02ld %d %s\n",
    vou_type,&type,&sum_flag,&prt_ser,vou_name)==5)
  {
    sprintf(sql,"insert into vou_file values('%s','%s','A','%c',%ld,%d)",
      vou_type,vou_name,type,sum_flag,prt_ser);
    if(mysql_query(conn,sql))err_me("дƾ֤�ļ���");
    sprintf(sql,"insert into vouch values(0,0,'A','%s',0,0,0,0,0)",vou_type);
    if(mysql_query(conn,sql))err_me("дƾ֤ͳ���ļ���");
    strcpy(vou_types[vou_count],vou_type);
    vou_count++;
    if(vou_count>100)
    {
      dis_str(22,22,"vou_count>100",60,0);
      own_exit();
    }
  }
  fclose(fp);
	
  dis_str(22,22,"��ʼ������Ա��!",0,0);
  cry_pwd("101102",init_pwd,1);
  sprintf(sql,"insert into operfile values(1,'%s','7','����Ա1','A','��',0,0)",init_pwd);
  if(mysql_query(conn,sql))err_me("д����Ա�ļ���");

  cry_pwd("000000",init_pwd,10);
  sprintf(sql,"insert into operfile values(10,'%s','4','����Ա10','A','��',0,0)",init_pwd);
  if(mysql_query(conn,sql))err_me("д�����Ա�ļ���");
  cry_pwd("000000",init_pwd,11);
  sprintf(sql,"insert into operfile values(11,'%s','4','����Ա11','A','��',0,0)",init_pwd);
  if(mysql_query(conn,sql))err_me("д�����Ա�ļ���");
  cry_pwd("000000",init_pwd,12);
  sprintf(sql,"insert into operfile values(12,'%s','4','����Ա12','A','��',0,0)",init_pwd);
  if(mysql_query(conn,sql))err_me("д�����Ա�ļ���");
  cry_pwd("000000",init_pwd,13);
  sprintf(sql,"insert into operfile values(13,'%s','4','����Ա13','A','��',0,0)",init_pwd);
  if(mysql_query(conn,sql))err_me("д�����Ա�ļ���");
  
  dis_str(22,22,"������������¼......",0,0);
  sprintf(sql,"insert into br_file values(%d,'������������','A',1,1,1,0,0)",br);
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"д���ļ���,%s",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    own_exit();
  }

  sprintf(sql,"insert into acc_file values(100,'H',9580100)");
  if(mysql_query(conn,sql))err_me("д�����ʺŴ�");
  sprintf(sql,"insert into acc_file values(100,'K',9585200)");
  if(mysql_query(conn,sql))err_me("д���Ŵ�");
  note("���ݿ�Ŀ�ļ����ӱ������ʼ�¼");
  dis_str(22,22,"�������ӱ������ʼ�¼......",0,0);
  
  short ici=0;
  for (;ici<item_count;ici++)
  {
		item_num=item_nums[ici];
    kind=0;
    while(kind<100)
    {
		  sprintf(sql,"insert into sumtable values(%d,%d,%ld,'A',0,0,0,0,0,0,0,0)",br,kind,item_num);
      if(mysql_query(conn,sql))err_me("д�����ļ���");
      kind++;
      if(kind==13)
        kind=90;
      if(kind==92)
        kind=99;
    }
  }
  note("����ƾ֤�ļ����ӱ���ƾ֤ͳ�Ƽ�¼");
  dis_str(22,22,"�������ӱ���ƾ֤ͳ�Ƽ�¼......",0,0);
  
  ici=0;
  for (;ici<vou_count;ici++)
  {
		strcpy(vou_type,vou_types[ici]);
    kind=0;
    while(kind<100)
    {
		  sprintf(sql,"insert into vouch values(%d,%d,'A','%s',0,0,0,0,0)",br,kind,vou_type);
      if(mysql_query(conn,sql))err_me("дƾ֤�ļ���");
      kind++;
      if(kind==13)
        kind=90;
      if(kind==92)
        kind=99;
    }
  }
  
  mysql_commit(conn);
  mysql_close(conn);
  
  dis_str(22,22,"�������,����һ������!",0,0);
  getch();
  keypad(stdscr,FALSE);echo();noraw();endwin();
  system("clear");
  return(0);
}
