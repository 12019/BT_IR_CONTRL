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
	else if(temp < VBAT_MIN)//电量低 
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
		tmp1 = temp%100;//十分位 百分位
		tmp2 = temp/100;//个位 十位
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
* Description    : AD管脚配置PA1_3  ADC1 Channel 1 2 3 /PC0_3  ADC1 Channel 10 11 12 13
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
* Description    : AD模式配置 --》DMA
* EntryParameter : None
* ReturnValue    : None
******************************************************************************/
static void ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);/* Enable ADC clock */		
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);					/*配置ADC时钟，为PCLK2的2分频，即9Hz*/
	/* DMA channel1 configuration */

	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;      // 外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;//ADC_Raw_Value;      // 内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // DMA 传输方向单向
	DMA_InitStructure.DMA_BufferSize = N*M;											    // 设置DMA在传输时缓冲区的长度 word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 设置DMA的内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // 内存数据字长
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     // 设置DMA的传输模式：连续不断的循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置DMA的优先级别
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // 设置DMA的2个memory中的变量互相访问
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);   

	DMA_Cmd(DMA1_Channel1, ENABLE);			      /* Enable DMA channel1 */

	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; 	 //扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = M;	 	//要转换的通道数目M
	ADC_Init(ADC1, &ADC_InitStructure);

	/*配置ADC1的通道   为55.5个采样周期，序列 */ 		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);	  /* Enable ADC1 DMA */	

	ADC_Cmd(ADC1, ENABLE);		 /* Enable ADC1 */

	ADC_ResetCalibration(ADC1);	/*复位校准寄存器 */ 	

	while(ADC_GetResetCalibrationStatus(ADC1));	 /*等待校准寄存器复位完成 */
	ADC_StartCalibration(ADC1);			 /* ADC校准 */	
	while(ADC_GetCalibrationStatus(ADC1));	/* 等待校准完成*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
}

/******************************************************************************
* FunctionName   : myADC_Init()
* Description    : AD初始化
* EntryParameter : None
* ReturnValue    : None
******************************************************************************/
void myADC_init(void)
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
} 

/******************************************************************************
**					　　　　　END FILE
******************************************************************************/
