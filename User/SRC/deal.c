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

#define FN0 00	//系统本身
#define FN1 01	//Irda通道
#define FN2 02	//RS485通道
#define FN3 03	//获取ESAM随机数及密文
#define FN4 04	//认证后抄表
#define FN5 05	//认证测试
#define FN6 06	//IAP升级


volatile FLASH_Status FLASHStatus;

void do_contrl_pro(void)
{
	if((Dat_dbuf.Contrl&0x01) == 0x01)
	{
		PowerDown();
	}
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
		if(i>MAXBUFFER-10)
		{
			Clear_RxBuffer3();
		}
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
					break;
				case AFN1:
					Dat_dbuf.WriteFN = RxBuffer3[i+4];
					switch(Dat_dbuf.WriteFN)
					{
						case FN0:
							Dat_dbuf.AFN1D.afn1_f0.Port = RxBuffer3[i+5];
							Dat_dbuf.AFN1D.afn1_f0.BaudRate = RxBuffer3[i+6];
							Dat_dbuf.AFN1D.afn1_f0.Parity = RxBuffer3[i+7];
							Dat_dbuf.AFN1D.afn1_f0.SysSleepTime = RxBuffer3[i+8];						
							break;
						case FN1:
						case FN2:
							Dat_dbuf.AFN1D.afn1_f12.OutTime_Thisport = RxBuffer3[i+5];
							Dat_dbuf.AFN1D.afn1_f12.TransLen = RxBuffer3[i+6];
							if(Dat_dbuf.AFN1D.afn1_f12.TransLen > templen)
							{
								Clear_RxBuffer3();
								return 0;
							}
							for(j=0;j<Dat_dbuf.AFN1D.afn1_f12.TransLen;j++)
							{
								Dat_dbuf.AFN1D.afn1_f12.TransData[j] = RxBuffer3[i+7+j];
							}
							break;
						case FN3:
							for(j=0;j<6;j++)
							{
								Dat_dbuf.AFN1D.afn1_f3.DBAddr[j] = RxBuffer3[i+5+j];
							}
							break;
						case FN4:
							Dat_dbuf.AFN1D.afn1_f4.OutTime_Thisport = RxBuffer3[i+5];
							for(j=0;j<6;j++)
							{
								Dat_dbuf.AFN1D.afn1_f4.DBAddr[j] = RxBuffer3[i+6+j];
							}
							Dat_dbuf.AFN1D.afn1_f4.TransLen = RxBuffer3[i+12];
							if(Dat_dbuf.AFN1D.afn1_f4.TransLen > templen)
							{
								Clear_RxBuffer3();
								return 0;
							}							
							for(j=0;j<Dat_dbuf.AFN1D.afn1_f4.TransLen;j++)
							{
								Dat_dbuf.AFN1D.afn1_f4.TransData[j] = RxBuffer3[i+13+j];
							}
							break;
						case FN5:
							for(j=0;j<6;j++)
							{
								Dat_dbuf.AFN1D.afn1_f5.DBAddr[j] = RxBuffer3[i+5+j];
							}
							break;
						case FN6:
							break;
						default:
							break;
				}
					break;
				case AFN2:
					Dat_dbuf.ReadFN = RxBuffer3[i+4];
					Dat_dbuf.AFN2D.ReadLen = RxBuffer3[i+5];
					break;
				case AFN3:
					break;
				default:
					break;
			}
			Re = 1;
		}
		else
		{
			return 0;
		}		
		Clear_RxBuffer3();
	}
	else														//数据帧不正确
	{
		Clear_RxBuffer3();		
		return 0;
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
			tempdata[1] = 8;
			tempdata[2] = ReturnFeame.contrl;		
			tempdata[3] = ReturnFeame.Data_uBuf.AFN;
			tempdata[4] = ReturnFeame.Data_uBuf.Fn;
			tempdata[5] = ReturnFeame.Data_uBuf.afn2_f4.sta;
			tempdata[6] = 0;
			tempdata[6] = CheckSum(tempdata,ReturnFeame.len+3);
			tempdata[7] = FRMAEEND;
			USART3send(tempdata,8);	
			break;		
		case AFN2:
			tempdata[0] = FRMAEHEAD;
			tempdata[1] = ReturnFeame.len+5;
			tempdata[2] = ReturnFeame.contrl;	
			tempdata[3] = ReturnFeame.Data_uBuf.AFN;
			tempdata[4] = ReturnFeame.Data_uBuf.Fn;
			tempdata[5] = ReturnFeame.Data_uBuf.SendLen;
			switch(Dat_dbuf.WriteFN)
			{
				case FN0:
					tempdata[6] = ReturnFeame.Data_uBuf.afn2_f0.flag;
					tempdata[7] = ReturnFeame.Data_uBuf.afn2_f0.leftbat.batperl;
					tempdata[8] = ReturnFeame.Data_uBuf.afn2_f0.leftbat.batperh;
					tempdata[9] = ReturnFeame.Data_uBuf.afn2_f0.ver.Xver;
					tempdata[10] = ReturnFeame.Data_uBuf.afn2_f0.ver.Sver;
					tempdata[11] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.yearh;
					tempdata[12] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.yearl;
					tempdata[13] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.mon;
					tempdata[14] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.date;
					tempdata[15] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.hour;
					tempdata[16] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.min;
					tempdata[17] = ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.sec;
					break;
				case FN1:
					for(i=0;i<ReturnFeame.Data_uBuf.SendLen;i++)
					{
						tempdata[6+i] = ReturnFeame.Data_uBuf.afn2_f1.SendData[i];						
					}
				case FN2:
					for(i=0;i<ReturnFeame.Data_uBuf.SendLen;i++)
					{
						tempdata[6+i] = ReturnFeame.Data_uBuf.afn2_f2.SendData[i];						
					}
					break;
				case FN3:
					for(i=0;i<8;i++)
					{
						tempdata[6+i] = ReturnFeame.Data_uBuf.afn2_f3.SendRand1[i];						
					}
					for(i=0;i<8;i++)
					{
						tempdata[14+i] = ReturnFeame.Data_uBuf.afn2_f3.SendEncD1[i];						
					}						
					break;
				case FN4:
					for(i=0;i<ReturnFeame.Data_uBuf.SendLen;i++)
					{
						tempdata[6+i] = ReturnFeame.Data_uBuf.afn2_f4.SendData[i];						
					}					
					break;
				case FN5:
					tempdata[6] = ReturnFeame.Data_uBuf.afn2_f5.SendStas;
					break;
				case FN6:
					break;
				default:
					break;
			}
			tempdata[ReturnFeame.len+3] = 0;
			tempdata[ReturnFeame.len+3] = CheckSum(tempdata,ReturnFeame.len+3);
			tempdata[ReturnFeame.len+4] = FRMAEEND;									
			USART3send(tempdata,(ReturnFeame.len+5));		
			break;
		default:
			break;
	}	
	ReturnFeame.Data_uBuf.AFN = 0xFF;
}
void AF1_F0_Proc(void)
{
	u32 BaudRate;
	u16 Parity;
	Sys_config.SleepTime = Dat_dbuf.AFN1D.afn1_f0.SysSleepTime;
	Sys_config.Sys_ready = SYSISREADY;
	switch(Dat_dbuf.AFN1D.afn1_f0.BaudRate)
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
	switch(Dat_dbuf.AFN1D.afn1_f0.Parity)
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
	switch(Dat_dbuf.AFN1D.afn1_f0.Port)
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
}
void AF1_F1_Proc(void)
{
	if(Sys_config.IrDAisconfed)
	{
//		Set_IRDA_power_ON();
		Sys_run.Out_run_Time_IRDA = 0;
		Sys_config.OutTime_IRDA = Dat_dbuf.AFN1D.afn1_f12.OutTime_Thisport;
		InitQue(&IrDARxQUE);
		SendBytes(Dat_dbuf.AFN1D.afn1_f12.TransData, Dat_dbuf.AFN1D.afn1_f12.TransLen);
		Sys_run.Out_run_Time_IRDA = 0;
	}
}
void AF1_F2_Proc(void)
{
	if(Sys_config.RS485isconfed)
	{
		Set_RS485_power_ON();
		Sys_run.Out_run_Time_RS485 = 0;
		Sys_config.OutTime_RS485 = Dat_dbuf.AFN1D.afn1_f12.OutTime_Thisport;
		InitQue(&RS485RxQUE);
		USART1send(Dat_dbuf.AFN1D.afn1_f12.TransData, Dat_dbuf.AFN1D.afn1_f12.TransLen);
		Sys_run.Out_run_Time_RS485 = 0;
	}	
}
void AF1_F3_Proc(void)
{
	u8 RandEncData[16];
	u8 i;
	Set_ESAM_power_ON();
	Get_ESAM_Data(Dat_dbuf.AFN1D.afn1_f3.DBAddr,RandEncData);
	for(i=0;i<8;i++)
	{
		ReturnFeame.Data_uBuf.afn2_f3.SendRand1[i] = RandEncData[i];
		ReturnFeame.Data_uBuf.afn2_f3.SendEncD1[i] = RandEncData[8+i];
	}
	Set_ESAM_power_OFF();	
}
void AF1_F4_Proc(void)
{
	u8 Re;
	if(Sys_config.IrDAisconfed)
	{
		Set_ESAM_power_ON();
		Ver_flag = 1;
		Re = DoVerifica(Dat_dbuf.AFN1D.afn1_f4.DBAddr);
		Ver_flag = 0;				
		if((Dat_dbuf.AFN1D.afn1_f4.TransLen > 0)&&((Re == PRIVATESUC)||(Re == PUBLICSUC)))
		{
			Sys_config.OutTime_IRDA = Dat_dbuf.AFN1D.afn1_f4.OutTime_Thisport;
			InitQue(&IrDARxQUE);
			SendBytes(Dat_dbuf.AFN1D.afn1_f4.TransData, Dat_dbuf.AFN1D.afn1_f4.TransLen);
			Sys_run.Out_run_Time_IRDA = 0;		
		}	
		else
		{
			ReturnFeame.Data_uBuf.afn2_f4.AFN = AFN0;
			ReturnFeame.Data_uBuf.afn2_f4.sta = Re - 2;
		}
		Set_ESAM_power_OFF();		
	}
}

