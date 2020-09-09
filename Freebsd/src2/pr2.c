
/******************************************************************
   程序名:pr50_232.c                  
   功  能:测试PR50在IBM方式下的打印清况
   编  译: cc pr50_232.c  -o p 
*******************************************************************/
#include <stdio.h>
#include <signal.h>   
#include <ctype.h>
main()
{
 int i;
 int line;
/**ESC[5i****/
printf("\x1b\x5b\x35\x69");
 
 /**
  前进line行
  newline(line);
 **/
 line=10;
 for(i=1;i<=line;i++)
  printf("\n");

 /**
  设定中文方式
 **/
 printf("\x1c\x26");

 printf("\n\n           PR50 IBM方式测试程序\n");
 printf("\n\nPR50 IBM方式测试程序(需先将PR50设为IBM方式)\n");
/**
        1b5701          设置水平扩展打印方式
**/
 printf("\x1b\x57\x01");
 printf("\n\nPR50 IBM方式测试程序1111111\n");
/**
      1b5702          取消水平扩展打印方式
**/
 printf("\x1b\x57\x02");

 printf("\n\nPR50 IBM方式测试程序结束\n");

 line=5;
 for(i=1;i<=line;i++)
  printf("\n");

 /**
  退    纸 backpage()
 **/
 printf("\x0c");
 sleep(2);
 printf("\x1b\x5b\x34\x69");
 return(0);
}
