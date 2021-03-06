#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "hax.h"

#include "stm32f10x.h"
#include "vex_hw.h"
#include "spi.h"

#if defined(USE_STDPERIPH_DRIVER)
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_misc.h"
#include "core_cm3.h"
#endif

#include "init.h"

volatile uint16_t adc_buffer[ADC_NUM];

void gpio_init(void)
{
	// enable gpio clock (+ AFIO for good measure)
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN
	             | RCC_APB2ENR_IOPAEN
	             | RCC_APB2ENR_IOPBEN
	             | RCC_APB2ENR_IOPCEN
	             | RCC_APB2ENR_IOPDEN
	             | RCC_APB2ENR_IOPEEN
	             | RCC_APB2ENR_IOPFEN
	             | RCC_APB2ENR_IOPGEN;

	// Set all gpios to "analog", aka diabled
	GPIOA->CRL = GPIOB->CRL = GPIOC->CRL = GPIOD->CRL
	           = GPIOE->CRL = GPIOF->CRL = GPIOG->CRL = 0;
	GPIOA->CRH = GPIOB->CRH = GPIOC->CRH = GPIOD->CRH
	           = GPIOE->CRH = GPIOF->CRH = GPIOG->CRH = 0;
}

void adc_init(void) {
	// ADCCLK(max 14Mhz)
	// XXX: IFI overclocked the ADC to 18MHz.
	//  PCLK2 /6 = 12 + 2/3 MHz
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | RCC_CFGR_ADCPRE_DIV6;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	// Enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	ADC_DeInit(ADC1);

	/*---- DMA1 channel1 configuration ----*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) &ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32) &adc_buffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_NUM;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_NUM;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2,  3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  4, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 7, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 8, ADC_SampleTime_239Cycles5);

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void nvic_init(void) {
	/* Place NVIC vector table at address 0 in code */
	SCB->VTOR = 0;

#if 0
	/* place NVIC vector table at address 0 in sram */
	SCB->VTOR = SCB_VTOR_TBLBASE;
#endif
}

#ifdef  USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\n", file, line) */
	printf("error %s:%d - assert failed\n", file, line);

	/* Infinite loop */
	for(;;){
	}
}
#endif
