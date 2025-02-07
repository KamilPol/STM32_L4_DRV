#include "adc.h"

adcError_t adc_init(adcChannel_t* aIns)
{
    switch ((uint32_t)aIns->adc)
    {
        case ADC1_BASE:
            RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;
            ADC12_COMMON->CCR |= (0b11 << ADC_CCR_CKMODE_Pos | ADC_CCR_VREFEN);	// Set ADC clock to HCLK/2 and enable VREFINT
        break;
        case ADC2_BASE:
            RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;
            ADC12_COMMON->CCR |= (0b11 << ADC_CCR_CKMODE_Pos | ADC_CCR_VREFEN);	// Set ADC clock to HCLK/2 and enable VREFINT
        break;
        case ADC3_BASE:
            RCC->AHB2ENR |= RCC_AHB2ENR_ADC345EN;
            ADC345_COMMON->CCR |= (0b11 << ADC_CCR_CKMODE_Pos | ADC_CCR_VREFEN);
        break;
        case ADC4_BASE:
            RCC->AHB2ENR |= RCC_AHB2ENR_ADC345EN;
            ADC345_COMMON->CCR |= (0b11 << ADC_CCR_CKMODE_Pos | ADC_CCR_VREFEN);
        break;
        case ADC5_BASE:
            RCC->AHB2ENR |= RCC_AHB2ENR_ADC345EN;
            ADC345_COMMON->CCR |= (0b11 << ADC_CCR_CKMODE_Pos | ADC_CCR_VREFEN);
        break;
        default:
            return wrongADCmodule;
        break;
    }

	aIns->adc->CR |= ADC_CR_ADSTP;
	while((aIns->adc->ISR & ADC_ISR_ADRDY));	
	aIns->adc->CR =0;
    aIns->adc->CFGR = 0x80000000;
	aIns->adc->CFGR = ADC_CFGR_OVRMOD | aIns->triggerEdge<<ADC_CFGR_EXTEN_Pos | aIns->externalTriggerEvent<<ADC_CFGR_EXTSEL_Pos | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG; // Set overrun mode, external trigger rising edge, TIM1_TRGO as trigger, DMA enable, DMA circular mode
	if (aIns->triggerEdge == noEXTtrigger)
    {
        aIns->adc->CFGR |= ADC_CFGR_CONT;
    }
    aIns->adc->CR |= ADC_CR_ADVREGEN;	
	aIns->adc->CR |= ADC_CR_ADCAL;
	while(aIns->adc->CR & ADC_CR_ADCAL);

    if (aIns->channelsCount>16)
       return channelCountErr;
	aIns->adc->SQR1 |= (aIns->channelsCount-1)<<ADC_SQR1_L_Pos; 

    for (uint32_t i=0; i<aIns->channelsCount; i++)
    {
        if (*(aIns->channels+i)>16 || *(aIns->channels+i)<1)
            return channelNumberErr;

        if (i<=4)
        {
            aIns->adc->SQR1 |= *(aIns->channels+i) << (6+i*6); 
        }else if(i<=9)
        {
            aIns->adc->SQR2 |= *(aIns->channels+i) << ((i-5)*6);
        }else if (i<=14)
        {
            aIns->adc->SQR3 |= *(aIns->channels+i) << ((i-10)*6);     
        }else 
        {
            aIns->adc->SQR4 |= *(aIns->channels+i) << ((i-15)*6);
        }
    }
	aIns->adc->CR |= ADC_CR_ADEN;
	while(!(aIns->adc->ISR & ADC_ISR_ADRDY));
	aIns->adc->CR |= ADC_CR_ADSTART;
    
    return adcOk;  
}