#include "myconfig.h"

void delay_nms(uint16_t time1)  //延时函数
{ 
	uint16_t i,j;
	for(i=0;i<time1;i++)
  		for(j=0;j<3000;j++);
}

void delay_nus(uint16_t time2)  //延时函数
{ 
	uint16_t i,j;
	for(i=0;i<time2;i++)
  		for(j=0;j<3;j++);
}


