void AF1_F5_Proc(void)
{
	u8 Re;
	if(Sys_config.IrDAisconfed)
	{
		Set_ESAM_power_ON();
		Ver_flag = 1;
		Re = DoVerifica(Dat_dbuf.AFN1D.afn1_f4.DBAddr);
		Ver_flag = 0;				
		ReturnFeame.Data_uBuf.afn2_f5.SendStas = Re + 1;		
	}
}

void AF1_Lib_Proc(void)
{
	switch(Dat_dbuf.WriteFN)
	{
		case FN0:
			AF1_F0_Proc();
			break;
		case FN1:
			AF1_F1_Proc();
			break;
		case FN2:
			AF1_F2_Proc();
			break;
		case FN3:
			AF1_F3_Proc();
			break;
		case FN4:
			AF1_F4_Proc();
			break;
		case FN5:
			AF1_F5_Proc();
			break;
		case FN6:
			break;
		default:
			break;
	}
}
void AF2_Lib_Proc(void)
{
	u32 temp;
	ReturnFeame.Data_uBuf.Fn = Dat_dbuf.ReadFN;
	ReturnFeame.Data_uBuf.AFN = Dat_dbuf.AFN;
	if(Dat_dbuf.ReadFN == Dat_dbuf.WriteFN)
	{
		switch(Dat_dbuf.ReadFN)
		{
			case FN0:
				ReturnFeame.Data_uBuf.SendLen = 12;
				if(Sys_config.Sys_ready == SYSISREADY)
				{
					ReturnFeame.Data_uBuf.afn2_f0.flag = 1;					
				}
				else
				{
					ReturnFeame.Data_uBuf.afn2_f0.flag = 0;					
				}
				temp = ADC_filter();
				ReturnFeame.Data_uBuf.afn2_f0.leftbat.batperl = (u8)(temp&0xff);
				ReturnFeame.Data_uBuf.afn2_f0.leftbat.batperh = (u8)((temp&0xff00)>>8);
				GetBuildTime();
				ReturnFeame.Data_uBuf.afn2_f0.ver.Xver = XVER;
				ReturnFeame.Data_uBuf.afn2_f0.ver.Sver = SVER;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.yearh = BuildTime.yearh;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.yearl = BuildTime.yearl;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.mon = BuildTime.mon;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.date = BuildTime.date;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.hour = BuildTime.hour;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.min = BuildTime.min;
				ReturnFeame.Data_uBuf.afn2_f0.ver.CompilTime.sec = BuildTime.sec;			
				break;
			case FN1:
				if(Dat_dbuf.AFN2D.ReadLen < NumOfQue(&IrDARxQUE))
				{
					ReturnFeame.Data_uBuf.SendLen = Dat_dbuf.AFN2D.ReadLen;					
					OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.afn2_f1.SendData,ReturnFeame.Data_uBuf.SendLen);
				}
				else
				{
					ReturnFeame.Data_uBuf.SendLen = NumOfQue(&IrDARxQUE);
					OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.afn2_f1.SendData,ReturnFeame.Data_uBuf.SendLen);
				}
				break;
			case FN2:
				if(Dat_dbuf.AFN2D.ReadLen < NumOfQue(&RS485RxQUE))
				{
					ReturnFeame.Data_uBuf.SendLen = Dat_dbuf.AFN2D.ReadLen;					
					OutQue(&RS485RxQUE,ReturnFeame.Data_uBuf.afn2_f2.SendData,ReturnFeame.Data_uBuf.SendLen);
				}
				else
				{
					ReturnFeame.Data_uBuf.SendLen = NumOfQue(&RS485RxQUE);
					OutQue(&RS485RxQUE,ReturnFeame.Data_uBuf.afn2_f2.SendData,ReturnFeame.Data_uBuf.SendLen);
				}				
				break;
			case FN3:
				ReturnFeame.Data_uBuf.SendLen = 16;
				break;
			case FN4:
				if(ReturnFeame.Data_uBuf.afn2_f4.AFN != AFN0)
				{
					if(Dat_dbuf.AFN2D.ReadLen < NumOfQue(&IrDARxQUE))
					{
						ReturnFeame.Data_uBuf.SendLen = Dat_dbuf.AFN2D.ReadLen;					
						OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.afn2_f1.SendData,ReturnFeame.Data_uBuf.SendLen);
					}
					else
					{
						ReturnFeame.Data_uBuf.SendLen = NumOfQue(&IrDARxQUE);
						OutQue(&IrDARxQUE,ReturnFeame.Data_uBuf.afn2_f1.SendData,ReturnFeame.Data_uBuf.SendLen);
					}
				}
				else
				{
					ReturnFeame.Data_uBuf.AFN = AFN0;
				}
				break;
			case FN5:
				ReturnFeame.Data_uBuf.SendLen = 1;
				break;
			case FN6:
				break;
			default:
				break;
		}
	}
	else
	{
		ReturnFeame.Data_uBuf.SendLen = 0;		
	}
	ReturnFeame.len = ReturnFeame.Data_uBuf.SendLen+3;
	if((BAT_Low_FLAG != 1))
	{
		ReturnFeame.contrl = 0x80;
	}
	else
	{
		ReturnFeame.contrl = 0x80|(0x01<<6);	
	}
	Return_BT();
}
void BT_Analysis(void)
{
	if(1 == CheckHE())
	{
		switch(Dat_dbuf.AFN)
		{
			case AFN0:
				break;
			case AFN1:
				AF1_Lib_Proc();
				break;
			case AFN2:
				AF2_Lib_Proc();
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
		Sys_run.Check_Bat_Time = 0;
		ADC_filter();
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
	EXTI9_5_ENABLE();
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

