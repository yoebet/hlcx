
/******************************************************************
   ������:pr50_232.c                  
   ��  ��:����PR50��IBM��ʽ�µĴ�ӡ���
   ��  ��: cc pr50_232.c  -o p 
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
  ǰ��line��
  newline(line);
 **/
 line=10;
 for(i=1;i<=line;i++)
  printf("\n");

 /**
  �趨���ķ�ʽ
 **/
 printf("\x1c\x26");

 printf("\n\n           PR50 IBM��ʽ���Գ���\n");
 printf("\n\nPR50 IBM��ʽ���Գ���(���Ƚ�PR50��ΪIBM��ʽ)\n");
/**
        1b5701          ����ˮƽ��չ��ӡ��ʽ
**/
 printf("\x1b\x57\x01");
 printf("\n\nPR50 IBM��ʽ���Գ���1111111\n");
/**
      1b5702          ȡ��ˮƽ��չ��ӡ��ʽ
**/
 printf("\x1b\x57\x02");

 printf("\n\nPR50 IBM��ʽ���Գ������\n");

 line=5;
 for(i=1;i<=line;i++)
  printf("\n");

 /**
  ��    ֽ backpage()
 **/
 printf("\x0c");
 sleep(2);
 printf("\x1b\x5b\x34\x69");
 return(0);
}
