#include "stm32f10x.h"
#include "myconfig.h"

__IO uint16_t ADC_ConvertedValue[N] = {0};

u16 ADC_filter(void)
{
	u16 Value[N] = {0};
	u8 i = 0,tmp1,tmp2;
	u16 max = 0,min = 0xffff;
  u32  sum = 0,temp = 0;
	
	for(i=0;i<N;i++)
	{
		Value[i] = ADC_ConvertedValue[i];
		if(Value[i] > max) max = Value[i];
		if(min > Value[i]) min = Value[i];
		sum += (u32)Value[i];
	}
	temp = sum - max -min;
	temp = temp /(N-2);
	
	if(temp > VBAT_MAX)
	{
		tmp1 = 99;
		tmp2 = 99;
		temp = (Hex2Bcd(tmp1)<<8) | Hex2Bcd(tmp2);
		if(BAT_Low_FLAG)
		{
			Sys_run.Sleep_run_Time_BatLow = 0;
			BAT_Low_FLAG = 0;
		}
	}	
	else if(temp < VBAT_MIN)//������ 
	{
		temp = 0x0;
		if(BAT_Low_FLAG != 1)
		{
			BAT_Low_FLAG = 1;
			RUN_ON();
			Sys_config.SleepTime_BatLow = MAXTIMEBATLOW;
		}
	}
	else
	{
		temp = (temp-VBAT_MIN)*10000/VBAT_USE;
		tmp1 = temp%100;//ʮ��λ �ٷ�λ
		tmp2 = temp/100;//��λ ʮλ
		temp = 0;
		temp = (Hex2Bcd(tmp1)<<8) | Hex2Bcd(tmp2);
		if(BAT_Low_FLAG)
		{
			Sys_run.Sleep_run_Time_BatLow = 0;
			BAT_Low_FLAG = 0;
		}
	}
	return (u16)temp;
}

		
/******************************************************************************
* FunctionName   : ADC_GPIO_Config()
* Description    : AD�ܽ�����PA1_3  ADC1 Channel 1 2 3 /PC0_3  ADC1 Channel 10 11 12 13
* EntryParameter : None
* ReturnValue    : None
******************************************************************************/
static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOB clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 |RCC_APB2Periph_GPIOB, ENABLE);

	/* PB0  ADC1 Channel8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

/******************************************************************************
* FunctionName   : ADC_Mode_Config()
* Description    : ADģʽ���� --��DMA
* EntryParameter : None
* ReturnValue    : None
******************************************************************************/
static void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);/* Enable ADC clock */		
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);					/*����ADCʱ�ӣ�ΪPCLK2��2��Ƶ����9Hz*/
	/* DMA channel1 configuration */

	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;      // �����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//ADC_Raw_Value;      // �ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // DMA ���䷽����
	DMA_InitStructure.DMA_BufferSize = N*M;											    // ����DMA�ڴ���ʱ�������ĳ��� word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          // ����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // ���������ֳ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // �ڴ������ֳ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     // ����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; // ����DMA�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // ����DMA��2��memory�еı����������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);   

	DMA_Cmd(DMA1_Channel1, ENABLE);			      /* Enable DMA channel1 */

	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; 	 //ɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ������ͣ�ؽ���ADCת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = M;	 	//Ҫת����ͨ����ĿM
	ADC_Init(ADC1, &ADC_InitStructure);

	/*����ADC1��ͨ��   Ϊ55.5���������ڣ����� */ 		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);	  /* Enable ADC1 DMA */	

	ADC_Cmd(ADC1, ENABLE);		 /* Enable ADC1 */

	ADC_ResetCalibration(ADC1);	/*��λУ׼�Ĵ��� */ 	

	while(ADC_GetResetCalibrationStatus(ADC1));	 /*�ȴ�У׼�Ĵ�����λ��� */
	ADC_StartCalibration(ADC1);			 /* ADCУ׼ */	
	while(ADC_GetCalibrationStatus(ADC1));	/* �ȴ�У׼���*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* ����û�в����ⲿ����������ʹ���������ADCת�� */ 
}

/******************************************************************************
* FunctionName   : myADC_Init()
* Description    : AD��ʼ��
* EntryParameter : None
* ReturnValue    : None
******************************************************************************/
void myADC_init(void)
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
} 

/******************************************************************************
**					����������END FILE
******************************************************************************/
