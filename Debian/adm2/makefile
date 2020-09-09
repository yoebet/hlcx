
INCLUDE = -I/usr/local/include/mysql -I/usr/include/ncursesw
LIB = -L/usr/lib

CC = gcc
CFLAGS = -g -O $(INCLUDE)
LDFLAGS = -lncursesw -lmysqlclient

OBJS = main.o tools.o op_exit.o pr_daily.o st_table.o backup_disk1.o\
 pr_total.o pr_balance.o pr_table.o input_data.o pr_tab_d.o add_oper.o\
 pr_vou_d.o pr_detail.o pr_account.o pr_open.o pr_close.o rate_query.o\
 rate_add.o rate_del.o rate_print.o rate_proc.o rate_s_prt.o del_oper.o\
 rate_sum_prt.o tty_disp.o tty_kill.o tty_enable.o tty_disable.o update_pwd.o\
 pid_disp.o pid_kill.o query_item.o add_item.o del_item.o update_item.o\
 query_vou.o add_vouch.o del_vou.o update_vou.o set_work_d.o write_sum.o\
 check_sum.o end_br.o hl_dev.o rbk.o
	
main: $(OBJS)
	$(CC) $(LIB) $(LDFLAGS) -o $@ $(OBJS)
	
#%.o: %.c

