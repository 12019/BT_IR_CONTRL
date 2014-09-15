#include "myconfig.h"
#include "ESAM.h"
#include "stm32f10x_flash.h"
#include    <stdio.h>
#include    <string.h>

volatile FLASH_Status FLASHStatus;

/*************����ͷβ��У��*****************/
u8 CheckHE(void)
{
	u8 tmp,t = 0,Re = 0;
	
	while((NumOfQue(&RxQUE3) != 16))
	{
		IWDG_ReloadCounter();
		delay_nms(100);
		t++;
		if(t>=100)//����10s��ʱ�˳�
		{
		W_Bat = ADC_filter();
		break;
		}
	}
	AllOutQue(&RxQUE3,RxTmp);//��������
	
	if(RxTmp[0] == 0xA9)
	{
		tmp = RxTmp[1];
	}
	if(RxTmp[tmp-1] == 0x16)
	{
InitQue(&RxQUE3);
		return 1;
	}
	else
	{
		InitQue(&RxQUE3);
		return Re;		
	}
}

/*************�������У��*****************/
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

u8 CheckDB(void)
{
	u8 Ln,L,i = 0,sum = 0;
	
	if(RxBuffer3[0] == 0xA9)
	{
		L = RxBuffer3[1];//֡�����ܳ���
	}
	else return 0;
	
	if(RxBuffer3[L-1] == 0x16)
	{
		Ln = RxBuffer3[10];//ת�����ݳ���
		
			if((Ln == L-13)&&(RxBuffer3[3]==0x03||RxBuffer3[3]==0x04))
			{
				for(i=0;i<(RxBuffer3[1]-2);i++)//���Ч��
				{
				sum += RxBuffer3[i];
				}
				if(sum == RxBuffer3[L-2])
				{
					if(RxBuffer3[2]&0x04)//��Ҫ��֤
					{
						if(RxBuffer3[3] == 0x04)//ESAM  (RAND1 && ENCDATA1)
						{
							GET_ESAM = 1;
						}		
						if(Ln == 0)
						{
							ZF_LEN = 0;
							IR_DB_RZ = 1; 
						}
						else 
						{
							ZF_LEN = 1; 
							IR_DB_RZ = 1; 
						}	
					}
					else//����Ҫ��֤
					{
						if(Ln == 0)
						{
							ZF_LEN = 0;
							IR_DB_RZ = 0;	
						}
						else 
						{
							ZF_LEN = 1; 
							IR_DB_RZ = 0;
						}
					}
				return 1;
				}
				else return 0;
			}
			else return 0;	
	}
	else return 0;
}
	
