#include "myconfig.h"
#include "ESAM.h"
#include "stm32f10x_flash.h"
#include    <stdio.h>
#include    <string.h>

#define FRMAEMINLEN 	0x05
#define FRMAEHEAD0		0xA9
#define FRMAEHEAD1		0xA8
#define FRMAEHEAD2		0xA7
#define FRMAEEND			0x16

#define MAXREPALY 	240

#define AFN0	0x00
#define AFN1	0x01
#define AFN2	0x02
#define AFN3	0x03

#define	BTPORT 			0x00
#define	IRDAPORT 		0x01
#define	RS485PORT 	0x02
#define	ESAMPORT 		0x03
#define	IRDAREPORT 	0x04
#define	LEFTPOWER 	0x05
#define	WAERVER		 	0x06
#define	RZCONTER	 	0x07

#define FN0 00	//系统本身
#define FN1 01	//Irda通道
#define FN2 02	//RS485通道
#define FN3 03	//获取ESAM随机数及密文
#define FN4 04	//认证后抄表
#define FN5 05	//认证测试
#define FN6 06	//IAP升级
#define FN7 07	//设置蓝牙模块名称



volatile FLASH_Status FLASHStatus;

void do_contrl_pro(void)
{
}
/*************数据头尾及校验*****************/
u8 CheckHE(void)
{
	u8 Re = 0;
	u16 i = 0;
	u16 j = 0;
	u8 tempsum = 0;
	u8 templen;
	if(RxCounter3<FRMAEMINLEN)			//数据未达到最小长度
		return 0;
	while((RxBuffer3[i] != FRMAEHEAD0)||(RxBuffer3[i] != FRMAEHEAD1)||(RxBuffer3[i] != FRMAEHEAD2))
	{
		i++;
		if(i>MAXBUFFER-10)
		{
			Clear_RxBuffer3();
		}
	}
	Sys_run.BTuart_Time_Enable = 1;	
	Sys_run.BTuart_Out_Run_Time = 0;
	templen = RxBuffer3[i+1];
	if(templen+i > RxCounter3)		//数据未接收完全
		return 0;
	if(RxBuffer3[i+templen-1] == FRMAEEND)
	{
		tempsum = RxBuffer3[i+templen-2];
		if(tempsum == CheckSum((RxBuffer3+i),(templen-2)))
		{
			Sys_run.BTuart_Time_Enable = 0;		
			Sys_run.BTuart_Out_Run_Time = 0;
			Sys_run.Sleep_run_Time = 0;
			Sys_D.CTL = RxBuffer3[i];
			Sys_D.DATA_LEN = templen - 4;
			for(j=0;j<Sys_D.DATA_LEN;j++)
			{
				Sys_D.DATA[j] = RxBuffer3[i+2+j];
			}	
			Re= 1;
		}		
	}
	return Re;
}

/*************数据求和校验*****************/
u8 CheckSum(u8 *buffer, u8 len)
{
	u8 i;
	u8 Re = 0;
	for(i=0;i<len;i++)
	{
		Re +=buffer[i];
	}
	return Re;
}


void Return_BT(void)
{

}
void AF1_F0_Proc(void)
{
}
void AF1_F1_Proc(void)
{
}
void AF1_F2_Proc(void)
{
}
void AF1_F3_Proc(void)
{
}
void AF1_F4_Proc(void)
{
}

void AF1_F5_Proc(void)
{
}
void AF1_F7_Proc(void)
{
}

void AF1_Lib_Proc(void)
{
}
void AF2_Lib_Proc(void)
{
}

