#include "ESAM.h"

u8 ESAMID[8];
u8 ESAMRand1[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	//随机数1
//u8 ESAMRand1[8]={0xC0,0x63,0xC2,0xBB,0xC2,0x4B,0x88,0x3A};
u8 ESAMEncData1[8]; //ESAM密文1
u8 ESAMEncData2[8];	//ESAM密文2

u8 DBRand2[8];	//电表随机数2
u8 DBEncData1[8];	//电表密文1
u8 DBESAMID[8];	//电表ESAM的ID

void ESAM_Reset(void)
{
	delay_nms(10);
	Set_ESAM_power_ON();
	delay_nms(100);
	GPIO_ESAMRST_ON();/* Set RSTIN LOW */  
	delay_nms(0x100);	
	GPIO_ESAMRST_OFF(); 	/* Set RSTIN HIGH */  
	delay_nms(100);
}

void SendToESAM(u8 *buffer,u16 len)
	{
	TxCounter2 = 0;
	MaxNbrofTx2 = len;
	TxBuffer2 = buffer;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	while(TxCounter2 != MaxNbrofTx2);
	}

u8 ISO7816wait(u16 len)
{
	u16 i = 0;
	while((RxCounter2 <= len)&&(i < ISO7816MAXWAITmS))
	{
		delay_nms(1);
		i++;
	}
	if(i >= ISO7816MAXWAITmS)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

u16 doISO7816_TPDU_T0(u8 *pAPDU, u8 *pMessage,u16 wLength) //7816协议解析
{
	u16 NeNc;
	u16 indexApdu = 4;
	u16 indexMessage = 0;
	u8 temp[MAXBUFFER];
	u8 SW1 = 0;
	u8 procByte;
	u8 cmdCase;
	u16 i = 0;
	
	Clear_RxBuffer2();

	SendToESAM(pAPDU,5);
	
	indexApdu = 5;

	if( wLength == 4 )
	{
		cmdCase = CASE1;
		NeNc = 0;
	}
	else if( wLength == 5) 
	{
		cmdCase = CASE2;
		NeNc = pAPDU[4]; // C5
		if (NeNc == 0) 
		{
			NeNc = 256;
		}
	}
	else if( wLength == 6) 
	{
		NeNc = pAPDU[4]; // C5
		cmdCase = CASE3;
	}
	else if( wLength == 7) 
	{
		NeNc = pAPDU[4]; // C5
		if( NeNc == 0 ) 
		{
			cmdCase = CASE2;
			NeNc = (pAPDU[5]<<8)+pAPDU[6];
		}
		else 
		{
			cmdCase = CASE3;
		}
	}
	else 
	{
		NeNc = pAPDU[4]; // C5
		if( NeNc == 0 ) 
		{
			cmdCase = CASE3;
			NeNc = (pAPDU[5]<<8)+pAPDU[6];
		}
		else 
		{
			cmdCase = CASE3;
		}
	}

	// Handle Procedure Bytes
	do 
	{
		if(ISO7816wait(indexApdu+i) == 1)
		{
			procByte = RxBuffer2[indexApdu+i];
			i++;
		}
		else
		{
			break;
		}		
		// Handle NULL
		if ( procByte == 0x60 ) 
		{
			continue;
		}
		// Handle SW1
		else if (((procByte & 0xF0) ==0x60) || ((procByte & 0xF0) ==0x90)) 
		{
			SW1 = 1;
			break;
		}
		// Handle INS
		else if ( pAPDU[1] == procByte)
		{
			if (cmdCase == CASE2) 
				{
					// receive data from card
					do 
						{
							ISO7816wait(indexApdu+1+indexMessage);
							temp[indexMessage] = RxBuffer2[indexApdu+1+indexMessage];
							indexMessage++;
						} while( 0 != --NeNc );
				}
			else 
				{
						 // Send data
					do 
					{
						SendToESAM(&(pAPDU[indexApdu]),1);
						indexApdu++;
					} while( 0 != --NeNc );
				}
		}
		// Handle INS ^ 0xff
		else if ( pAPDU[1] == (procByte ^ 0xff)) 
		{
			if (cmdCase == CASE2) 
			{
				// receive data from card
				ISO7816wait(indexApdu+1+indexMessage);
				temp[indexMessage] = RxBuffer2[indexApdu+1+indexMessage];
				indexMessage++;
			}
			else 
			{
				SendToESAM(&(pAPDU[indexApdu]),1);
				indexApdu++;				
			}
			NeNc--;
		}
		else 
		{
			break;
		}
	} while (NeNc != 0);

	// Status Bytes
	if (SW1 == 0) 
	{
		ISO7816wait(indexApdu+1+indexMessage);
		temp[indexMessage] = RxBuffer2[indexApdu+1+indexMessage];
		indexMessage++;		
	}
	else 
	{
		temp[indexMessage] = procByte;//RxBuffer2[6+indexMessage];
		indexMessage++;
	}
	ISO7816wait(indexApdu+1+indexMessage);
	temp[indexMessage] = RxBuffer2[indexApdu+1+indexMessage];
	indexMessage++;
	for(i=0;i<indexMessage;i++)
	{
		pMessage[i] = temp[i];
	}
	Clear_RxBuffer2();	
	return( indexMessage );
}

u8 ReadRand1(void)//读取随机数1
{
	u8 pCommand[5]={0x00,0x84,0x00,0x00,0x08};
	u8 pGet[10];
	u8 i;
	doISO7816_TPDU_T0(pCommand, pGet,5);
	if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
	{
		for(i=0;i<8;i++)
		{
			ESAMRand1[i] = pGet[i];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

u8 ReadEncData1_Private(u8 *DbAddr)
{
	u8 pCommand1[21]={0x80,0xFA,0x00,0x01,0x10,0xa7,0x01,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12};
	u8 pCommand2[13]={0x80,0xFA,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u8 pCommand3[5]={0x00,0xC0,0x00,0x00,0x08};
	u8 pGet[10];	
	u8 i;
	for(i=0;i<6;i++)
	{
		pCommand1[7+i] = DbAddr[i];
		pCommand1[15+i] = DbAddr[i];
	}
	for(i=0;i<8;i++)
	{
		pCommand2[5+i] = ESAMRand1[i];
	}
	doISO7816_TPDU_T0(pCommand1, pGet,21);
	if((pGet[0] == 0x90)&&(pGet[1] == 0x00))
	{
		doISO7816_TPDU_T0(pCommand2, pGet,13);
		if((pGet[0] == 0x61)&&(pGet[1] == 0x08))
		{
			doISO7816_TPDU_T0(pCommand3, pGet,5);
			if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
			{
				for(i=0;i<8;i++)
				{
					ESAMEncData1[i] = pGet[i];
				}
				return 1;
			}
			else
			{
				return 0;
			}			
		}
		else
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}		
}

u8 ReadEncData1_Public(u8 *DbAddr)
{
	u8 pCommand1[13]={0x80,0xFA,0x00,0x03,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u8 pCommand2[13]={0x80,0xFA,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u8 pCommand3[5]={0x00,0xC0,0x00,0x00,0x08};
	u8 pGet[10];	
	u8 i;
	for(i=0;i<6;i++)
	{
		pCommand1[7+i] = DbAddr[i];
	}
	for(i=0;i<8;i++)
	{
		pCommand2[5+i] = ESAMRand1[i];
	}
	doISO7816_TPDU_T0(pCommand1, pGet,21);
	if((pGet[0] == 0x90)&&(pGet[1] == 0x00))
	{
		doISO7816_TPDU_T0(pCommand2, pGet,13);
		if((pGet[0] == 0x61)&&(pGet[1] == 0x08))
		{
			doISO7816_TPDU_T0(pCommand3, pGet,5);
			if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
			{
				for(i=0;i<8;i++)
				{
					ESAMEncData1[i] = pGet[i];
				}
				return 1;
			}
			else
			{
				return 0;
			}			
		}
		else
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}		
}

u8 CompEncData1(void)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(ESAMEncData1[i] != DBEncData1[i])
		{
			break;
		}
	}
	if(i<8)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

u8 ReadEncData2_Private(u8 *DbAddr)
{
	u8 pCommand1[13]={0x80,0xFA,0x00,0x02,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12};
	u8 pCommand2[13]={0x80,0xFA,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u8 pCommand3[5]={0x00,0xC0,0x00,0x00,0x08};
	u8 pGet[10];	
	u8 i;
	for(i=0;i<6;i++)
	{
		pCommand1[7+i] = DbAddr[i];
	}
	for(i=0;i<8;i++)
	{
		pCommand2[5+i] = DBRand2[i];
	}
	doISO7816_TPDU_T0(pCommand1, pGet,21);
	if((pGet[0] == 0x90)&&(pGet[1] == 0x00))
	{
		doISO7816_TPDU_T0(pCommand2, pGet,13);
		if((pGet[0] == 0x61)&&(pGet[1] == 0x08))
		{
			doISO7816_TPDU_T0(pCommand3, pGet,5);
			if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
			{
				for(i=0;i<8;i++)
				{
					ESAMEncData2[i] = pGet[i];
				}
				return 1;
			}
			else
			{
				return 0;
			}			
		}
		else
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}		
}

u8 ReadEncData2_Publice(void)
{
	u8 pCommand1[13]={0x80,0xFA,0x00,0x04,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12};
	u8 pCommand2[13]={0x80,0xFA,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u8 pCommand3[5]={0x00,0xC0,0x00,0x00,0x08};
	u8 pGet[10];	
	u8 i;
	for(i=0;i<8;i++)
	{
		pCommand1[5+i] = DBESAMID[i];
	}
	for(i=0;i<8;i++)
	{
		pCommand2[5+i] = DBRand2[i];
	}
	doISO7816_TPDU_T0(pCommand1, pGet,21);
	if((pGet[0] == 0x90)&&(pGet[1] == 0x00))
	{
		doISO7816_TPDU_T0(pCommand2, pGet,13);
		if((pGet[0] == 0x61)&&(pGet[1] == 0x08))
		{
			doISO7816_TPDU_T0(pCommand3, pGet,5);
			if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
			{
				for(i=0;i<8;i++)
				{
					ESAMEncData2[i] = pGet[i];
				}
				return 1;
			}
			else
			{
				return 0;
			}			
		}
		else
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}		
}

void Clear_IrDARxBuffer1(void)
{
	u16 i;
	IrDARxCounter = 0;
	for(i=0;i<MAXBUFFER;i++)
	{
		IrDARxBuffer1[i] = 0x00;
	}
}


void IrDASend(u8 *buffer,u16 len)
{
	u16 i;
	Clear_IrDARxBuffer1();
	for(i=0;i<len;i++)
	{
		SendOneByte(buffer[i]);		
	}
}

u8 IrDAwait(u16 len)
{
	u16 i = 0;
	while((IrDARxCounter < len)&&(i < IRDAMAXWAITmS))
	{
		IWDG_ReloadCounter();	
		delay_nms(1);
		i++;
	}
	if(i >= IRDAMAXWAITmS)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

u8 Mem_Reverse(u8 *in, u8 len, u8 *out)
{
	u8 i ;
	u8 tmp[64];
	if(len >63)
	{
		return 0;
	}
	for(i=0;i<len;i++)
	{
		tmp[i] = in[i] ;
	}	
	for(i=0;i<len;i++)
	{
		out[i] = tmp[len-1-i] ;
	}
	return 1 ;
}

u8 Pact645_Analysis(u8 *bCheckBuf, u8 bChecklen, u8 *DbAddr)
{
	u8 bit, dlen;
	u8 i;
	u8 sum = 0;
	u8 tmp[8];
	for(bit=0; bit+12 <= bChecklen; bit++)
	{
		if((bCheckBuf[bit] == 0x68) && (bCheckBuf[bit+7] == 0x68))
		{
			dlen = bCheckBuf[bit+9];								
			if((bit+dlen+12) <= bChecklen)							
			{
				if(bCheckBuf[bit+9+dlen+2] == 0x16)	
				{
					for(i=(bit+1);i<(bit+7);i++)
					{
						if(bCheckBuf[i] != (DbAddr[i-bit-1])) 
						{
							return 1;				//检验电表地址
						}						
					}
					for(i=bit;i<bit+9+dlen+1;i++)
					{
						sum += bCheckBuf[i];
					}
					if(sum != bCheckBuf[bit+9+dlen+1])
					{
						return 2;					//CS校验未通过
					}		
					else
					{
						break;
					}
				}
			}
		}
	}
	for(i=0;i<8;i++)
	{
		tmp[i] = bCheckBuf[bit+20+i] - 0x33;
	}
	Mem_Reverse(tmp, 8, DBESAMID);
	for(i=0;i<8;i++)
	{
		tmp[i] = bCheckBuf[bit+28+i] - 0x33;
	}
	Mem_Reverse(tmp, 8, DBEncData1);
	for(i=0;i<8;i++)
	{
		tmp[i] = bCheckBuf[bit+36+i] - 0x33;
	}
	Mem_Reverse(tmp, 8, DBRand2);	
	return 3; //全部成功
}

u8 GetDBInfo(u8 *DbAddr)
{
	u8 i;
	u8 Re;
	u8 ESAMRand1_b[8];
	u8 pCommand1[32]={0xFE,0xFE,0xFE,0xFE,0x68,0x32,0x00,0x00,0x00,0x00,0x00,0x68,
										0x03,0x10,0x32,0x36,0xB3,0x3A,0x39,0x35,0x35,0x3B,
										0x6F,0xC2,0x33,0x33,0xEE,0x6D,0x33,0x33,0x00,0x16};
	for(i=0;i<6;i++) //更改表号
	{
		pCommand1[5+i] = DbAddr[i];
	}
	Mem_Reverse(ESAMRand1,8,ESAMRand1_b);
	for(i=0;i<8;i++)	//添加随机数
	{
		pCommand1[22+i] = ESAMRand1_b[i]+0x33;
	}	
//	pCommand1[18] = (u8)(OPERATOR0&0xFF)+0x33;
//	pCommand1[19] = (u8)(OPERATOR1&0xFF)+0x33;
//	pCommand1[20] = (u8)(OPERATOR2&0xFF)+0x33;
//	pCommand1[21] = (u8)(OPERATOR3&0xFF)+0x33;
	for(i=4;i<30;i++)	//CS计算
	{
		pCommand1[30] +=  pCommand1[i];
	}
	
	IrDASend(pCommand1,32);
	
	Re = IrDAwait(50);
	Pact645_Analysis(IrDARxBuffer1, IrDARxCounter, DbAddr);
	return Re;
}

u8 Pact645_Analysis2(u8 *bCheckBuf, u8 bChecklen, u8 *DbAddr)
{
	u8 bit,dlen;
	u8 i;
	u8 sum = 0;
	
	for(bit=0; bit+12 <= bChecklen; bit++)
	{
		if((bCheckBuf[bit] == 0x68) && (bCheckBuf[bit+7] == 0x68))
		{
			dlen = bCheckBuf[bit+9];								
			if((bit+dlen+12) <= bChecklen)							
			{
				if(bCheckBuf[bit+9+dlen+2] == 0x16)	
				{
					for(i=(bit+1);i<(bit+7);i++)
					{
						if(bCheckBuf[i] != (DbAddr[i-bit-1])) 
						{
							return 1;				//检验电表地址
						}						
					}
					for(i=bit;i<bit+9+dlen+1;i++)
					{
						sum += bCheckBuf[i];
					}
					if(sum != bCheckBuf[bit+9+dlen+1])
					{
						return 2;					//CS校验未通过
					}
					else
					{
						break;
					}					
				}
			}
		}
	}
	if((dlen >= 4)&&(bCheckBuf[bit+9+dlen] == 0x3A)&&(bCheckBuf[bit+8+dlen] == 0x33)&&(bCheckBuf[bit+7+dlen] == 0x36)&&(bCheckBuf[bit+6+dlen] == 0x32))
	{
		return 3; //全部成功		
	}
	else
	{
		return 4;
	}
}


u8 DoVerifica(u8 *DbAddr)
{
	u8 Re;
	u8 i;
	u8 pCommand1[32]={0xFE,0xFE,0xFE,0xFE,0x68,0x31,0x00,0x00,0x00,0x00,0x00,0x68,
										0x03,0x10,0x32,0x36,0x33,0x3A,0x39,0x35,0x35,0x3B,
										0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x00,0x16};
	u8 ESAMEncData2_b[8];
	u8 DBADD[6];
	for(i=0;i<6;i++)//更改表号
	{
		pCommand1[5+i] = DbAddr[i];
	}
Mem_Reverse(DbAddr, 6, DBADD);
	ReadRand1();
	Re = GetDBInfo(DbAddr);
	if(Re == 0)
	{
		Re = NORETURN;
		return Re;
	}
	
ReadEncData1_Private(DbAddr);
	if(CompEncData1() == 1)
	{
ReadEncData2_Private(DbAddr);
		Mem_Reverse(ESAMEncData2, 8, ESAMEncData2_b);
		for(i=0;i<8;i++)
		{
			pCommand1[22+i] = ESAMEncData2_b[i]+0x33;
		}
//		pCommand1[18] = (u8)(OPERATOR0&0xFF)+0x33;
//		pCommand1[19] = (u8)(OPERATOR1&0xFF)+0x33;
//		pCommand1[20] = (u8)(OPERATOR2&0xFF)+0x33;
//		pCommand1[21] = (u8)(OPERATOR3&0xFF)+0x33;
		for(i=4;i<30;i++)	//CS计算
		{
			pCommand1[30] +=  pCommand1[i];
		}		
		IrDASend(pCommand1,32);
		IrDAwait(20);
		if(Pact645_Analysis2(IrDARxBuffer1, IrDARxCounter, DbAddr) == 3)
		{
			Re = PRIVATESUC;			
		}
		else 
		{
			Re = VERIFICAFAIL;
		}
	}
	else
	{
ReadEncData1_Public(DbAddr);
		if(CompEncData1() == 1)
		{
			ReadEncData2_Publice();
			Mem_Reverse(ESAMEncData2, 8, ESAMEncData2_b);
			for(i=0;i<8;i++)
			{
				pCommand1[22+i] = ESAMEncData2_b[i]+0x33;
			}
//			pCommand1[18] = (u8)(OPERATOR0&0xFF)+0x33;
//			pCommand1[19] = (u8)(OPERATOR1&0xFF)+0x33;
//			pCommand1[20] = (u8)(OPERATOR2&0xFF)+0x33;
//			pCommand1[21] = (u8)(OPERATOR3&0xFF)+0x33;
			for(i=4;i<30;i++)	//CS计算
			{
				pCommand1[30] +=  pCommand1[i];
			}		
			IrDASend(pCommand1,32);
			IrDAwait(20);
			if(Pact645_Analysis2(IrDARxBuffer1, IrDARxCounter, DbAddr) == 3)
			{
				Re = PUBLICSUC;			
			}
		}
		else
		{
			Re = VERIFICAFAIL;
		}
	}
	return Re;
}

u32 ReadCounter(void)
{
	u8 pCommand[5]={0x00,0xB0,0x9A,0x00,0x04};
	u8 pGet[6];
	u8 i;
	u32 Re = 0;
	doISO7816_TPDU_T0(pCommand, pGet,5);
	if((pGet[4] == 0x90)&&(pGet[5] == 0x00))
	{
		for(i=0;i<4;i++)
		{
			Re |= ((pGet[i]&0xFF)<<(24 - 8*i));
		}
	}
	return Re;
}

void ReadESAMID(void)
{
	u8 pCommand[5]={0x00,0xB0,0x99,0x01,0x08};
	u8 pGet[10];
	u8 i;
	doISO7816_TPDU_T0(pCommand, pGet,5);
	if((pGet[8] == 0x90)&&(pGet[9] == 0x00))
	{
		for(i=0;i<8;i++)
		{
			ESAMID[i]= pGet[i];
		}
	}
}

u8 ReadESAMSta(void)
{
	u8 pCommand[5]={0x00,0xB0,0x92,0x00,0x01};
	u8 pGet[3];
	u8 Re = 0;
	doISO7816_TPDU_T0(pCommand, pGet,5);
	if((pGet[1] == 0x90)&&(pGet[2] == 0x00))
	{
		Re = pGet[0];
	}
	return Re;
}

void Get_ESAM_Data(void)
{
	u8 i = 0;
	u8 Dbad[6] = {0};
		         /* S    L   蓝牙模块发出  AFN  认证失败  CS    P */
	u8 RZSB[] = {0xA9,0x07,0x80,         0x04,0xff,    0x00,0x16};
//	u8 DBADD[6];
	u8 ESAM_Data_Send_Back[23] = {0xa9,0x17,0x84,0x04,0x11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x00,0x16};
	
	PWR_ESAM_ON();
	PWM_Enable();
	PWR_IR_ON();			
	
	for(i=0;i<6;i++)
	{
	Dbad[i] = RxBuffer3[4+i];//6BYTE电表表号
	}
	
//	Mem_Reverse(Dbad, 6, DBADD);
	
ReadRand1();

//	if(GetDBInfo(Dbad)== 0)
//	{
//		RZSB[4] = NORETURN;
//		for(i=0;i<5;i++)
//		{
//		RZSB[5] += RZSB[i];//CS
//		}
//		USART3send(RZSB,7);
//	}
//	else
//	{
		if(ReadEncData1_Private(Dbad) == 1)		
		{
			for(i=0;i<8;i++)
			{
				ESAM_Data_Send_Back[5+i] = ESAMRand1[7-i];
			}
			for(i=0;i<8;i++)
			{
				ESAM_Data_Send_Back[5+i+8] = ESAMEncData1[7-i];
			}
			
			for(i=0;i<21;i++)//CS
			{
				ESAM_Data_Send_Back[21] += ESAM_Data_Send_Back[i];
			}
			
			USART3send(ESAM_Data_Send_Back,0x17);
		}
		else
		{
			RZSB[4] = VERIFICAFAIL;
			for(i=0;i<5;i++)
			{
			RZSB[5] += RZSB[i];//CS
			}
			USART3send(RZSB,7);
		}
//	}
	PWR_ESAM_OFF();
}