void Set_Sys(void)
{
	char buf[50];
//	u8 command5[] = {"AT+NAME?\r\n"};
	u8 i = 0;
	u32 BaudRate;
	u16 Parity;
	
	/*0xA9 L    C    AFN         ͨ���� ������  Ч��λ �ȴ�����ʱ�� Э�����״̬ ģ�鳬ʱʱ�� ģ�����             CS   0x16*/
	/*               03���Ⳮ��  00 ��  00 1200 00 ż  ����         00 ͸��       ��          4BYTE                         */
	/*               04ESAM      01 485 01 2400 01 ��               01 Э��֧��                                             */
	/*                           02 ��  03 9600 02 ��                                                                       */
	/*               02������           06 38400                                                                            */
	/*0xA9 0x0F 0x00 0x02        0x00   0x00    0x00   0x05         0x01          0x05        0x00 0x00 0x00 0x00  0xC0 0x16*/
	
	if(1 == CheckSum())
	{
		if(RxTmp[1] != 0x10)//���ó��ȹ̶�Ӧ��Ϊ16
		{
     sysread = 0xff;
		}	
	  else
		{
		sysread = 1;
		
//AFN = RxTmp[3];
		
		W_Mode = RxTmp[4];//ͨ����
		W_BaudRate = RxTmp[5];//������	
		W_Parity = RxTmp[6];//Ч��λ
		W_Time = RxTmp[7];//�ȴ�����ʱ��	
		M_Time = RxTmp[9];//ģ�鳬ʱʱ��

		if(W_Time == 0)//��������͹���
		{
			PowerDown();//��λ��������
		}
		
		/*      ������C����      */
		if((RxTmp[2]&0x01))//��������͹���
		{
			PowerDown();//��λ��������
		}
		
		if((RxTmp[2]&0x02))
		{
			if(W_Mode == BLMODE)//BL���ã������� Ч��λ ģ����루4BYTE��
			{		
				/*ģ����루4BYTE��*/
				W_Code[0] = RxTmp[10];
				W_Code[1] = RxTmp[11];	
				W_Code[2] = RxTmp[12];	
				W_Code[3] = RxTmp[13];				

				sprintf(buf,"AT+NAME=BOOST%x%x%x%x%x%x%x%x\r\n",
				(W_Code[0]&0xf0)>>4,W_Code[0]&0x0f,(W_Code[1]&0xf0)>>4,W_Code[1]&0x0f,
				(W_Code[2]&0xf0)>>4,W_Code[2]&0x0f,(W_Code[3]&0xf0)>>4,W_Code[3]&0x0f);	
				
				IWDG_ReloadCounter();
				Clear_RxBuffer3();
				SetUartState(COM3,CBR_38400,USART_Parity_No);//BL 38400 1ͣ ��
				Set_BL_Enter();//����ATģʽ
				delay_nms(1000);//�ȴ���λ
		
				while((buf[i-1]!=0x0D)||(buf[i]!=0x0A))//��������	
				{
				USART_SendData(USART3, buf[i++]);
				delay_nms(10);
				}
				i = 0;
				while(!OK)//while((RxBuffer3[1]!=0x4f)||(RxBuffer3[2]!=0x4b))//'OK' 
				{
					IWDG_ReloadCounter();
					delay_nms(100);
					i++;
					if(i>=30)//BL����ʧ�� ����3s�ٴγ�������
					{	
							Clear_RxBuffer3();
							while((buf[i-1]!=0x0D)||(buf[i]!=0x0A))//�������� 
							{
							USART_SendData(USART3, buf[i++]);
							delay_nms(10);
							}
							i = 0;
							while(!OK)
							{
								delay_nms(100);
								i++;
								if(i>=30)//BL�ٴ�����ʧ�� ����ϵͳ
								{	
								NVIC_SystemReset();	
								}
							}
					}
				}
				NVIC_SystemReset();//������ɺ�����ϵͳ
			}
		}	
		
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
	
			if(W_Mode == UARTMODE)//ͨ����Ϊ485
			{
			SetUartState(COM1, BaudRate,Parity);
			}
			else if(W_Mode == IRDAMODE)//ͨ����Ϊ����
			{
				switch(BaudRate)
				{
					case(1200):
					{
					IR_BaudRate_Time = 83;
					break;
					}				
					case(2400):
					{
					IR_BaudRate_Time = 42;
					break;
					}
					default:
					IR_BaudRate_Time = 83;
					break;	
				}
			}	
		
				/*       Э�����״̬      */
				if(RxTmp[8]==0)//����͸��:IR 485 ESAM   RxTmp[4] == 0 1 3
				{
					TrasferMode = 0;//����ͨ����͸�� ֱ��ת��
				}
				else//Э��֧��:���Ⳮ�� ESAM �����л�
				{	
					if(W_Mode==0)//���Ⳮ��
					{	
						TrasferMode = 3;
					}							
				}
					
			if(W_Mode == 0)//IR
			{			
				IR_GPIO_Init();/*ģ�⴮���ж�����*/
				if(!TrasferMode)
				{
					IR_TC = 1;
				}
			}
			else if(W_Mode == 1)//485
			{
				Rs485_TC = 1;
				PWR_485_ON();
			}
//			else if(W_Mode == 3)//ESAM
//			{
//			IR_GPIO_Init();/*ģ�⴮���ж�����*/			
//			PWR_ESAM_ON();
//			delay_nms(100);
//			ESAM_Reset();
//			}
			
			BL_REQ_FLAG = 2;//�ǵ�һ֡Ӧ�����ݱ�־
			
			BL_STA = 2;//BL������ LED 1HZ
	}
 }
}