void EXT_CTRL(void)
{
	switch(Sys_D.DATA[0])
	{
		case 0x00:
			PWR_EXT_OFF();
			break;
		case 0x01:
			PWR_EXT_ON();	
			break;
			default:
				break;			
	}
}
void BT_Analysis(void)
{
	if(1 == CheckHE())
	{
		switch(Sys_D.CTL)
		{
			case FRMAEHEAD0:
				SendOneByte_56K(0xff,Sys_D.DATA[0]);
				break;
			case FRMAEHEAD1:
				SendOneByte_38K(0xff,Sys_D.DATA[0]);				
				break;
			case FRMAEHEAD2:
				EXT_CTRL();
				break;
			default:
				break;
		}
	}
}

void Time_Comp(void)
{
	if(Sys_run.Sleep_run_Time_BatLow >(Sys_config.SleepTime_BatLow*60*100))
	{
		PowerDown();		
	}
	if(Sys_run.Sleep_run_Time > (Sys_config.SleepTime*60*100))
	{
		PowerDown();
	}
	if((Sys_run.Out_run_Time_IRDA >(Sys_config.OutTime_IRDA*100))&&(Ver_flag != 1))
	{
		Set_IRDA_power_OFF();
	}
	if(Sys_run.Out_run_Time_RS485 >(Sys_config.OutTime_RS485*100))
	{
		Set_RS485_power_OFF();
	}
	if(Sys_run.Check_Bat_Time > CHECKBATISLOW*60*100)
	{
//		Sys_run.Check_Bat_Time = 0;
//		ADC_filter();
	}
	if((Sys_run.BTuart_Time_Enable == 1)&&(Sys_run.BTuart_Out_Run_Time > MAXBTUARTOUT /*Sys_config.OutTime_BTUART*/))
	{
		Sys_run.BTuart_Time_Enable = 0;
		Sys_run.BTuart_Out_Run_Time = 0;
		Clear_RxBuffer3();
	}
	if((Sys_run.ESAM_Time_Enable == 1)&&(Sys_run.ESAM_Out_Time > MAXESAMOUT*100))
	{
		Sys_run.ESAM_Time_Enable = 0;
		Sys_run.ESAM_Out_Time = 0;
		Set_ESAM_power_OFF();	
	}
}


void PowerDown(void)
{
	RUN_OFF();//关LED	
	PWM_Disable();
	PWR_IR_OFF();
	PWR_BL_OFF();
	PWR_EXT_OFF();
//	PWR_485_OFF();
//	PWR_ESAM_OFF();

	BKP_WriteBackupRegister(BKP_DR5,IWDG_SLEEP);
	PWR_EnterSTANDBYMode();
}

void PowerUp(void)
{
	GPIO_BLRST_OFF();//复位模块
	GPIO_BLSET_OFF();//KEY置低 配对
	PWR_BL_ON();
}

void PowerOff(void)
{
	PWR_BL_OFF();
	Set_IRDA_power_OFF();
	Set_RS485_power_OFF();
	Set_ESAM_power_OFF();
}

void Set_BL_Enter(void)
{
	PWR_BL_OFF();
	GPIO_BLSET_ON();
	delay_nms(50);
	PWR_BL_ON();
}
void Set_BL_Exit(void)
{
	GPIO_BLSET_OFF();
	PWR_BL_OFF();
}

