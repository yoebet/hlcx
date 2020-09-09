
/**
根据结果集的行设置相应的结构体，生成部分sql语句。注意字段顺序
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "mysql.h"
#include "../include/sh_table.h"

void set_f_operator(struct f_operator *op,MYSQL_ROW row){
	
	memset(op, 0, sizeof(*op));
	op->operator=atoi(row[0]);
	strcpy(op->password,row[1]);
	strcpy(op->op_level,row[2]);
	strcpy(op->op_name,row[3]);
	strcpy(op->op_flag,row[4]);
	strcpy(op->tty_no,row[5]);
	op->tty_pid=atoi(row[6]);
	op->ser_num=atoi(row[7]);
}


void set_f_branch(struct f_branch *br,MYSQL_ROW row){
	 
	br->branch=atoi(row[0]);
	strcpy(br->br_name,row[1]);
	strcpy(br->br_flag,row[2]);
	br->work_d=atol(row[3]);
	br->pr_flag=atoi(row[4]);
	br->sum_flag=atoi(row[5]);
	br->init_flag=atoi(row[6]);
	br->ser_num=atoi(row[7]);
}

void set_s_trade(struct s_trade *trade,MYSQL_ROW row){
	 
	//memset(&trade, 0, sizeof(trade));
	short i=0;
	trade->branch=atoi(row[i++]);
	trade->operator=atoi(row[i++]);
	trade->ser_num=atoi(row[i++]);
	trade->tr_op=atoi(row[i++]);
	trade->tr_code=atoi(row[i++]);
	trade->amount=atol(row[i++]);
	strcpy(trade->correct,row[i++]);
	strcpy(trade->type,row[i++]);
	trade->term=atoi(row[i++]);
	trade->open_br=atoi(row[i++]);
	trade->account=atol(row[i++]);
	strcpy(trade->card_acc,row[i++]);
	strcpy(trade->acc_type,row[i++]);
	strcpy(trade->customer,row[i++]);
	strcpy(trade->abstract,row[i++]);
	trade->open_d=atol(row[i++]);
	trade->close_d=atol(row[i++]);
	trade->balance=atol(row[i++]);
	trade->interest=atol(row[i++]);
	trade->s_int=atol(row[i++]);
	trade->tax_int=atol(row[i++]);
	trade->transfer=atol(row[i++]);
	trade->det_num=atoi(row[i++]);
	trade->item_num=atol(row[i++]);
}

void set_s_trade_i_sql(struct s_trade *tt,char *sql){
	
  sprintf(sql,"insert into trade values(%d,%d,%d,%d,%d,%ld,'%s','%s',%d,%d,%ld,'%s',\
    '%s','%s','%s',%ld,%ld,%ld,%ld,%ld,%ld,%ld,%d,%ld)",
    tt->branch,tt->operator,tt->ser_num,tt->tr_op,tt->tr_code,tt->amount,tt->correct,tt->type,tt->term,
    tt->open_br,tt->account,tt->card_acc,tt->acc_type,tt->customer,tt->abstract,tt->open_d,tt->close_d,
    tt->balance,tt->interest,tt->s_int,tt->tax_int,tt->transfer,tt->det_num,tt->item_num);
}

void set_f_item(struct f_item *it,MYSQL_ROW row){
	 
	strcpy(it->type,row[0]);
	strcpy(it->acc_type,row[1]);
	it->term=atoi(row[2]);
	it->item_num=atol(row[3]);
	strcpy(it->item_name,row[4]);
	strcpy(it->item_flag,row[5]);
	it->sum_flag=atoi(row[6]);
	it->prt_ser=atoi(row[7]);
}

void set_f_voufile(struct f_voufile *vo,MYSQL_ROW row){
	
	strcpy(vo->vou_type,row[0]);
	strcpy(vo->vou_name,row[1]);
	strcpy(vo->flag,row[2]);
	strcpy(vo->type,row[3]);
	vo->sum_flag=atol(row[4]);
	vo->prt_ser=atoi(row[5]);
}

void set_f_vouch(struct f_vouch *vou,MYSQL_ROW row){
	
	vou->branch=atoi(row[0]);
	vou->kind=atoi(row[1]);
	strcpy(vou->flag,row[2]);
	strcpy(vou->vou_type,row[3]);
	vou->rest_que=atol(row[4]);
	vou->use_que=atol(row[5]);
	vou->cancel=atol(row[6]);
	vou->chg_book=atol(row[7]);
	vou->takeback=atol(row[8]);
}

void set_f_rate(struct f_rate *rat,MYSQL_ROW row){
	
	strcpy(rat->ltype,row[0]);
	rat->term=atoi(row[1]);
	rat->int_date=atol(row[2]);
	rat->rate=atol(row[3]);
}

void set_ss_mast(struct ss_mast *smb,MYSQL_ROW row){
	
	short i=0;
	smb->open_br=atoi(row[i++]);
	smb->account=atol(row[i++]);
	strcpy(smb->acc_type,row[i++]);
	strcpy(smb->card_acc,row[i++]);
	strcpy(smb->card_type,row[i++]);
	strcpy(smb->customer,row[i++]);
	strcpy(smb->address,row[i++]);
	strcpy(smb->passwd,row[i++]);
	strcpy(smb->zj_kind,row[i++]);
	strcpy(smb->zj_num,row[i++]);
	smb->acc_open_d=atol(row[i++]);
	smb->card_open_d=atol(row[i++]);
	smb->acc_close_d=atol(row[i++]);
	smb->card_close_d=atol(row[i++]);
	smb->balance=atol(row[i++]);
	smb->det_num=atoi(row[i++]);
	smb->interest=atol(row[i++]);
	smb->tax_int=atol(row[i++]);
	strcpy(smb->acc_flag,row[i++]);
	smb->transfer=atol(row[i++]);
	smb->item_num=atol(row[i++]);
	smb->last_day=atol(row[i++]);
	smb->open_op=atoi(row[i++]);
	smb->close_br=atoi(row[i++]);
	smb->close_op=atoi(row[i++]);
}

void set_ss_mast_i_sql(struct ss_mast *ss,char *sql){
	
	sprintf(sql,"insert into s_mast values(%d,%ld,'%s','%s','%s','%s','%s','%s',\
	  '%s','%s',%ld,%ld,%ld,%ld,%ld,%d,%ld,%ld,'%s',%ld,%ld,%ld,%d,%d,%d)",
	  ss->open_br,ss->account,ss->acc_type,ss->card_acc,ss->card_type,ss->customer,
	  ss->address,ss->passwd,ss->zj_kind,ss->zj_num,ss->acc_open_d,ss->card_open_d,
	  ss->acc_close_d,ss->card_close_d,ss->balance,ss->det_num,ss->interest,ss->tax_int,
	  ss->acc_flag,ss->transfer,ss->item_num,ss->last_day,ss->open_op,ss->close_br,
	  ss->close_op);
}

void set_ss_mast_u_sql(struct ss_mast *ss,char *sql){
	
	sprintf(sql,"update s_mast set \
	  open_br=%d,account=%ld,acc_type='%s',card_acc='%s',card_type='%s',customer='%s',\
	  address='%s',acc_open_d=%ld,card_open_d=%ld,acc_close_d=%ld,card_close_d=%ld,\
	  balance=%ld,det_num=%d,interest=%ld,tax_int=%ld,acc_flag='%s',transfer=%ld,\
	  item_num=%ld,last_d=%ld,open_op=%d,close_br=%d,close_op=%d\
	  where ",
	  ss->open_br,ss->account,ss->acc_type,ss->card_acc,ss->card_type,ss->customer,
	  ss->address,ss->acc_open_d,ss->card_open_d,ss->acc_close_d,ss->card_close_d,
	  ss->balance,ss->det_num,ss->interest,ss->tax_int,ss->acc_flag,ss->transfer,
	  ss->item_num,ss->last_day,ss->open_op,ss->close_br,ss->close_op);
}


void set_s_tax_i_sql(struct s_tax *tax,char *sql){
	
	sprintf(sql,"insert into tax values(%d,%ld,'%s','%s',%ld,%ld,%ld,%ld,%d,%ld,%ld,\
	  '%s','%s',%ld,%ld,%ld,%d,%ld,%ld,'%s',%d,%ld,'%s',%ld,'%s')",
	  tax->branch,tax->account,tax->card_acc,tax->customer,tax->open_d,tax->start_d,
	  tax->close_d,tax->end_d,tax->term,tax->balance,tax->interest,tax->acc_type,
	  tax->acc_flag,tax->transfer,tax->item_num,tax->rate,tax->close_op,tax->tr_country,
	  tax->taxamt,tax->withdraw,tax->ser_num,tax->back1,tax->back2,tax->back3,tax->back4);
}

void set_ss_detail(struct ss_detail *sd,MYSQL_ROW row){
	
	sd->open_br=atoi(row[0]);
	sd->account=atol(row[1]);
	strcpy(sd->card_acc,row[2]);
	sd->det_num=atoi(row[3]);
	sd->tr_date=atol(row[4]);
	sd->amount=atol(row[5]);
	strcpy(sd->abstract,row[6]);
	strcpy(sd->flag,row[7]);
	sd->tr_br=atoi(row[8]);
	sd->tr_op=atoi(row[9]);
	sd->balance=atol(row[10]);
}

void set_f_sumtable(struct f_sumtable *sum,MYSQL_ROW row){
	
	sum->branch=atoi(row[0]);
	sum->kind=atoi(row[1]);
	sum->item_num=atol(row[2]);
	strcpy(sum->flag,row[3]);
	sum->det_amt=atof(row[4]);//要include stdlib.h，否则得不到期望结果
	//sscanf(row[4],"%lf",&sum->det_amt);
	sum->cre_amt=atof(row[5]);
	//sscanf(row[5],"%lf",&sum->cre_amt);
	sum->open_que=atol(row[6]);
	sum->cls_que=atol(row[7]);
	sum->dvou_que=atol(row[8]);
	sum->cvou_que=atol(row[9]);
	sum->balance=atof(row[10]);
	//sscanf(row[10],"%lf",&sum->balance);
	sum->rest_que=atol(row[11]);
}

void set_f_sumtable_u_sql(struct f_sumtable *sum,char *sql){
	
	sprintf(sql,"update sumtable set \
	  branch=%d,kind=%d,item_num=%ld,flag='%s',det_amt=%f,cre_amt=%f,open_que=%ld,\
	  cls_que=%ld,dvou_que=%ld,cvou_que=%ld,balance=%f,rest_que=%ld where ",
	  sum->branch,sum->kind,sum->item_num,sum->flag,sum->det_amt,sum->cre_amt,sum->open_que,
	  sum->cls_que,sum->dvou_que,sum->cvou_que,sum->balance,sum->rest_que);
}

void set_f_sumtable_i_sql(struct f_sumtable *sum,char *sql){
	
	sprintf(sql,"insert into sumtable values(%d,%d,%ld,'%s',%f,%f,%ld,%ld,%ld,%ld,%f,%ld)",
	  sum->branch,sum->kind,sum->item_num,sum->flag,sum->det_amt,sum->cre_amt,sum->open_que,
	  sum->cls_que,sum->dvou_que,sum->cvou_que,sum->balance,sum->rest_que);
}