void Send_req(void)//������
{
	          /*��ʼ  ����  ģ�鷢��  ������:AFN   ��Կʣ����֤����     ʣ�����  Э��汾 ����汾                 CS      ����*/
	u8 req[] = {0xA9, 0x12, 0x80,            0x02, 0x00,0x00,0x00,0x00, 0x00,0x00, 0x00,   0x00,0x00,0x00,0x00,0x00, 0x00,   0x16};//������ �����򸽴���Կʣ����֤����(4BYTE)��ʣ�������Ϣ(2BYTE)
	u8 i;
	u8 temp[5];
		
	req[4] = (RZ_Counter&0xff000000)>>24;
	req[5] = (RZ_Counter&0xff0000)>>16;
	req[6] = (RZ_Counter&0xff00)>>8;
	req[7] = RZ_Counter&0xff;
	
	req[8] =  W_Bat&0xff;
	req[9] = (W_Bat&0xff00)>>8;
		
	req[10] = XVER;
		
	GetBuildTime(temp);
	
	req[11] = SVER;
	req[12] = 0x20;
	req[13] = temp[2];
	req[14] = temp[1];
	req[15] = temp[0];
		
		for(i=0;i<16;i++)
		{
		req[16] += req[i];//CS
		}
		
		BL_REQ_FLAG = 1;//������Ӧ������֡��־
		USART3send(req,0x12);	
}

void PowerDown(void)
{
	BL_STA = 0;
	RUN_OFF();//��LED	
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
	GPIO_BLRST_OFF();//��λģ��
	GPIO_BLSET_OFF();//KEY�õ� ���
	PWR_BL_ON();
	BL_STA = 1;//δ����
}

void PowerOff(void)
{
	PWR_IR_OFF();
	PWR_BL_OFF();
	PWR_485_OFF();
	PWR_ESAM_OFF();
}

void Set_BL_Enter(void)
{
	AT_FLAG = 1;
	PWR_BL_OFF();
	GPIO_BLSET_ON();
	delay_nms(50);
	PWR_BL_ON();
}
void Set_BL_Exit(void)
{
	AT_FLAG = 0;
	GPIO_BLSET_OFF();
	PWR_BL_OFF();
//	delay_nms(50);
//	PWR_BL_ON();	
}

void Req_BL(void)
{
	u16 timeout = 0;
	u16 timeout1 =0;
	
	while((LED2_0)&&(timeout1++ < 6000))//�ȴ����ʱ�� Լ10min
	{
		IWDG_ReloadCounter();
		delay_nms(100);
	}
	
	if(timeout1 >= 6000)
	{
		timeout1 = 0;
		PowerDown();
	}
	
	if(LED2_1)
	{
		do
		{
			IWDG_ReloadCounter();
			
			if(timeout>29)
			{
				timeout = 0;
				PowerDown();
			}	
			timeout++;
			
			Send_req();//BL����Ӧ�� ������
			
			Set_Sys();//Ӧ���������	
			
		}while((LED2_1)&&(sysread == 0));//�������� ����Ӧ��
	}
	
	if(sysread != 1)//Set_SysЧ���sysread == 1
	{
		NVIC_SystemReset();
	}
}

void SendOneByte(u8 Byte)
{
	if(POW_IR)
	{
		PWM_Enable();
		PWR_IR_ON();
	}
	POW_IR = 0;
//	EXTI9_5_DISABLE();	
	RX_FLAG = 0;
	CountRX = 0;
	Receive_bit = 0;
	
	TX_FLAG = 1;
	Tx_Parity = 0;
	BYTE = Byte;
	while(TX_FLAG == 1);
	delay_nms(1);
//	EXTI9_5_ENABLE();	
}

void SendBytes(u8 *str,u8 len)
{
  u16 i;
  for(i=0;i<len;i++)
  {
  	 SendOneByte(str[i]);
  }
}

//void SendStr(u8 *str) //�����ַ������������������0x00���жϷ�����
//{
// while(*str)
// SendOneByte(*str++);
//}

