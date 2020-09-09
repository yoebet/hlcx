/**************************************************************
 ������:pr_balance.ec 
 ������:pr_balance() 
 ��  ��:���ɸ���/ȫ�������� 
**************************************************************/       
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

MYSQL_RES *res;
MYSQL_ROW row;

struct bal_s
{   
  long km;
  char flag;
  char kmname[17];
  long home_num; /* ���� */
  double sbal; /*�����*/
  short lcount; /*��ʧ���� */
  short fcount; /*���ủ�� */
  long  dcvou;  /*�跽���� */
  double dcre;   /*�跽������*/
  long  ddvou;  /*�������� */
  double ddet;   /*����������*/
  long  dcls;  /*������*/
  long  dopen;  /*������*/
} bal[100];  /** �ܿ�Ŀ��=100 **/

extern short br_id[50];
short k,cen_id,branch_id,item_count,b_count,biser=0;
short sflag=0,sh,ymd[3];
char c,tabletype;
char br_name[11];
long rectmp,ltoday;
  
pr_balance()
{
  char yy;
  struct in_data da;

  static short in[2]={27,8};
  short num=2,flag;

  init_in(&da);

  clear_scr(5,22,22,77);
  mvaddstr(5,30,"����ӡ����");  
  refresh();

  dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

  rtoday(&ltoday);
  rjulmdy(ltoday,ymd);

  da.tr_br=100;
  branch_id=da.tr_br;
  if (branch_id==0)
    return(0);

  note("�������ļ�����bal����!");
  if(read_bal()==-1)
    return(-1);

  note("����ĳ��branch_id����");
  note("����������bal[100]��ͳ�Ʊ�����0"); 
  init_bal();

  note("ȡ����!");
	
	sprintf(sql,"SELECT br_name FROM br_file WHERE  branch= %d ",branch_id);
  	
  mysql_query(conn,sql);

  res=mysql_store_result(conn);

  if((row=mysql_fetch_row(res))!=NULL){
    strcpy(br_name,row[0]);
  }
  mysql_free_result(res);

  if(row==NULL)
  {
    sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("ȡ�����б���,����Ŀ�ı��ս�/���������ͷ�����!");
  if(read_sumtable()!=0)
  {
    sprintf(sys_err_str,"ȡ�������ݴ�!");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("��branch_id���ĸ��������ֱ�ͳ�Ƶ�bal[]!");
	if(sum_bal()!=0)
  {
    sprintf(sys_err_str,"ͳ������!.");
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  note("����ͳ�ƵĽ����������ͽ���������!");
  pr_bal();
  
  return(0);
}

/**************************************************
������:read_bal()             
��  ��:���������ļ�data/def_bal
       ��������������bal[100]
��  ��:
       ȫ�ֱ���bal[100]
**************************************************/
read_bal()
{
  FILE *fp;
  int j,k;
  if((fp=fopen("./data/def_bal","r"))==NULL)
  {	
    dis_str(22,22,"�������ļ�/usr/hlcx/data/def_bal�򲻿�!",60,0);
    getch();
    return(-1);
  }
  note("�� def_bal ������bal��ֵ!");
  for(k=0;;k++)
  {
  	//211102 0 �� �� �������ո����
    j=fscanf(fp,"%ld %c %s",&bal[k].km,&bal[k].flag,bal[k].kmname);
    if(j!=3) 
      break;
    if (bal[k].km==999999)
      break;
  }
  item_count=k-1;
  return(0);
}

/**************************************************
������:init_bal()         
��  ��:����������bal[100]��ͳ�Ʊ�����0 
��  ��:
       ȫ�ֱ���
         bal[100]
         item_count  ��Ŀ��
**************************************************/
init_bal()
{
  int j;
  for(j=0;j<=item_count;j++)
  {
    bal[j].lcount=0;
    bal[j].fcount=0;
    bal[j].home_num=0L;
    bal[j].dcvou=0L;
    bal[j].ddvou=0L;
    bal[j].dcls=0L;
    bal[j].dopen=0L;
    bal[j].sbal=0;
    bal[j].dcre=0;
    bal[j].ddet=0;
  }
  return(0);
}

/**************************************************
������:read_sumtable()          
��  ��:ȡ�����б���,����Ŀ�ı���(kind=90)
       ��/���������ͽ�/����������
��  ��:
       ȫ�ֱ���
         bal[100]
         item_count  ��Ŀ��
**************************************************/
read_sumtable()
{
  int i;
  double lcre,ldet;
  long  lcvou,ldvou,lopen,lcls;
  for(i=0;i<=item_count;i++)
  {
    lcvou=ldvou=lopen=lcls=0;
    lcre=ldet=0.00;
	
	  /****************/
	
	  sprintf(sql,"SELECT cvou_que,cre_amt,dvou_que,det_amt,open_que,cls_que\
  	    FROM sumtable WHERE  branch= %d AND kind=90 AND item_num=%ld",branch_id,bal[i].km);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
		  lcvou=atol(row[0]);
		  lcre=atof(row[1]);
		  ldvou=atol(row[2]);
		  ldet=atof(row[3]);
		  lopen=atol(row[4]);
		  lcls=atol(row[5]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"ȡ��Ŀ[%ld]�ı������ʴ�!.",bal[i].km);
      dis_str(22,22,sys_err_str,60,0);
      getch();
    }
	
	/****************/
	
    bal[i].dcvou+=lcvou;
    bal[i].ddvou+=ldvou;
    bal[i].dopen+=lopen;
    bal[i].dcls+=lcls;
    bal[i].dcre+=lcre; 
    bal[i].ddet+=ldet; 
  }
  return(0);//?
}
/********************************************************
������:pr_bal()                                       
����:����ͳ�ƵĽ����������ͽ��������ɳ���        
********************************************************/
pr_bal()
{
  FILE *fp;
  short balpoint,ii;
  char lin[150],balstr[15],branch_name[11];
  char balstr1[15],balstr2[15];
  char bal_file[55];
  char cmd_str[90];
  char flag;
  char sql[96];

  if(branch_id==888)
  {
    sprintf(bal_file,"./data/t,mp_all_bal");
    if((fp=fopen(bal_file,"w"))==NULL)
    {	
      dis_str(22,22,"�����ļ���������!",60,0);
      getch();
      return(-1);
    }
    fprintf(fp,"%40sȫ��������"," " );
    fprintf(fp,"%20s ����:%4d��%02d��%02d��\n"," ",ymd[2],ymd[0],ymd[1]);
  }
  else
  {
    sprintf(bal_file,"./tmp/tmp_br_bal_%d",branch_id);
    if((fp=fopen(bal_file,"w"))==NULL)
    {	
      dis_str(22,22,"�����ļ���������!",60,0);
      getch();
      return(-1);
    }
	
	/*********************/
	
	snprintf(sql,95,"SELECT br_name FROM br_file WHERE  branch= %d ",branch_id);
  	
    mysql_query(conn,sql);
	
    res=mysql_store_result(conn);

    if((row=mysql_fetch_row(res))!=NULL){
	  strcpy(br_name,row[0]);
    }
    mysql_free_result(res);
  
    if(row==NULL)
    {
      sprintf(sys_err_str,"ȡ����[%d]�����ƴ�!%s.",branch_id,mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
    }
	
	/*********************/
	
    fprintf(fp,"%40s  ����\n"," ");
    fprintf(fp,"%20s ����:%4d��%02d��%02d��        \n"," ",ymd[2],ymd[0],ymd[1]);
  }

  fprintf(fp,"���������ש����������������ש��������������������ש��������������������ש��������������ש����ש����ש������ש����ש�����\n");
  fprintf(fp,"��      ��                ��     ��      ��     ��     ��      ��     ��              ��    ��    ��      ��    ��    ��\n");
  fprintf(fp,"����Ŀ�ũ�    ��Ŀ����    �ǩ����ש��������������贈���ש���������������    ��  ��    �������������� �໧ ����ʧ�����ᩧ\n");
  fprintf(fp,"��      ��                ��������   �� �� ��   ��������   �� �� ��   ��              ��    ��    ��      ��    ��    ��\n");
  fprintf(fp,"�ǩ������贈���������������贈���贈�������������贈���贈�������������贈�������������贈���贈���贈�����贈���贈����\n");
      
  ii=0;
  while (1)
  {
    if(bal[ii].sbal+bal[ii].home_num+bal[ii].dcre
      +bal[ii].ddet+bal[ii].dcvou+bal[ii].ddvou==0.0 )
    {
      ii++;
      if(ii>item_count)
        break;
      continue;
    }
    fprintf(fp,"��%6ld��%16s��%4ld��%14.2lf��%4ld��%14.2lf��%14.2lf��%4ld��%4ld��%6ld��%4ld��%4ld��\n",bal[ii].km,bal[ii].kmname,bal[ii].dcvou,bal[ii].dcre/100.00,bal[ii].ddvou,bal[ii].ddet/100.00,bal[ii].sbal/100.00,bal[ii].dcls,bal[ii].dopen,bal[ii].home_num,bal[ii].lcount,bal[ii].fcount);
    if(bal[ii].flag=='1')
      fprintf(fp,"�ǩ������贈���������������贈���贈�������������贈���贈�������������贈�������������贈���贈���贈�����贈���贈����\n");
    ii++;
    if(ii>item_count)
      break;
  }
  fprintf(fp,"���������ߩ����������������ߩ����ߩ��������������ߩ����ߩ��������������ߩ��������������ߩ����ߩ����ߩ������ߩ����ߩ�����\n");
  fclose(fp);
    
	/*****************/
	
	sprintf(sql,"UPDATE br_file SET pr_flag = (pr_flag+1) WHERE branch= %d ",branch_id);
	
    if(mysql_query(conn,sql)||mysql_errno(conn)){
      sprintf(sys_err_str,"�޸ı����ļ���!%s\n",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }
	
	/*****************/
	
  //_DyFuntion(bal_file,3);

    dis_str(22,22,"�Ƿ��ӡ(y/n):[ ]",60,0);
    flag=yorn(22,44);
    if(flag=='y'||flag=='Y') 
    {
      dis_str(22,22,"��׼���ô�ӡ������һ����ʼ��ӡ",60,0);
      getch();       
      sprintf(cmd_str,"pr %s > "Printer, bal_file);
      system(cmd_str);
    }
  return(0);
}
/*******************************************************
������:sum_bal() 
��  ��:��branch_id���ĸ��������ֱ�ͳ�Ƴ��� 
*******************************************************/
sum_bal()
{
  sprintf(sys_err_str,"����ͳ�Ƹ��������...");
  dis_str(22,22,sys_err_str,60,0);
  note("ͳ�ƻ������!");
  s_bal('s');
  note("ͳ�ƶ������!");
  s_bal('f');
  note("���ܸ��������!");
  to_bal(); 
  return(0);
}

/****************************************************
������:fkm(item) 
��  ��:���ݿ�Ŀ��������������λ�� 
��  ��:��Ŀ�� item                    
��  ��:�˿�Ŀ�������е�λ�� 
****************************************************/
fkm(item)
long item;
{
  short m,k;
  for(k=0;k<=item_count;k++)
  {
    if(bal[k].km==item)
      break;
  }
  if(k>item_count)
    return(-1);
  else
    return(k);
} 

/*****************************************************
������:s_bal(type)
��  ��:ͳ�Ƹ���������� 
��  ��:
       char type; ��ͳ�ƵĴ���
*****************************************************/
s_bal(type)
char type;
{
  char ss_amt[15],ss_flag[2],s_flag,sbr[150];
  long skm,sbalance,s_close_d;
  int rcode,j;

  note("�� sbr[]��ֵ!");
  for(j=0;j<150;j++)
    sbr[j]=' ';

  switch(type)
  {
    case 's':
      sprintf(sbr,"select balance,item_num,acc_flag from %c_mast where open_br = %d",type,branch_id);
      break;
    default:
      note("����!");
      sprintf(sbr,"select amount,item_num,acc_flag from %c_mast where open_br = %d",type,branch_id);
      break;
  }
  
  mysql_query(conn,sbr);
  
  if(mysql_errno(conn))
  {
    sprintf(sys_err_str,"%s.",mysql_error(conn));
    dis_str(22,22,sys_err_str,60,0);
    getch();
    return(-1);
  }

  res=mysql_use_result(conn);

  for(row=mysql_fetch_row(res);row!=NULL;row=mysql_fetch_row(res))
  {
    if(mysql_errno(conn)!=0)
      break;
		sbalance=atol(row[0]);
		skm=atol(row[1]);
		strcpy(ss_flag,row[2]);
	
    s_flag=ss_flag[0];
    note("����K ����C ����B ��ʧ����G!");
    if(s_flag=='K'||s_flag=='C'|| s_flag=='B'||s_flag=='G')
      continue;
    else
    {
      note("����/��ʧ/����!");
      note("�ұ���������Ŀ�������е��±�!");
      j=fkm(skm);
      if(j!=-1)
      {
        bal[j].home_num++; /*�ܻ��� */
        if((s_flag=='L')||(s_flag=='l'))
          bal[j].lcount++;
        if(s_flag=='F')
          bal[j].fcount++;
        bal[j].sbal+=sbalance;
      }
    }
  }
  mysql_free_result(res);

  return(0);
}

/*****************************************************
������:to_bal()                               
��  ��:ͳ�Ƹ�С��,�ܼƿ�Ŀ
˵  ��:�ӿ�Ŀ�ź���λΪ����,С�ƿ�Ŀ����λ��Ϊ00
*****************************************************/
to_bal()
{
  FILE *fp;
  long saccno,acc1[20];
  short s1,s2;
  long tohome_num;
  long todcvou,toddvou,todcls,todopen;
  double tobal;
  double todcre,toddet;
  char balstr[14];
  char balstr1[14],balstr2[14];
  short tolcount,tofcount,i,j,k;
  short subkm;

  fp=fopen("./data/def_sbal","r");
  if(fp==NULL) 
  {
    dis_str(22,22,"���ܶ����ļ�/usr/hlcx/data/def_sbal�򲻿�!",60,0);
    getch();
    return(-1);
  }
  tohome_num=tolcount=tofcount=0L;
  todcvou=toddvou=todopen=todcls=0L;
  tobal=todcre=toddet=0.0;
  s1=s2=0;
  while(fscanf(fp,"%ld",&saccno)==1)
  {
    note("С����δ����!");
    if(saccno != 0L)
    {
      note("����Ŀ�ŷ�������acc1[]��!");
      acc1[s1]=saccno;
      note("�ۼ�С�ƿ�Ŀ��!");
      s1++;
    }
    else
    {
      note("С�����ѽ���!");
      if(s1<2) 
      {
        note("��С�ƿ�Ŀ,�����ļ���ʽ��!");
        s1=s2=0;
        continue;
      }
      note("С�ƿ�Ŀ���������е�λ��!");
      j=fkm(acc1[0]);

      note("С���������ó�ֵ!");
      bal[j].home_num=bal[j].lcount=bal[j].fcount=0L;
      bal[j].dcvou=bal[j].ddvou=bal[j].dopen=bal[j].dcls=0L;
      bal[j].sbal=0.0;
      bal[j].dcre=0.0;
      bal[j].ddet=0.0;

      for(s2=1;s2<s1;s2++)
      {
        note("�ۼӿ�Ŀ���������е�λ��!");
        i=fkm(acc1[s2]);

        bal[j].home_num+=bal[i].home_num;
        bal[j].lcount  +=bal[i].lcount;
        bal[j].fcount  +=bal[i].fcount;
        bal[j].dcvou   +=bal[i].dcvou;
        bal[j].ddvou   +=bal[i].ddvou;
        bal[j].dcls    +=bal[i].dcls;
        bal[j].dopen   +=bal[i].dopen;
        bal[j].sbal     +=bal[i].sbal;
        bal[j].dcre    +=bal[i].dcre;
        bal[j].ddet    +=bal[i].ddet;
      }
      s1=s2=0;
    }
  }
  fclose(fp);
  return(0);
}

/*****************************************************
������:str_right(s)
��  ��:���ַ���s�Ҷ���
��  ��:
       char s[]; �账����ַ���
*****************************************************/
str_right(s)
char s[];
{
  int i,j;
  j=i=strlen(s);
  i--;
  while(j>0) 
  {
    j--;
    if(s[j] !=' ') 
    { 
      s[i]=s[j];
      i--;
    }
    if(i !=j)
      s[j]=' ';
  }
  return(0);
}

