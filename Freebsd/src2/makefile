
INCLUDE = -I/usr/local/include/mysql
LIB = -L/usr/lib -L/usr/local/lib/mysql

CC = gcc
CFLAGS = -g -O $(INCLUDE)
LDFLAGS = -lcurses -lmysqlclient

OBJS = op_main.o tools.o tools_adm.o st_table.o input_data.o s_open.o\
 s_depositary.o s_withdraw.o s_close.o s_lost.o s_unlost.o s_correct.o\
 s_query.o c_open.o c_depositary.o c_withdraw.o c_close.o c_lost.o\
 c_unlost.o c_correct.o c_query.o pr_h.o pr_code.o zh_cro_sum.o\
 zh_cro_vou.o zh_pro_sum.o zh_pro_vou.o zh_quer_sum.o zh_quer_vou.o\
 total_br.o total_op.o qtrade_op.o qtrade_ser.o query_chk.o server_cal.o\
 update_pwd.o hl_dev.o s_pwreset.o c_scard.o s_chghb.o

main: $(OBJS)
	$(CC) $(LIB) $(LDFLAGS) -o $@ $(OBJS)
	
%.o: %.c