void Clear_RxBuffer1(void)
{
	u16 i;
	RxCounter1 = 0;
	for(i=0;i<MAXBUFFER;i++)
	{
		RxBuffer1[i] = 0x00;
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

//void Clear_IrRxBuffer1(void)
//{
//	u16 i;
//	IrRxCounter1 = 0;
//	for(i=0;i<MAXIRBUFLEN;i++)
//	{
//		IrBuf1[i] = 0x00;
//	}
//}

//void Clear_IrRxBuffer2(void)
//{
//	u16 i;
//	IrRxCounter2 = 0;
//	for(i=0;i<MAXIRBUFLEN;i++)
//	{
//		IrBuf2[i] = 0x00;
//	}
//}

void BL_Unpack(u8 *Buffer, u16 Length)//�������ݽ��
{
	/*0xA9 L     C        AFN         DBBH          Ln         CS   0x16*/
	/*                    03���Ⳮ��  6BYTE                             */
	/*                    04ESAM                                        */
	/*                                                                  */
	/*           ��Ҫ��֤                                               */
	/*0xA9 Ln+13 0x04     0x03        0 0 0 0 0 0   n BYTE     0x00 0x16*/	
	
	
	         /* S    L   ����ģ�鷢��  AFN  ��֤ʧ��  CS    P */
u8 RZSB[] = {0xA9,0x07,0x80,         0x03,0xff,    0x00,0x16};
u8 DB[6] = {0};
u8 i = 0;
u8 Ve;
	
	if(1 == CheckDB())
	{
		for(i=0;i<6;i++)
		{
		DB[i] = Buffer[4+i];//6BYTE�����
		}
		
		if(IR_DB_RZ)//��֤
		{
			PWR_ESAM_ON();
			PWM_Enable();
			PWR_IR_ON();
			
				if(GET_ESAM)
				{
					GET_ESAM = 0;
					Get_ESAM_Data();	
				}
				else
				{	
					Ver_flag = 1;
					Ve = DoVerifica(DB);
					Ver_flag = 0;
					
					PWR_ESAM_OFF();
							
						if((Ve == PRIVATESUC)||(Ve == PUBLICSUC))//��ESAM�����ɹ�  
						{
							if(ZF_LEN == 0)
							{
								RZSB[4] = Ve;
								for(i=0;i<5;i++)
								{
									RZSB[5] += RZSB[i];//CS
								}
								USART3send(RZSB,7);
							}
							else
							{
								for(i=11;i<(Length-2);i++)//ʣ���ֽ�ת������ 
								{
									SendOneByte(Buffer[i]);
								}
							}
						}
						else//��֤ʧ�� 2:VERIFICAFAIL	3:NORETURN			
						{
							RZSB[4] = Ve;
							for(i=0;i<5;i++)
							{
								RZSB[5] += RZSB[i];//CS
							}
							USART3send(RZSB,7);
						}
					}
			}
		
		else//����֤
		{
			PWM_Enable();
			PWR_IR_ON();
			
			if(ZF_LEN == 0)
			{
				RZSB[4] = NORETURN;//��������Ӧ
				for(i=0;i<5;i++)
				{
					RZSB[5] += RZSB[i];//CS
				}
				USART3send(RZSB,7);
			}
			else
			{
				for(i=11;i<(Length-2);i++)//ʣ���ֽ�ת������ 
				{
					SendOneByte(Buffer[i]);
				} 
			}
		}
	}
	Clear_RxBuffer3();
}

void ESAM_Info(void)
{
	ESAM_Reset();//ESAM��λ
	
	RZ_Counter = ReadCounter();
	
	PWR_ESAM_OFF();
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

void WriteFLAG(void)
{
	FLASH_Unlock();	   /* Unlock the Flash Program Erase controller */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);		/* Clear All pending flags */
	FLASH_ErasePage(SaveDataStartAddr);//����1k    
  delay_nms(10);           
	FlashWrite(0x11011011);						 
}

u8 LoadFlash(void)
{
	uint32_t FLASH_READ;
	
	FLASH_READ = (*(volatile uint32_t*) SaveDataStartAddr);
	
	if(FLASH_READ == 0x11011011)//�Ѿ��ɹ�����BL
	{
	return 1;
	}
	else return 0;
}

