#include "myconfig.h"
#include    <stdio.h>
#include    <string.h>



/*************数据头尾及校验*****************/
u8 CheckHE(void)
{
	u16 i;
	u8 tmp;
	u8 Re;
	if((NumOfQue(&RxQUE3) >= 11))
	{
		AllOutQue(&RxQUE3,RxTmp);
	}
	else if((Rx2len >= 15))
	{
		for(i=0;i<RxTmp2[7];i++)
		{
			RxTmp[i] = RxTmp2[6+i];
		}
	}
	else
	{
		Re = 0;;
	}
	if(RxTmp[0] == 0xA9)
	{
		tmp = RxTmp[1];
	}
	if(RxTmp[tmp-1] == 0x16)
	{
		return 1;
	}
	else
	{
		InitQue(&RxQUE3);
		memset(RxTmp,0x00,sizeof(RxTmp));
		memset(RxTmp2,0x00,sizeof(RxTmp2));
		Rx2len = 0;		
		return Re;		
	}
}

/*************数据求和校验*****************/
u8 CheckSum(void)
{
	u8 i,sum = 0;
	
	if(1 == CheckHE())
	{
		for(i=0;i<(RxTmp[1]-2);i++)
		{
			sum += RxTmp[i];
		}
	}
	if(sum == RxTmp[RxTmp[1]-2])
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
	
	
void Set_Sys(void)
{
	u32 BaudRate;
	u16 Parity;
	u8 temp[4];
	u8 i;
	/*0xA9 L    C    AFN  MODE BADUE PAR HB   TIME (codeX4)            CS   0x16*/
	/*0xA9 0x0F 0x02 0x02 0x02 0x06 0x02 0x05 0x10 0x12 0x34 0x56 0x78 0xDA 0x16*/
	if(1 == CheckSum())
	{
		sysread = 1;
		if((RxTmp[2]&0x01) == 0x01)
		{
			PowerDown();
		}
		W_Mode = RxTmp[4];
		W_BaudRate = RxTmp[5];
		switch(W_BaudRate)
		{
			case(0):
			{
				BaudRate = CBR_1200;
				break;
			}				
			case(1):
			{
				BaudRate = CBR_2400;
				break;
			}
			case(3):
			{
				BaudRate = CBR_9600;
				break;
			}
			case(6):
			{
				BaudRate = CBR_38400;
				break;
			}			
			default:
				break;		
		}
		W_Parity = RxTmp[6];
		switch(W_Parity)
		{
			case(0):
			{
				Parity = EVENPARITY;
				break;
			}				
			case(1):
			{
				Parity = ODDPARITY;
				break;
			}
			case(2):
			{
				Parity = NOPARITY;
				break;
			}			
			default:
				break;		
		}		
		HB_Time = RxTmp[7];		
		W_Time = RxTmp[8];		
		time_sleep = 0;
		if((RxTmp[1]>11)&&((RxTmp[2]&0x02) == 0x02)&&(W_Mode == BLMODE))
		{
			for(i=0;i<4;i++)
			{
				temp[i] = RxTmp[9+i];
			}			
		}		
		switch(W_Mode)
		{
			case(IRDAMODE):
			{
				SetUartState(iRDA, BaudRate,Parity);
				break;
			}				
			case(UARTMODE):
			{
				SetUartState(UART, BaudRate,Parity);
				break;
			}
			case(BLMODE):
			{
				SetUartState(BL, CBR_38400,NOPARITY);
				Set_BL(temp);
				SetUartState(BL, CBR_9600,NOPARITY);				
				break;
			}			
			default:
				break;			
		}
	memset(RxTmp,0x00,sizeof(RxTmp));
	memset(RxTmp2,0x00,sizeof(RxTmp2));
	Rx2len = 0;
	}
}

u8 Set_BL(u8 *pBuffer)
{
  u8 buffer[50]="AT+NAME=BOOST12345678\r\n";  
  u8 length,timeout = 0;
	Set_BL_Enter();
  memset(buffer,0x00,sizeof(buffer));
  sprintf((char *)buffer,"AT+NAME=BOOST%d%d%d%d%d%d%d%d\r\n",(pBuffer[0]>>4)&0x0f,pBuffer[0]&0x0f,(pBuffer[1]>>4)&0x0f,pBuffer[1]&0x0f,(pBuffer[2]>>4)&0x0f,pBuffer[2]&0x0f,(pBuffer[3]>>4)&0x0f,pBuffer[3]&0x0f);
  length = strlen((char *)buffer);
  do
  {  
    timeout++;
		BLsend1(buffer,length);
		Delay(500);
		AllOutQue(&RxQUE3,buffer);
    if(strstr((char *)buffer,"OK")>0)
    {
      break;
    }
  }while(timeout<50);	
	Set_BL_Exit();
  if( timeout>=50 )
  {
    return FALSE;
  }
  return TRUE;	
}


void Send_req(u8 com)
{
	u8 req[] = {0xA9,0x06,0x80,0x02,0x00,0x16};
	u8 i;
	InitQue(&RxQUE3);
	if(com == BL)
	{
		for(i=0;i<5;i++)
		{
			if(i<4) {req[4] += req[i];}
		}
		BLsend1(req,0x06);
	}
	if(com == iRDA)
	{
		for(i=0;i<6;i++)
		{
			if(i<5) {req[4] += req[i];}
			InsertQue(&RxQUE3,req[i]);
		}
		Com3first = COMTXON;
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//		USART2send(&RxQUE3);
	}		
}

void Send_HB(void)
{
	/********** 0xA9 L    C    AFN  HB   MODE BADUE PAR TIME TIMEH TIMEL POWL POWH VER  CS 0x16*/
	u8 HB[16] = {0xA9,0x10,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x16};
	u8 i;
	
	HB[0] = 0xA9;
	HB[1] = 0x10;
	HB[2] = 0x80;
	HB[3] = 0x01;
	HB[4] = HB_Time;
	HB[5] = W_Mode;
	HB[6] = W_BaudRate;
	HB[7] = W_Parity;
	HB[8] = W_Time;
//	W_TimeRe = W_Time;
//	
//	W_TimeRe = GetTime();
	HB[10] = W_TimeRe&0xff;
	HB[9] = (W_TimeRe>>8)&0xff;
	
	W_Bat = ADC_Getvalue();
	HB[11] = W_Bat&0xff;
	HB[12] = (W_Bat >>8)&0xff;
	HB[13] = VERL;//Com3con;
	HB[14] = 0;
	for(i=0;i<14;i++)
	{
		HB[14] += HB[i];
	}	
	HB[15] =0x16;
	BLsend1(HB,0x10);
}
u16 GetTime(void)
{
	u16 temp;
	temp = W_Time;
	temp = temp*60;
	temp -= time_sleep;
	return temp;
}
void PowerDown(void)
{
	PWR_OT_OFF();
	PWR_IR_OFF();
	PWR_BL_OFF();
	RUN_OFF();
	BKP_WriteBackupRegister(BKP_DR5,IWDG_SLEEP);
	PWR_EnterSTANDBYMode();
}

void PowerUp(void)
{
	PWR_BL_ON();
	GPIO_BLRST_OFF();
	GPIO_BLSET_OFF();	
}

void Set_BL_Enter(void)
{
	PWR_BL_OFF();
	GPIO_BLSET_ON();
	Delay(500);
	PWR_BL_ON();
}
void Set_BL_Exit(void)
{
	PWR_BL_OFF();
	GPIO_BLSET_OFF();
	Delay(500);
	PWR_BL_ON();	
}
void Req_Irda(void)
{
	u16 timeout = 0;
	while(LED2_0)
	{
		timeout++;
		Send_req(iRDA);
		Delay(1000);
		Delay(1000);
		Delay(1000);		
		Set_Sys();
		if(timeout >= (W_Time*60))
		{
			PowerDown();
		}		
	}	
}
void Req_BL(void)
{
	u16 timeout = 0;
	u16 timeout1 =0;
	while((LED2_0)&&(timeout1++ < (W_Time*60*2*10)))
	{
		Delay(100);
	}
	do
	{
		timeout++;
		Send_req(BL);
		Delay(1000);
		Delay(1000);	
		Set_Sys();			
		if(timeout>=(W_Time*60))
		{
			PowerDown();
		}			
	}while((LED2_1)&&(sysread == 0));
	if(timeout1 >= (W_Time*60*2*10))
	{
		PowerDown();
	}
}
	
u8 Check_BLlink(void)
{
	u8 Re;
	u8 i = 0;
	if(LED2_1)
	{
		Re = BLINKOK;
	}
	if(LED2_0)
	{
		while(LED1_0)
		{
			i++;
			Delay(100);
			if(i>0x20)
			{
				break;
			}
		}
		if(i<0x02)
		{
			Re = BLINKWAIT;
		}
		else if(i<0x0A)
		{
			Re = BLINKMATCH;
		}
		else
		{
			Re = BLINKOFF;
		}
	}	
	return Re;
}
