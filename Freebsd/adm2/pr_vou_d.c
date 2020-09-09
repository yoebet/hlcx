/******************************************************************
* ������:pr_vou_d.ec 
* ������:pr_vou_d(br,flag)
* ��  ��:��ӡָ�������ձ�/�±�/�걨����
* ��  ��:br   ���� 999_ȫ���� A99_A���´� 000_�˳� ABC_ָ����
         flag ��־  0���걨 1-12ĳ�±� 90�ձ� 99���걨
*******************************************************************/
#include<stdio.h>
#include<string.h>

#include "mysql.h"
#include "../include/apt.h"
#include "../include/sh_table.h"

extern char sys_err_str[96];
extern MYSQL *conn;

extern char sql[];

pr_vou_d(br,flag)
short br,flag;
{

  /**�����ʽ����,�����ӡ���˳��һ��**/
  struct vou
  {
    char type[3];
    char mc[25];
    long yesd;
    long back;
    long open;
    long change;
    long cancel;
    long rest;
  }dd[100];
  /**ƾ֤���ʽ����,��vouth���˳��һ��,���ڶ�ȡ����**/
  /*
  $struct sum
  {
    short br;
    short kind;
    char flag[2];
    char type[3];
    long  rest;
    long open;
    long cancel;
    long chg_book;
    long takeback;
  } ss;
  */
  struct f_vouch ss;

  /**�����ʽ�ַ���,�����ӡ���һ��,Ϊ���������**/
  char mc[25],s_yesd[9],s_back[9],s_open[5];
  char s_rest[9],s_change[5],s_cancel[5];
  char s_jf_amt[16],s_df_amt[16],s_ye_bal[16],s_df_bal[16];
  double bal,jf_amt,df_amt,ye_bal,df_bal;

  FILE *fp_i,*fp_o;
  short branch,kind;
  long vou;
  char fname[50],vou_name[25],s_vou[7],s_bz[3];
  char branch_name[11],s_type[3];
  short i0,kk,i,j,ii,num,ymd[3];
  char buffer[27],*sp;
  long ltoday;
  char bt0[5];
  short br_id[130],s_br,e_br;
  char br_name[130][11];
  short i_br,k,br_num,vou_num,jj;
  char cmd_str[100];

  MYSQL_RES *res;
  MYSQL_ROW row;

  note("�������ɸ���!");

  note("���ձ��������ļ����γ���������!");
  if((fp_i=fopen("./data/def_vou_d","r"))==NULL)
  {	
    dis_err("�ձ��������ļ�def_vou_dδ�ҵ�!");
    return(1);
  }

  i=1;
  while (fgets(buffer,29,fp_i))
  {
    if (feof(fp_i))
    {
      fclose (fp_i);
      break;
    }
    buffer[26]='\0';
    sp=buffer;
    j=0;
    memcpy(s_type,sp+j,2); 
    s_type[2]='\0';
    j+=2;
    memcpy(vou_name,sp+j,24); 
    vou_name[24]='\0';
    strcpy(dd[i].type,s_type);
    strcpy(dd[i].mc,vou_name);
    if (atoi(s_type)==22)
      break;
    i=i+1;
  }
  vou_num=i;
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
  if (br==i_br*100)
  {
    s_br=i_br*100+1;
    e_br=i_br*100+99; 
  }
  
  /********************************/
  
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
    br_id[k]=999;
    if((row=mysql_fetch_row(res))==NULL||mysql_errno(conn)!=0)
      break;
	br_id[k]=atoi(row[0]);
	strcpy(br_name[k],row[1]);
  }
  mysql_free_result(res);
  
	/**********************************/

  sprintf(sql,"SELECT * FROM vouch WHERE branch=? AND kind=? AND vou_type=? ");
  
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
  param_bind[2].buffer= &s_type;
  
  memset(result_bind, 0, sizeof(result_bind));
  
  result_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[0].buffer= &ss.branch;
  result_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  result_bind[1].buffer= &ss.kind;
  result_bind[2].buffer_type= MYSQL_TYPE_STRING;
  result_bind[2].buffer= &ss.flag;
  result_bind[2].buffer_length=2;
  result_bind[3].buffer_type= MYSQL_TYPE_STRING;
  result_bind[3].buffer= &ss.vou_type;
  result_bind[3].buffer_length=3;
  result_bind[4].buffer_type= MYSQL_TYPE_LONG;
  result_bind[4].buffer= &ss.rest_que;
  result_bind[5].buffer_type= MYSQL_TYPE_LONG;
  result_bind[5].buffer= &ss.use_que;
  result_bind[6].buffer_type= MYSQL_TYPE_LONG;
  result_bind[6].buffer= &ss.cancel;
  result_bind[7].buffer_type= MYSQL_TYPE_LONG;
  result_bind[7].buffer= &ss.chg_book;
  result_bind[8].buffer_type= MYSQL_TYPE_LONG;
  result_bind[8].buffer= &ss.takeback;
	
  /********************************/
   
   
  if (k==1)
    br_num=1;
  else
    br_num=k-1;

  for(ii=1;ii<=br_num;ii++)
  {
    if (br_id[ii]==999)
      continue;
    sprintf(sys_err_str,"���ڴ���[%3d_%10s]������:\n",br_id[ii],br_name[ii]);
    note(sys_err_str);
    branch=br_id[ii];
    strcpy(branch_name,br_name[ii]);

    note("��ʼ�������ʽ����!");
    for (i=0;i<=vou_num;i++)
    {  
      dd[i].yesd=0;
      dd[i].back=0.0;
      dd[i].open=0;
      dd[i].change=0.0;
      dd[i].cancel=0.0;
      dd[i].rest=0;
    }

    for (i=1;i<=vou_num;i++)
    {  
      strcpy(s_type,dd[i].type);
      strcpy(vou_name,dd[i].mc);
      i0=atoi(s_type);
      if (i0==11 || i0==22 ||i0==33)
        continue;
      sprintf(sys_err_str,"���ڶ�ƾ֤  ����:[%2s]!",s_type);
      note(sys_err_str);
	  
	  /********************************/
	  
	  if (mysql_stmt_bind_param(stmt, param_bind)||
			mysql_stmt_execute(stmt)||
			mysql_stmt_bind_result(stmt, result_bind)||
			mysql_stmt_store_result(stmt)||
			mysql_stmt_fetch(stmt))
	  {
        sprintf(sys_err_str,"��ƾ֤��[%ld],��־[%d]!",kind);
        dis_err(sys_err_str);
        continue;
	  }
	  mysql_stmt_free_result(stmt);
	  
	  /********************************/
	  
      dd[i].rest=ss.rest_que;
      dd[i].open=ss.use_que;
      dd[i].cancel=ss.cancel;
      dd[i].change=ss.chg_book;
      dd[i].back=ss.takeback;
      dd[i].yesd=ss.rest_que+ss.use_que+ss.cancel+ss.chg_book-ss.takeback;
    }
  
    note("�������ɸ���!");
    sprintf(fname,"./tmp/tmp_vou_d_%d",branch);
    if((fp_o=fopen(fname,"w"))==NULL)
    {	
      dis_err("�ձ������ļ���������!");
      return(1);
    }
    fprintf(fp_o,"%27sӪҵ%s����\n", " ",bt0);
    fprintf(fp_o,"%5s ����:[%d]    ����:[%s] %30s �Ʊ�����:%4d��%02d��%02d��      \n"," ",branch,branch_name," ",ymd[2],ymd[0],ymd[1]);
    fprintf(fp_o,"���������������������������ש������������������ש������������������ש����������������ש�������������������\n");
    fprintf(fp_o,"��                        ��                                      ��    ����֧��    ��                  ��\n");
    fprintf(fp_o,"��     ��  ��  ��  Ŀ     ��     ���տ��     ��     ��������     �ǩ����ש����ש�����     ���ս��     ��\n");
    fprintf(fp_o,"��                        ��                  ��                  �����������۩����ϩ�                  ��\n");
    fprintf(fp_o,"�ǩ������������������������贈�����������������贈�����������������贈���贈���贈���贈������������������\n");

    for (i=1;i<=vou_num;i++)
    {
      if (atoi(dd[i].type)==33)
      {
        fprintf(fp_o,"��%24s��                  ��                  ��    ��    ��    ��                  ��\n",dd[i].mc);
        continue;
      }
      if (atoi(dd[i].type)==11)
      {
        fprintf(fp_o,"�ǩ������������������������贈�����������������贈�����������������贈���贈���贈���贈������������������\n");
        continue;
      }
      if (atoi(dd[i].type)==22)
      {
        fprintf(fp_o,"���������������������������ߩ������������������ߩ������������������ߩ����ߩ����ߩ����ߩ�������������������\n");
        break;
      }
      if (dd[i].yesd==0)
        strcpy(s_yesd,"        ");
      else
        sprintf(s_yesd,"%8d",dd[i].yesd);

      if (dd[i].back==0)
        strcpy(s_back,"        ");
      else
        sprintf(s_back,"%8d",dd[i].back);

      if (dd[i].open==0)
        strcpy(s_open,"    ");
      else
        sprintf(s_open,"%4d",dd[i].open);

      if (dd[i].change==0)
        strcpy(s_change,"    ");
      else
        sprintf(s_change,"%4d",dd[i].change);

      if (dd[i].cancel==0)
        strcpy(s_cancel,"    ");
      else
        sprintf(s_cancel,"%4d",dd[i].cancel);

      if (dd[i].rest==0)
        strcpy(s_rest,"        ");
      else
        sprintf(s_rest,"%8d",dd[i].rest);
      fprintf(fp_o,"��%24s��     %8s     ��     %8s     ��%4s��%4s��%4s��     %8s     ��\n",dd[i].mc,s_yesd,s_back,s_open,s_change,s_cancel,s_rest);
    }

    //fprintf(fp_o,"���������������������������ߩ������������������ߩ������������������ߩ����������������ߩ�������������������\n");
    fprintf(fp_o,"                  �Ʊ�:                                                      ����:\n");
    fclose(fp_o);
    clear_scr(5,22,22,77);
    dis_str(22,22,"�Ƿ��ӡ����(y/n):[ ]",0,0);
    flag=yorn(22,48);
    if(flag=='y'||flag=='Y') 
    {
      dis_err("��׼���ô�ӡ������һ����ʼ��ӡ!");
      sprintf(cmd_str,"pr %s> "Printer,fname);
      system(cmd_str);
    }
  }
  return(0);
}