void SendOneByte(u8 Byte)
{
	
}
void SendOneByte_56K(u8 addr,u8 Byte)
{
	u8 tmp;
	if(POW_IR != POW_IR_56K)
	{
		Set_IRDA_power_ON_56K();
	}
	TX_FLAG = 1;
	Tx_bit = 0;
	while((CountTX <= RC199_PREAMBLE_T*2)&&(Tx_bit == 0))//发送起始位
	{
		if(CountTX < RC199_PREAMBLE_T)
			{
				TXD_high();  
			}
			else if((CountTX >= RC199_PREAMBLE_T)&&(CountTX < RC199_PREAMBLE_T*2))
			{
				TXD_low(); 				
			}
			else
			{
				CountTX = 0;				
				Tx_bit = 1;				
			}
	}	
	while((CountTX <= RC199_LOGIC_1_ALL)&&(Tx_bit > 0)&&(Tx_bit < 5))	//4位地址
	{
		tmp	= (addr >> (Tx_bit - 1))& 0x01;		
		if((CountTX < RC199_LOGIC_1_H	))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < RC199_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < RC199_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= RC199_LOGIC_1_ALL)&&(Tx_bit > 4)&&(Tx_bit < 13))//8bit数据
	{
		tmp	= (Byte >> (Tx_bit - 5))& 0x01;
		if((CountTX < RC199_LOGIC_1_H))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < RC199_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < RC199_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= RC199_LOGIC_1_ALL)&&(Tx_bit > 12)&&(Tx_bit < 17))//地址反码
	{
		tmp	= (~addr >> (Tx_bit - 1))& 0x01;		
		if((CountTX < RC199_LOGIC_1_H	))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < RC199_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < RC199_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= RC199_LOGIC_1_ALL)&&(Tx_bit > 16)&&(Tx_bit < 25))//8bit数据反码
	{
		tmp	= (~BYTE >> (Tx_bit - 15))& 0x01;
		if((CountTX <= RC199_LOGIC_1_H))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX <= RC199_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX <= RC199_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}	
}

void SendOneByte_38K(u8 addr,u8 Byte)
{
	u8 tmp;
	if(POW_IR != POW_IR_56K)
	{
		Set_IRDA_power_ON_56K();
	}
	TX_FLAG = 1;	
	Tx_bit = 0;
	while((CountTX <= (NEC_PREAMBLE_T_H+NEC_PREAMBLE_T_L))&&(Tx_bit == 0))//发送起始位
	{
		if(CountTX < NEC_PREAMBLE_T_H)
			{
				TXD_high();  
			}
			else if((CountTX >= NEC_PREAMBLE_T_H)&&(CountTX < (NEC_PREAMBLE_T_H+NEC_PREAMBLE_T_L)))
			{
				TXD_low(); 				
			}
			else
			{
				CountTX = 0;				
				Tx_bit = 1;				
			}
	}	
	while((CountTX <= NEC_LOGIC_1_ALL)&&(Tx_bit > 0)&&(Tx_bit < 5))	//4位地址
	{
		tmp	= (addr >> (Tx_bit - 1))& 0x01;		
		if((CountTX < NEC_LOGIC_1_H	))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < NEC_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < NEC_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= NEC_LOGIC_1_ALL)&&(Tx_bit > 4)&&(Tx_bit < 9))	//4位地址反码
	{
		tmp	= (~addr >> (Tx_bit - 5))& 0x01;		
		if((CountTX < NEC_LOGIC_1_H	))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < NEC_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < NEC_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= NEC_LOGIC_1_ALL)&&(Tx_bit > 8)&&(Tx_bit < 17))//8bit数据
	{
		tmp	= (Byte >> (Tx_bit - 8))& 0x01;
		if((CountTX < NEC_LOGIC_1_H))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX < NEC_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX < NEC_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}
	while((CountTX <= NEC_LOGIC_1_ALL)&&(Tx_bit > 16)&&(Tx_bit < 25))//8bit数据反码
	{
		tmp	= (~BYTE >> (Tx_bit - 15))& 0x01;
		if((CountTX <= NEC_LOGIC_1_H))
		{
			TXD_high(); 
		}
		else if((tmp == 0)&&(CountTX <= NEC_LOGIC_0_ALL))
		{
			TXD_low(); 
		}
		else if((tmp == 1)&&(CountTX <= NEC_LOGIC_1_ALL))
		{
			TXD_low();
		}
		else
		{
			CountTX = 0;
			Tx_bit++;							
		}
	}	
	
}

void SendBytes(u8 *str,u8 len)
{
  u16 i;
  for(i=0;i<len;i++)
  {
//  	 SendOneByte(str[i]);
  }
}



void Clear_RxBuffer2(void)
{
	u16 i;
	RxCounter2 = 0;
	for(i=0;i<MAXBUFFER;i++)
	{
		RxBuffer2[i] = 0x00;
	}
}

