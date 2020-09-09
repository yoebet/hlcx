/****************************************************************
������:end_br.ec
������:end_br()
��  ��:������־��ָ�����ŵ�����
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <sys/ipc.h>
#include "mysql.h"
#include "../include/sh_table.h"

extern MYSQL *conn;
extern char sql[];

MYSQL *conn2;

void _clscon_e(){
	if(conn2!=NULL)mysql_close(conn2);
}

end_br()
{
  
  struct f_branch br;
  struct f_sumtable sum,mysum;
  struct f_vouch vou,myvou;
  short debug;
  char yy,sys_err_str[100];
  long  item;
  long  ldate;
  double  balance;
  short  kind,rest_que; 
  short mdy_today[3],mdy_today1[3],ymd[3];
	char cond[100];
	char *vouch_u_bkt="update vouch set branch=%d,kind=%d,flag='%s',vou_type='%s',\
	    rest_que=%ld,use_que=%ld,cancel=%ld,chg_book=%ld,takeback=%ld\
	     where branch=%d and kind=%d and vou_type='%s'";

  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res2;
  MYSQL_ROW row2;

  static short in[2]={27,8};
  short num=2,flag;
  struct in_data da;

  init_in(&da);

  while(1)
  {
    clear_scr(5,22,22,77);
    mvaddstr(5,30,"����������ҵ��");
    refresh();

    dis_str(22,22,"���ڴ���,���Ժ�......",0,0);  

	  da.tr_br=100;
	
    
    sprintf(sql,"SELECT * FROM br_file WHERE  branch= %d ",da.tr_br);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"ȡ����¼��!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      dis_str(22,22,"����������!",0,0);  
      getch();
      return(-1);
		}
    set_f_branch(&br,row);
    mysql_free_result(res);
	
    if (br.init_flag>0)
    {
      dis_str(22,22,"���������ѽ���!",60,0);
      refresh();
      getch();
      return(0);
    }
    if (br.sum_flag==0)
    {
      dis_str(22,22,"����δ������!",60,0);
      refresh();
      getch();
      return(0);
    }
    rjulmdy(br.work_d,mdy_today);

    //ת��������������
    sprintf(sql,"SELECT * FROM sumtable WHERE kind=90 and branch=%d",da.tr_br);
	
    if(mysql_query(conn,sql)){
      sprintf(sys_err_str,"�޸������ļ���!%s.",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      getch();
      return(-1);
    }

    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      sprintf(sys_err_str,"�޸������ļ���!");
      dis_str(22,22,sys_err_str,60,0); 
      getch();
      return(-1);
		}
    
    if(conn2==NULL){
    	conn2=mysql_init(NULL);
      atexit(_clscon_e);
	    if(!connectdb(conn2)){
	      sprintf(sys_err_str,"%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      getch();
	      return(-1);
	    }
    }
    
    /***********************************/
    
    if(mysql_query(conn2,"START TRANSACTION")){
      sprintf(sys_err_str,"������������");
      dis_err(sys_err_str);
      return(-1);
    }
    
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      set_f_sumtable(&sum,row);
      
      note("���汾�ռ�¼ ����ĵڼ���+200");
      ymd[2]=mdy_today[2];
      ymd[0]=1;
      ymd[1]=1;
      rmdyjul(ymd,&ldate);
      //ldate=br.work_d-ldate+200;
      //br.work_d��������
      ldate=(br.work_d-ldate)/(24*60*60)+200;
      note("д����Ŀ�ı������ʼ�¼");
      //mysum.kind=ldate;
      sum.kind=ldate;
      
      //ֱ��insert����
      set_f_sumtable_i_sql(&sum,sql);
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"���汾�����ʴ�!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }

      note("�޸���������");
      sum.kind=91;
      
      set_f_sumtable_u_sql(&sum,sql);
      sprintf(cond,"branch=%d and kind=%d and item_num=%ld",da.tr_br,sum.kind,sum.item_num);
      strcat(sql,cond);
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"�޸��������ʴ�!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }
	
	  note("ȡ����Ŀ�ı������ʼ�¼");
	  sum.kind=mdy_today[0];
	  
    sprintf(sql,"SELECT * FROM sumtable WHERE branch=%d and kind=%d and item_num=%ld",
      sum.branch,sum.kind,sum.item_num);
	
    if(mysql_query(conn2,sql)){
      sprintf(sys_err_str,"ȡ�������ʴ�!%s.",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
    if(row2==NULL){
      sprintf(sys_err_str,"ȡ�������ʴ�!");
      dis_str(22,22,sys_err_str,60,0);
    	mysql_rollback(conn2);
    	mysql_free_result(res);
      getch();
    	return(1);
    }
    set_f_sumtable(&mysum,row2);
	
	  note("�ۼӱ��մ������������");
	  mysum.det_amt+=sum.det_amt;
	
	  note("�ۼӱ��ս跽���������");
	  mysum.cre_amt+=sum.cre_amt;
	
	  note("�������");
	  mysum.balance=sum.balance;
	
	  note("�ۼƱ���������");
	  mysum.open_que+=sum.open_que;
	  mysum.cls_que+=sum.cls_que;
	  mysum.dvou_que+=sum.dvou_que;
	  mysum.cvou_que+=sum.cvou_que;
	  mysum.rest_que=sum.rest_que;//?
	  note("�޸ı���XX��¼(������90�ۼ��뱾��XX");
	  
    set_f_sumtable_u_sql(&mysum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"�޸ı������ʴ�!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
	
	  note("����������90�ۼ��뱾������99��¼");
	  sum.kind=99;
	  
    sprintf(sql,"SELECT * FROM sumtable WHERE branch=%d and kind=%d and item_num=%ld",
      sum.branch,sum.kind,sum.item_num);
	
    if(mysql_query(conn2,sql)){
      sprintf(sys_err_str,"ȡ�������ʴ�!%s.",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
    if(row2==NULL){
      sprintf(sys_err_str,"ȡ�������ʴ�!");
      dis_str(22,22,sys_err_str,60,0);
    	mysql_rollback(conn2);
      mysql_free_result(res);
    	return(0);
    }
    set_f_sumtable(&mysum,row2);
	  
	  mysum.det_amt+=sum.det_amt;
	  mysum.cre_amt+=sum.cre_amt;
	  mysum.balance=sum.balance;
	  mysum.open_que+=sum.open_que;
	  mysum.cls_que+=sum.cls_que;
	  mysum.dvou_que+=sum.dvou_que;
	  mysum.cvou_que+=sum.cvou_que;
	  mysum.rest_que=sum.rest_que;
	  
    set_f_sumtable_u_sql(&mysum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"�޸ı������ʴ�!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }	
	
	  note("����������90����");
	  sum.det_amt=0;
	  sum.cre_amt=0;
	  sum.open_que=0;
	  sum.cls_que=0;
	  sum.dvou_que=0;
	  sum.cvou_que=0;
	  sum.kind=90;
	  
    set_f_sumtable_u_sql(&sum,sql);
    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
    strcat(sql,cond);
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"�޸ı������ʴ�!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
	
		  note("���12��31��");
		  if(mdy_today[1]==31&&mdy_today[0]==12) 
		  {
		    note("���12��31��ʱ,������99д������0��¼");
		    mysum.kind=0;
		    
	    set_f_sumtable_u_sql(&mysum,sql);
	    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",mysum.branch,mysum.kind,mysum.item_num);
	    strcat(sql,cond);
	    if(mysql_query(conn2,sql))
	    {
	      sprintf(sys_err_str,"�޸ı������ʴ�!%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      mysql_rollback(conn2);
        mysql_free_result(res);
	      getch();
	      return(-1);
	    }
	
	    sum.balance=0;
	    sum.rest_que=0;
	    sum.kind=99;
	    note("�屾��99��¼");
	    
	    set_f_sumtable_u_sql(&sum,sql);
	    sprintf(cond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
	    strcat(sql,cond);
	    if(mysql_query(conn2,sql))
	    {
	      sprintf(sys_err_str,"�屾��99��¼���ʴ�!%s",mysql_error(conn2));
	      dis_str(22,22,sys_err_str,60,0);
	      mysql_rollback(conn2);
        mysql_free_result(res);
	      getch();
	      return(-1);
	    }
	  
	    note("��1-12��99��¼");
	    set_f_sumtable_u_sql(&sum,sql);
	    int sqllen=strlen(sql);
	    char *sqlcond=sql+sqllen;
	    for(sum.kind=1;sum.kind<=12;sum.kind++)
	    {
	      sprintf(sqlcond,"branch=%d and kind=%d and item_num=%ld",sum.branch,sum.kind,sum.item_num);
	      //strcat(sql,cond);
		    if(mysql_query(conn2,sql))
		    {
		      sprintf(sys_err_str,"��1-12��99��¼���ʴ�!%s",mysql_error(conn2));
		      dis_str(22,22,sys_err_str,60,0);
		      mysql_rollback(conn2);
          mysql_free_result(res);
		      getch();
		      return(-1);
		    }
	    }
	   }

    }
    mysql_free_result(res);


    //ת��ƾ֤�ļ���������
    sprintf(sql,"select * from vouch where kind=90 and branch=%d",da.tr_br);;
    if(mysql_query(conn,sql))
    {
      sprintf(sys_err_str,"�޸�ƾ֤�ļ���!%s",mysql_error(conn));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      getch();
      return(-1);
    }
    res=mysql_store_result(conn);
    row=mysql_fetch_row(res);
		if(row==NULL){
      sprintf(sys_err_str,"û��ƾ֤!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      getch();
      return(-1);
		}
    
    for(;row!=NULL;row=mysql_fetch_row(res))
    {
      if(row==NULL) break;
      set_f_vouch(&vou,row);
      
      vou.kind=91; 
      sprintf(sql,vouch_u_bkt,
        vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
        vou.cancel,vou.chg_book,vou.takeback,da.tr_br,vou.kind,vou.vou_type);
      
      if(mysql_query(conn2,sql))
      {
        sprintf(sys_err_str,"�޸�����ƾ֤�ļ���!%s",mysql_error(conn2));
        dis_str(22,22,sys_err_str,60,0);
        mysql_rollback(conn2);
        mysql_free_result(res);
        getch();
        return(-1);
      }

	  note("ȡ��ƾ֤�ı��¼�¼");
	  vou.kind=mdy_today[0];
	
	  sprintf(sql,"select * from vouch where branch=%d and kind=%d and vou_type='%s'",
	    vou.branch,vou.kind,vou.vou_type);
	
	  if(mysql_query(conn2,sql))
	  {
	     sprintf(sys_err_str,"ȡ���¼�¼��!%s",mysql_error(conn2));
	     dis_str(22,22,sys_err_str,60,0);
       mysql_rollback(conn2);
       mysql_free_result(res);
	     getch();
	    return(1);
	  }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
		if(row2==NULL){
      sprintf(sys_err_str,"ȡ���¼�¼��!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
		}
    set_f_vouch(&myvou,row2);
    mysql_free_result(res);
    
  //myvou.open_que+=vou.open_que;
  myvou.cancel+=vou.cancel;
  myvou.chg_book+=vou.chg_book;
  myvou.takeback+=vou.takeback;
  myvou.rest_que=vou.rest_que;
  
  sprintf(sql,vouch_u_bkt,
    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
  
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"�޸ı������ʴ�!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }

  vou.kind=99;
  
    sprintf(sql,"select * from vouch where branch=%d and kind=%d and vou_type='%s'",
      vou.branch,vou.kind,vou.vou_type);;
    if(mysql_query(conn2,sql))
    {
      sprintf(sys_err_str,"ȡ99�������ʴ�!%s",mysql_error(conn2));
      dis_str(22,22,sys_err_str,60,0);
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
    }
    res2=mysql_store_result(conn2);
    row2=mysql_fetch_row(res2);
		if(row2==NULL){
      sprintf(sys_err_str,"ȡ99�������ʴ�!");
      dis_str(22,22,sys_err_str,60,0); 
      mysql_rollback(conn2);
      mysql_free_result(res);
      getch();
      return(-1);
		}
    set_f_vouch(&myvou,row2);
		
//?  myvou.open_que+=vou.open_que;
  myvou.cancel+=vou.cancel;
  myvou.chg_book+=vou.chg_book;
  myvou.takeback+=vou.takeback;
  myvou.rest_que=vou.rest_que;
  
  sprintf(sql,vouch_u_bkt,
    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"�޸�99�������ʴ�!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }

  //vou.open_que=0;
  vou.cancel=0;
  vou.use_que=0;
  vou.chg_book=0;
  vou.takeback=0;
  vou.kind=90;
  
  sprintf(sql,vouch_u_bkt,
    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
  if(mysql_query(conn2,sql))
  {
    sprintf(sys_err_str,"��90�������ʴ�!%s",mysql_error(conn2));
    dis_str(22,22,sys_err_str,60,0);
    mysql_rollback(conn2);
    mysql_free_result(res);
    getch();
    return(-1);
  }
  
  if(mdy_today[1]==31&&mdy_today[0]==12)
  {
    myvou.kind=0;
	  sprintf(sql,vouch_u_bkt,
	    myvou.branch,myvou.kind,myvou.flag,myvou.vou_type,myvou.rest_que,myvou.use_que,
	    myvou.cancel,myvou.chg_book,myvou.takeback,myvou.branch,myvou.kind,myvou.vou_type);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"����������ʴ�!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
      mysql_free_result(res);
	    getch();
	    return(-1);
	  }

    vou.rest_que=0;
    vou.kind=99;
	  sprintf(sql,vouch_u_bkt,
	    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
	    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"�޸���99�������ʴ�!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
      mysql_free_result(res);
	    getch();
	    return(-1);
	  }

    for(vou.kind=1;vou.kind<=12;vou.kind++)
    {
		  sprintf(sql,vouch_u_bkt,
		    vou.branch,vou.kind,vou.flag,vou.vou_type,vou.rest_que,vou.use_que,
		    vou.cancel,vou.chg_book,vou.takeback,vou.branch,vou.kind,vou.vou_type);
		  if(mysql_query(conn2,sql))
		  {
		    sprintf(sys_err_str,"�޸������ʴ�!%s",mysql_error(conn2));
		    dis_str(22,22,sys_err_str,60,0);
		    mysql_rollback(conn2);
        mysql_free_result(res);
		    getch();
		    return(-1);
		  }
    }
  }
}
  mysql_free_result(res);

    note("��������ӡ��־,�����ʱ�־����ˮ����,��ʼ����־��1");
    
	  sprintf(sql,"update br_file set sum_flag=0,pr_flag=0,ser_num=0,init_flag=1");
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"�޸ı����ļ���!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
	    getch();
	    return(-1);
	  }

    note("��������ˮ���!");
    
	  sprintf(sql,"delete from trade where branch=%d",da.tr_br);
	  if(mysql_query(conn2,sql))
	  {
	    sprintf(sys_err_str,"��ձ�����־��!%s",mysql_error(conn2));
	    dis_str(22,22,sys_err_str,60,0);
	    mysql_rollback(conn2);
	    getch();
	    return(-1);
	  }

    mysql_commit(conn2);
     break;
  }
  dis_err("������ϣ�");
  return(0);
}

dis_err_date(long dd)
{
  short ymd[3];
  char tmp_str[20];

  rjulmdy(dd,ymd);
  sprintf(tmp_str,"%d��%d��%d��",ymd[2],ymd[0],ymd[1]);
  dis_err(tmp_str);
}
