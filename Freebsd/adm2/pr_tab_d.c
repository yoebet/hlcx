/******************************************************************
* ������:pr_tab_d.ec 
* ������:pr_tab_d(br,flag)
* ��  ��:��ӡָ�������ձ�/�±�/�걨
* ��  ��:br   ���� 999_ȫ���� A99_A���´� 000_�˳� ABC_ָ����
         flag ��־  0���걨 1-12ĳ�±� 90�ձ� 99���걨
*******************************************************************/
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern long sys_today;
extern short sys_operator;
extern MYSQL *conn;
extern char sql[];

pr_tab_d(br,flag)
short br,flag;
{

  /**�ձ����ʽ����,�����ӡ���˳��һ��**/
  struct tab
  {
    short bz;
    long km;
    char mc[19];
    long clos;
    double jf_amt;
    long open;
    double df_amt;
    long rest;
    double jf_bal;
    double df_bal;
    long jf_cnt;
    long df_cnt;
  }dd[100];

  /**�����ʽ�ַ���,�����ӡ���һ��,Ϊ���������**/
  char mc[19],s_clos[5],s_jf_amt[11],s_open[5],s_df_amt[11];
  char s_rest[7],s_jf_bal[13],s_df_bal[13],s_df_cnt[5],s_jf_cnt[5];
  
  /**���ʱ��ʽ����,��sumtabl���˳��һ��,���ڶ�ȡ����**/
  /*
  struct sum
  {
    short br;
    short kind;
    long km;
    char flag[2];
    dec_t det_amt;
    dec_t cre_amt;
    long open;
    long clos;
    long rec;
    long pay;
    dec_t bal;
    long  rest;
  } ss;
  */
  struct f_sumtable ss;

  FILE *fp_i,*fp_o;
  short branch,kind;
  long item;
  char fname[50],item_name[19],s_item[7],s_bz[3];
  double bal;
  short kk,i,j,bz,ymd[3];
  char buffer[24],*sp;
  long ltoday;
  char bt0[5];
  short br_id[130],s_br,e_br;
  char br_name[130][11];
  short i_br,k,br_num,item_num,ii,jj;
  char cmd_str[100];

  MYSQL_RES *res;
  MYSQL_ROW row;

  if((fp_i=fopen("./data/def_tab_d","r"))==NULL)
  {	
    dis_err("�ձ������ļ�δ�ҵ�!"); 
    return(1);
  }

  i=1;
  while (fgets(buffer,74,fp_i))
  {
    if (feof(fp_i))
    {
      fclose (fp_i);
      break;
    }
    sp=buffer;
    j=0;
    memcpy(s_bz,sp+j,2); 
    s_bz[2]='\0';
    dd[i].bz=atoi(s_bz);
    j+=2;
    memcpy(s_item,sp+j,6); 
    s_item[6]='\0';
    j+=6;
    memcpy(item_name,sp+j,17); 
    item_name[17]='\0';
    item=atol(s_item);
    dd[i].km=item;
    strcpy(dd[i].mc,item_name);
    if (item==222222)
      break;
    i=i+1;
  }
  item_num=i;
  fclose(fp_i);

  rtoday(&ltoday);
  rjulmdy(ltoday,ymd);

  kind=flag;
  if (flag==90)
    strcpy(bt0,"�ձ�");
  else if (flag==99)
    strcpy(bt0,"�걨");
  else if (flag==0)
    strcpy(bt0,"����");
  else
    strcpy(bt0,"�±�");

  note("���ڶ�ȡ�账���������!");
  s_br=br;
  e_br=br; 
  i_br=br/100;
  if (br==999)
  {
    s_br=101;
    e_br=999; 
  }
  /**
  if (br==i_br*100)
  {
    s_br=i_br*100+1;
    e_br=i_br*100+99; 
  }
  **/
  
  /**********************************/
  
  sprintf(sql,"SELECT branch,br_name FROM br_file WHERE branch>=%d AND branch<=%d ORDER BY branch",s_br,e_br);
  	
  if(mysql_query(conn,sql))
  {
    sprintf(sys_err_str,"��ѯ����!%s.",mysql_error(conn));
    dis_err(sys_err_str);
    return(-1);
  }

  res=mysql_store_result(conn);

  br_num=0;
  for(k=1;;k++)
  {
    if((row=mysql_fetch_row(res))==NULL||mysql_errno(conn)!=0)
      break;
	br_id[k]=atoi(row[0]);
	strcpy(br_name[k],row[1]);
  }
  mysql_free_result(res);
	
	/**********************************/

  sprintf(sql,"SELECT * FROM sumtable WHERE branch=? AND kind=? AND item_num=? ");
  
  MYSQL_STMT *stmt;
  MYSQL_BIND param_bind[3];
  MYSQL_BIND result_bind[12];
  
  stmt = mysql_stmt_init(conn);
  if (!stmt||mysql_stmt_prepare(stmt, sql, strlen(sql)))
  {
	sprintf(sys_err_str,"��ʼ��Ԥ��������!");
	dis_err(sys_err_str);
	return(-1);
  }
  memset(param_bind, 0, sizeof(param_bind));
  
  param_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  param_bind[0].buffer= &branch;

  param_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  param_bind[1].buffer= &kind;

  param_bind[2].buffer_type= MYSQL_TYPE_LONG;
  param_bind[2].buffer= &item;
  
  memset(result_bind, 0, sizeof(result_bind));
  
  result_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[0].buffer= &ss.branch;
  result_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[1].buffer= &ss.kind;
  result_bind[2].buffer_type= MYSQL_TYPE_LONG;
  result_bind[2].buffer= &ss.item_num;
  result_bind[3].buffer_type= MYSQL_TYPE_STRING;
  result_bind[3].buffer= &ss.flag;
  result_bind[3].buffer_length=2;
  result_bind[4].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[4].buffer= &ss.det_amt;
  result_bind[5].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[5].buffer= &ss.cre_amt;
  result_bind[6].buffer_type= MYSQL_TYPE_LONG;
  result_bind[6].buffer= &ss.open_que;
  result_bind[7].buffer_type= MYSQL_TYPE_LONG;
  result_bind[7].buffer= &ss.cls_que;
  result_bind[8].buffer_type= MYSQL_TYPE_LONG;
  result_bind[8].buffer= &ss.dvou_que;
  result_bind[9].buffer_type= MYSQL_TYPE_LONG;
  result_bind[9].buffer= &ss.cvou_que;
  result_bind[10].buffer_type= MYSQL_TYPE_DOUBLE;
  result_bind[10].buffer= &ss.balance;
  result_bind[11].buffer_type= MYSQL_TYPE_LONG;
  result_bind[11].buffer= &ss.rest_que;
  

	/**********************************/
	
  if (k==1)
    br_num=1;
  else
    br_num=k-1;
	
  for(ii=1;ii<=br_num;ii++)
  {
    branch=br_id[ii];

    note("��ʼ�������ʽ����!");
    for (i=0;i<=item_num;i++)
    {  
      dd[i].clos=0;
      dd[i].jf_amt=0.0;
      dd[i].open=0;
      dd[i].df_amt=0.0;
      dd[i].rest=0;
      dd[i].jf_bal=0.0;
      dd[i].df_bal=0.0;
      dd[i].jf_cnt=0;
      dd[i].df_cnt=0;
    }
    for(jj=1;jj<item_num;jj++)
    {
      item=dd[jj].km;
      strcpy(item_name,dd[jj].mc);
      if (item==111111 || item==222222 || item==888888)
        continue;
      sprintf(sys_err_str,"���ڶ�����  ��Ŀ:[%6ld]!",item);
      note(sys_err_str);
	  
	  /****************************/
	  
	  
	  if (mysql_stmt_bind_param(stmt, param_bind)||
			mysql_stmt_execute(stmt)||
			mysql_stmt_bind_result(stmt, result_bind)||
			mysql_stmt_store_result(stmt)||
			mysql_stmt_fetch(stmt))
	  {
        sprintf(sys_err_str,"�����ʴ�!��Ŀ��[%ld],��־[%d]!",item,kind);
        dis_err(sys_err_str);
        continue;
	  }
	  mysql_stmt_free_result(stmt);
	  
	  /****************************/
	  
      dd[jj].clos=ss.cls_que;
      bal=ss.det_amt;
      //dectodbl(&ss.det_amt,&bal);
      dd[jj].df_amt=bal;
      dd[jj].open=ss.open_que;
      bal=ss.cre_amt;
      //dectodbl(&ss.cre_amt,&bal);
      dd[jj].jf_amt=bal;
      dd[jj].rest=ss.rest_que;
      bal=ss.balance;
      //dectodbl(&ss.balance,&bal);
      if (bal>0)
        dd[jj].df_bal=bal;
      else
        dd[jj].jf_bal=-bal;
      dd[jj].jf_cnt=ss.cvou_que;
      dd[jj].df_cnt=ss.dvou_que;
    }
	
    note("�������ɱ���!");
    sprintf(fname,"./tmp/tmp_tab_d_%d",branch);
    if((fp_o=fopen(fname,"w"))==NULL)
    {	
      dis_err("�ձ��ļ���������!");
      return(1);
    }
    fprintf(fp_o,"%40sӪҵ%s��\n", " ",bt0);
    fprintf(fp_o,"%20s  �Ʊ�����:%4d��%2d��%2d��      \n"," ",ymd[2],ymd[0],ymd[1]);
    fprintf(fp_o,"���������������������ש����������������������������������ש����������������������������������ש�����������\n");
  if (flag==90)
    fprintf(fp_o,"��                  ��          ���շ�����              ��             �������             ��  ҵ����  ��\n");
  else if (flag==99)
    fprintf(fp_o,"��                  ��          ���귢����              ��             �������             ��  ҵ����  ��\n");
  else if (flag==0)
    strcpy(bt0,"����");
  else
    fprintf(fp_o,"��                  ��          ���·�����              ��             �������             ��  ҵ����  ��\n");
    fprintf(fp_o,"��  ҵ  ��  ��  ��  �ǩ����ש����������ש����ש����������ש������ש������������ש������������ש����ש�����\n");
    fprintf(fp_o,"��                  ��������  ��  ��  ��������  ��  ��  �� ���� ��   ��  ��   ��   ��  ��   ���跽��������\n");
    fprintf(fp_o,"�ǩ������������������贈���贈���������贈���贈���������贈�����贈�����������贈�����������贈���贈����\n");

    for(i=1;i<=item_num;i++)
    {
      if (dd[i].km==111111)
      {
        fprintf(fp_o,"�ǩ������������������贈���贈���������贈���贈���������贈�����贈�����������贈�����������贈���贈����\n");
        continue;
      }
      if (dd[i].km==222222)
      {
        fprintf(fp_o,"���������������������ߩ����ߩ����������ߩ����ߩ����������ߩ������ߩ������������ߩ������������ߩ����ߩ�����\n");
        fprintf(fp_o,"                                       �Ʊ�:                     ����:\n");
        break;
      }
      if (dd[i].km==888888)
        kk=0;
      else
        kk=i;
  
      strcpy(mc,dd[i].mc);

      if (dd[kk].clos==0)
        strcpy(s_clos,"    ");
      else
        sprintf(s_clos,"%4d",dd[kk].clos);

      if (dd[kk].jf_amt==0.0)
        strcpy(s_jf_amt,"          ");
      else
        sprintf(s_jf_amt,"%10.2lf",dd[kk].jf_amt/100.00);

      if (dd[kk].open==0)
        strcpy(s_open,"    ");
      else
        sprintf(s_open,"%4d",dd[kk].open);

      if (dd[kk].df_amt==0.0)
        strcpy(s_df_amt,"          ");
      else
        sprintf(s_df_amt,"%10.2lf",dd[kk].df_amt/100.00);

      if (dd[kk].rest==0)
        strcpy(s_rest,"      ");
      else
        sprintf(s_rest,"%6d",dd[kk].rest);

      if (dd[kk].jf_bal==0.0)
        strcpy(s_jf_bal,"            ");
      else
        sprintf(s_jf_bal,"%12.2lf",dd[kk].jf_bal/100.00);

      if (dd[kk].df_bal==0.0)
        strcpy(s_df_bal,"            ");
      else
        sprintf(s_df_bal,"%12.2lf",dd[kk].df_bal/100.00);

      if (dd[kk].df_cnt==0)
        strcpy(s_df_cnt,"    ");
      else
        sprintf(s_df_cnt,"%4d",dd[kk].df_cnt);

      if (dd[kk].jf_cnt==0)
        strcpy(s_jf_cnt,"    ");
      else
        sprintf(s_jf_cnt,"%4d",dd[kk].jf_cnt);

      fprintf(fp_o,"��%18s��%4s��%10s��%4s��%10s��%6s��%12s��%12s��%4s��%4s��\n",mc,s_clos,s_jf_amt,s_open,s_df_amt,s_rest,s_jf_bal,s_df_bal,s_jf_cnt,s_df_cnt);
      if (dd[i].bz==1)
      {
        dd[0].clos  +=dd[i].clos;
        dd[0].jf_amt+=dd[i].jf_amt;
        dd[0].open  +=dd[i].open;
        dd[0].df_amt+=dd[i].df_amt;
        dd[0].rest  +=dd[i].rest;
        dd[0].jf_bal+=dd[i].jf_bal;
        dd[0].df_bal+=dd[i].df_bal;
        dd[0].jf_cnt+=dd[i].jf_cnt;
        dd[0].df_cnt+=dd[i].df_cnt;
      }
    }
    fclose(fp_o);
    clear_scr(6,20,22,77);
    dis_str(22,22,"�Ƿ��ӡ����(y/n):[ ]",0,0);
    flag=yorn(22,48);
    if(flag=='y'||flag=='Y') 
    {
      dis_err("��׼���ô�ӡ������һ����ʼ��ӡ");
      sprintf(cmd_str,"pr %s> "Printer,fname);
      system(cmd_str);
    }
  }
  mysql_stmt_close(stmt);
  
  clear_l22();
  return(0);
}