void Clear_RxBuffer3(void)
{
	u16 i;
	RxCounter3 = 0;
	for(i=0;i<MAXBUFFER;i++)
	{
		RxBuffer3[i] = 0x00;
	}
}


void ESAM_Info(void)
{
	Set_ESAM_power_ON();//ESAM复位
	
	RZ_Counter = ReadCounter();
	
	Set_ESAM_power_OFF();
}

u8 FlashWrite(u32 Data)
{
		u32 Address = SaveDataStartAddr;

		FLASHStatus = FLASH_ProgramWord(Address, Data);
		
		if (FLASHStatus == FLASH_COMPLETE)
		{
//				Address = Address + 4;
		return 1;	 
		}
		else
		{
		return 0;
		}
}


void Wait_BTlink(void)
{
	u16 timeout1 =0;
	
	while((LED2_0)&&(timeout1++ < MAXTIMEWAITBTLINK*600))//等待配对时间 约5min
	{
		IWDG_ReloadCounter();
		delay_nms(100);
	}	
	if(timeout1 >= MAXTIMEWAITBTLINK*600)
	{
		timeout1 = 0;
		PowerDown();
	}
	else
	{
		BT_STA = LEDSTA_BTLINKON;
	}
}

void BTSetName(u8 *Name)
{
	char tempbuf[]={"AT+NAME=BOOST00000000\r\n"};
	u8 timeout = 0;
	
	sprintf(tempbuf,"AT+NAME=BOOST%x%x%x%x%x%x%x%x\r\n",
				(Name[0]&0xf0)>>4,Name[0]&0x0f,(Name[1]&0xf0)>>4,Name[1]&0x0f,
				(Name[2]&0xf0)>>4,Name[2]&0x0f,(Name[3]&0xf0)>>4,Name[3]&0x0f);
	SetUartState(COM3,CBR_38400,USART_Parity_No);//BL 38400 1停 无
	Set_BL_Enter();//进入AT模式
	do
	{
		IWDG_ReloadCounter();
		timeout++;
		Clear_RxBuffer3();
		delay_nms(1000);
		USART3send((u8 *)tempbuf,sizeof(tempbuf)-1);
		delay_nms(100);
		if( strstr((char *)RxBuffer3,"OK")>0 )
		{
			break;
		}			
	}while(timeout<5);
	NVIC_SystemReset();
}


/*********************BLSET***************************/	
void BTSet(void)
{
	u8 BLset_command[] = {"AT+UART=115200,0,2\r\n"};
	u8 timeout = 0;
	if(0 == LoadFlash())//未设置BL
	{
		SetUartState(COM3,CBR_38400,USART_Parity_No);//BL 38400 1停 无
		Set_BL_Enter();//进入AT模式
		do
		{
			IWDG_ReloadCounter();
			timeout++;
			Clear_RxBuffer3();
			delay_nms(1000);
			USART3send(BLset_command,sizeof(BLset_command)-1);//设置115200 1停 偶
			delay_nms(100);
			if( strstr((char *)RxBuffer3,"OK")>0 )
			{
				break;
			}			
		}while(timeout<5);
		if(timeout<5)
		{
			WriteFLAG();//设置完成		
		}
	NVIC_SystemReset();
  }
}

void WriteFLAG(void)
{
	FLASH_Unlock();	   /* Unlock the Flash Program Erase controller */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);		/* Clear All pending flags */
	FLASH_ErasePage(SaveDataStartAddr);//擦除1k    
  delay_nms(10);           
	FlashWrite(0x11011011);						 
}

u8 LoadFlash(void)
{
	uint32_t FLASH_READ;
	
	FLASH_READ = (*(volatile uint32_t*) SaveDataStartAddr);
	
	if(FLASH_READ == 0x11011011)//已经成功设置BL
	{
		return 1;
	}
	else return 0;
}

