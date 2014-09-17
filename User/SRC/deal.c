#include "myconfig.h"
#include "ESAM.h"
#include "stm32f10x_flash.h"
#include    <stdio.h>
#include    <string.h>

#define FRMAEMINLEN 0x05
#define FRMAEHEAD		0xA9
#define FRMAEEND		0x16

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

volatile FLASH_Status FLASHStatus;

void do_contrl_pro(void)
{
	if((Dat_dbuf.Contrl&0x01) == 0x01)
	{
		PowerDown();
	}
//	if((Dat_dbuf.Contrl&0x80) == 0x80)
//	{
//		
//	}
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
	while(RxBuffer3[i] != FRMAEHEAD)
	{
		i++;
	}
	templen = RxBuffer3[i+1];
	if(templen+i > RxCounter3)		//数据未接收完全
		return 0;
	if(RxBuffer3[i+templen-1] == FRMAEEND)
	{
		tempsum = RxBuffer3[i+templen-2];
		if(tempsum == CheckSum((RxBuffer3+i),(templen-2)))
		{
			Sys_run.Sleep_run_Time = 0;
			Dat_dbuf.Contrl = RxBuffer3[i+2];
			do_contrl_pro();
			Dat_dbuf.AFN = RxBuffer3[i+3];
			switch(Dat_dbuf.AFN)
			{
				case AFN0:
					Dat_dbuf.AFN0dbuf.Port = RxBuffer3[i+4];
					Dat_dbuf.AFN0dbuf.BaudRate = RxBuffer3[i+5];
					Dat_dbuf.AFN0dbuf.Parity = RxBuffer3[i+6];
					Dat_dbuf.AFN0dbuf.SleepTime = RxBuffer3[i+7];
					break;
				case AFN1:
					Dat_dbuf.AFN1dbuf.Port = RxBuffer3[i+4];
					Dat_dbuf.AFN1dbuf.OutTime_Thisport = RxBuffer3[i+5];
					Dat_dbuf.AFN1dbuf.TranLen= RxBuffer3[i+6];
					for(j=i+7;j<templen-2;j++)
					{
						Dat_dbuf.AFN1dbuf.TranDat[j-i-7] = RxBuffer3[j];				
					}
					break;
				case AFN2:
					Dat_dbuf.AFN2dbuf.Port = RxBuffer3[i+4];
					Dat_dbuf.AFN2dbuf.ReqLen = RxBuffer3[i+5];
					if(Dat_dbuf.AFN2dbuf.ReqLen > MAXREPALY)
					{
						Dat_dbuf.AFN2dbuf.ReqLen = MAXREPALY;
					}
					break;
				case AFN3:
					Dat_dbuf.AFN3dbuf.Port = RxBuffer3[i+4];
					Dat_dbuf.AFN3dbuf.NeedRZ = RxBuffer3[i+5];
					Dat_dbuf.AFN3dbuf.OutTime_IRDA = RxBuffer3[i+6];
					for(j=0;j<6;j++)
					{
						Dat_dbuf.AFN3dbuf.DBAddr[j] = RxBuffer3[i+7+j];
					}
					Dat_dbuf.AFN3dbuf.TransLen = RxBuffer3[i+13];
					for(j=0;j<Dat_dbuf.AFN3dbuf.TransLen;j++)
					{
						Dat_dbuf.AFN3dbuf.TransData[j] = RxBuffer3[i+14+j];
					}
					break;
				default:
					break;
			}
			Dat_dbuf.Dataisready = 1;
			Re = 1;
		}
		else
		{
			return 0;
		}		
		Clear_RxBuffer3();
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
	u8 tempdata[0xff];
	u8 i;
	switch(ReturnFeame.Data_uBuf.AFN)
	{		
		case AFN0:			
			tempdata[0] = FRMAEHEAD;
			tempdata[1] = ReturnFeame.len+5;
			tempdata[2] = ReturnFeame.contrl;
			tempdata[3] = ReturnFeame.Data_uBuf.AFN;
			tempdata[4] = ReturnFeame.Data_uBuf.AFN0ubuf.Port;
			tempdata[5] = ReturnFeame.Data_uBuf.AFN0ubuf.End_rep;
			tempdata[6] = 0;
			for(i=0;i<6;i++)
			{
				tempdata[6] += tempdata[i];
			}
			tempdata[7] = FRMAEEND;
			USART3send(tempdata,8);
			break;
		case AFN1:
			tempdata[0] = FRMAEHEAD;
			tempdata[1] = ReturnFeame.len+5;
			tempdata[2] = ReturnFeame.contrl;
			tempdata[3] = ReturnFeame.Data_uBuf.AFN;
			tempdata[4] = ReturnFeame.Data_uBuf.AFN1ubuf.Port;
			tempdata[5] = ReturnFeame.Data_uBuf.AFN1ubuf.TranRep;
			tempdata[6] = 0;
			for(i=0;i<6;i++)
			{
				tempdata[6] += tempdata[i];
			}
			tempdata[7] = FRMAEEND;
			USART3send(tempdata,8);
			break;
		case AFN2:
			tempdata[0] = FRMAEHEAD;
			tempdata[1] = ReturnFeame.len+5;
			tempdata[2] = ReturnFeame.contrl;
			tempdata[3] = ReturnFeame.Data_uBuf.AFN;
			tempdata[4] = ReturnFeame.Data_uBuf.AFN2ubuf.Port;
			tempdata[5] = ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen;
			for(i=0;i<ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen;i++)
			{
				tempdata[6+i] = ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[i];
			}
			tempdata[6+ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen] = 0;
			for(i=0;i<6+ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen;i++)
			{
				tempdata[6+ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen] += tempdata[i];
			}
			tempdata[7+ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen] = FRMAEEND;
			USART3send(tempdata,8+ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);
			break;
		case AFN3:
			
			break;		
		default:
			break;
	}
	ReturnFeame.Data_uBuf.AFN = 0xFF;
}
void AF0_Lib_Proc(void)
{
	u32 BaudRate;
	u16 Parity;
	Sys_config.SleepTime = Dat_dbuf.AFN0dbuf.SleepTime;
	Sys_config.Sys_ready = SYSISREADY;
	switch(Dat_dbuf.AFN0dbuf.BaudRate)
		{
			case(0):
			BaudRate = CBR_1200;
			break;
			case(1):
			BaudRate = CBR_2400;
			break;
			case(3):
			BaudRate = CBR_9600;
			break;
			case(6):
			BaudRate = CBR_38400;
			break;
			default:
			break;		
		}
	switch(Dat_dbuf.AFN0dbuf.Parity)
	{
		case(0):
		Parity = EVENPARITY;
		break;
		case(1):
		Parity = ODDPARITY;
		break;
		case(2):
		Parity = NOPARITY;
		break;
		default:
		break;		
	}	
	switch(Dat_dbuf.AFN0dbuf.Port)
	{
		case IRDAPORT:
				switch(BaudRate)
				{
					case(CBR_1200):
						IR_BaudRate_Time = 83;
						break;
					case(CBR_2400):
						IR_BaudRate_Time = 42;
						break;
					default:
						IR_BaudRate_Time = 83;
						break;	
				}
				Sys_config.IrDAisconfed = 1;
			break;
		case RS485PORT:
			SetUartState(RS485, BaudRate, Parity);
			Sys_config.RS485isconfed = 1;
			break;
		default:
			break;
	}
	ReturnFeame.head = FRMAEHEAD;
	ReturnFeame.len = 0x03;
	ReturnFeame.contrl = 0x80;
	ReturnFeame.End = FRMAEEND;
	ReturnFeame.Data_uBuf.AFN = Dat_dbuf.AFN;
	ReturnFeame.Data_uBuf.AFN0ubuf.End_rep = 1;
	ReturnFeame.Data_uBuf.AFN0ubuf.Port = Dat_dbuf.AFN0dbuf.Port;
	
	Return_BT();
}
void AF1_Lib_Proc(void)
{
	ReturnFeame.head = FRMAEHEAD;
	ReturnFeame.len = 0x03;
	ReturnFeame.contrl = 0x80;
	ReturnFeame.End = FRMAEEND;
	ReturnFeame.Data_uBuf.AFN = Dat_dbuf.AFN;
	ReturnFeame.Data_uBuf.AFN1ubuf.Port = Dat_dbuf.AFN1dbuf.Port;
	switch(Dat_dbuf.AFN1dbuf.Port)
	{
		case BTPORT:
			//配置蓝牙模块名称，暂时未实现
			BTSetName(Dat_dbuf.AFN1dbuf.TranDat);
			break;
		case IRDAPORT: 
			if(Sys_config.IrDAisconfed)
			{
				ReturnFeame.Data_uBuf.AFN1ubuf.TranRep = 1;
				Return_BT();
				Sys_config.OutTime_IRDA = Dat_dbuf.AFN1dbuf.OutTime_Thisport;
				InitQue(&IrDARxQUE);
				SendBytes(Dat_dbuf.AFN1dbuf.TranDat, Dat_dbuf.AFN1dbuf.TranLen);
				Sys_run.Out_run_Time_IRDA = 0;
			}
			else
			{
				ReturnFeame.Data_uBuf.AFN1ubuf.TranRep = 0;
				Return_BT();
			}
			break;
		case RS485PORT:
			if(Sys_config.RS485isconfed)
			{
				ReturnFeame.Data_uBuf.AFN1ubuf.TranRep = 1;
				Return_BT();
				Sys_config.OutTime_RS485 = Dat_dbuf.AFN1dbuf.OutTime_Thisport;
				InitQue(&RS485RxQUE);
				USART1send(Dat_dbuf.AFN1dbuf.TranDat, Dat_dbuf.AFN1dbuf.TranLen);
				Sys_run.Out_run_Time_RS485 = 0;
			}
			else
			{
				ReturnFeame.Data_uBuf.AFN1ubuf.TranRep = 0;
				Return_BT();
			}
			break;
		case ESAMPORT:
			break;
		default:
			break;
	}
}
void AF2_Lib_Proc(void)
{
	u32 temp;
	switch(Dat_dbuf.AFN2dbuf.Port)
	{
		case IRDAPORT: 
			if(Dat_dbuf.AFN2dbuf.ReqLen < NumOfQue(&IrDARxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = Dat_dbuf.AFN2dbuf.ReqLen;
				OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);
			}
			else if(Dat_dbuf.AFN2dbuf.ReqLen >= NumOfQue(&IrDARxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = NumOfQue(&IrDARxQUE);
				OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);				
			}
			break;
		case RS485PORT:
			if(Dat_dbuf.AFN2dbuf.ReqLen < NumOfQue(&RS485RxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = Dat_dbuf.AFN2dbuf.ReqLen;
				OutQue(&RS485RxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);
			}
			else if(Dat_dbuf.AFN2dbuf.ReqLen >= NumOfQue(&RS485RxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = NumOfQue(&RS485RxQUE);
				OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);				
			}			
			break;
		case ESAMPORT:
			break;
		case IRDAREPORT:
			if(Dat_dbuf.AFN2dbuf.ReqLen < NumOfQue(&IrDARxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = Dat_dbuf.AFN2dbuf.ReqLen;
				OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);
			}
			else if(Dat_dbuf.AFN2dbuf.ReqLen >= NumOfQue(&IrDARxQUE))
			{
				ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = NumOfQue(&IrDARxQUE);
				OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat,ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen);				
			}
			break;
		case LEFTPOWER:
			temp = ADC_filter();
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = 0x02;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[0] = (u8)(temp&0xff);
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[1] = (u8)((temp&0xff00)>>8);		
			break;
		case WAERVER:
			GetBuildTime();
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = 0x09;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[0] = XVER;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[1] = SVER;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[2] = BuildTime.yearh;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[3] = BuildTime.yearl;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[4] = BuildTime.mon;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[5] = BuildTime.date;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[6] = BuildTime.hour;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[7] = BuildTime.min;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[8] = BuildTime.sec;
			break;
		case RZCONTER:
			ESAM_Info();
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen = 0x04;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[0] = (RZ_Counter&0xff000000)>>24;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[1] = (RZ_Counter&0xff0000)>>16;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[2] = (RZ_Counter&0xff00)>>8;
			ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealDat[3] = RZ_Counter&0xff;
			break;		
		default:
			break;
	}		
	ReturnFeame.head = FRMAEHEAD;
	ReturnFeame.len = ReturnFeame.Data_uBuf.AFN2ubuf.ReqRealLen+3;
	if(ReturnFeame.Data_uBuf.AFN2ubuf.ReRZ != 0)
	{
		ReturnFeame.contrl = 0x80|((ReturnFeame.Data_uBuf.AFN2ubuf.ReRZ)<<1);	
		ReturnFeame.Data_uBuf.AFN2ubuf.ReRZ = 0;	
	}
	else
	{
		ReturnFeame.contrl = 0x80;
	}
	ReturnFeame.End = FRMAEEND;
	ReturnFeame.Data_uBuf.AFN = Dat_dbuf.AFN;
	ReturnFeame.Data_uBuf.AFN2ubuf.Port = Dat_dbuf.AFN2dbuf.Port;
	
	Return_BT();
}
void AF3_Lib_Proc(void)
{
	u8 i;
	u8 RandEncData[16];
	Set_ESAM_power_ON();
	switch(Dat_dbuf.AFN3dbuf.Port)
	{
		case IRDAPORT: 
			if(Dat_dbuf.AFN3dbuf.NeedRZ)
			{
				Ver_flag = 1;
				ReturnFeame.Data_uBuf.AFN2ubuf.ReRZ = DoVerifica(Dat_dbuf.AFN3dbuf.DBAddr);
				ReturnFeame.Data_uBuf.AFN2ubuf.ReRZ += 1;
				Ver_flag = 0;				
			}
			if(Dat_dbuf.AFN3dbuf.TransLen > 0)
			{
				Sys_config.OutTime_IRDA = Dat_dbuf.AFN3dbuf.OutTime_IRDA;
				InitQue(&IrDARxQUE);
				SendBytes(Dat_dbuf.AFN1dbuf.TranDat, Dat_dbuf.AFN1dbuf.TranLen);
				Sys_run.Out_run_Time_IRDA = 0;		
			}
			break;
		case ESAMPORT:
			Get_ESAM_Data(Dat_dbuf.AFN3dbuf.DBAddr,RandEncData);
			InitQue(&IrDARxQUE);
			for(i=0;i<16;i++)
			{
				InsertQue(&IrDARxQUE,RandEncData[i]);
			}
			break;
		default:
				break;
	}
	Set_ESAM_power_OFF();
}
void BT_Analysis(void)
{
	if(1 == CheckHE())
	{
		switch(Dat_dbuf.AFN)
		{
			case AFN0:
				AF0_Lib_Proc();
				break;
			case AFN1:
				AF1_Lib_Proc();
				break;
			case AFN2:
				AF2_Lib_Proc();
				break;
			case AFN3:
				AF3_Lib_Proc();
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
	if(Sys_run.Out_run_Time_IRDA >(Sys_config.OutTime_IRDA*100))
	{
		Set_IRDA_power_OFF();
	}
	if(Sys_run.Out_run_Time_RS485 >(Sys_config.OutTime_RS485*100))
	{
		Set_RS485_power_OFF();
	}
}


void PowerDown(void)
{
	RUN_OFF();//关LED	
	PWM_Disable();
	PWR_IR_OFF();
	PWR_BL_OFF();
	PWR_485_OFF();
	PWR_ESAM_OFF();

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
	if(POW_IR == 0)
	{
		Set_IRDA_power_ON();
	}

	RX_FLAG = 0;
	CountRX = 0;
	Receive_bit = 0;
	
	TX_FLAG = 1;
	Tx_Parity = 0;
	BYTE = Byte;
	while(TX_FLAG == 1);
	delay_nms(1);
}

void SendBytes(u8 *str,u8 len)
{
  u16 i;
  for(i=0;i<len;i++)
  {
  	 SendOneByte(str[i]);
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
	ESAM_Reset();//ESAM复位
	
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
	
	while((LED2_0)&&(timeout1++ < MAXTIMEWAITBTLINK*600))//等待配对时间 约10min
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
	char tempbuf[30];
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

