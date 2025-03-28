#ifndef ADC_H
#define ADC_H
#include "stm32l4xx.h"


typedef uint32_t adcChannelsCount_t;
typedef uint32_t adcChannelNumbers_t;

typedef enum 
{
    adcOk,
    channelCountErr,
    channelNumberErr,
    wrongADCmodule
}adcError_t;

typedef enum 
{
    noEXTtrigger = 0,
    risingEdge,
    fallingEdge,
    bothEdges

}adcEXTTriggerEdge_t;

typedef struct
{
    ADC_TypeDef* adc;
    adcChannelsCount_t channelsCount;
    adcChannelNumbers_t* channels;
    adcEXTTriggerEdge_t triggerEdge;
    uint32_t externalTriggerEvent;
    adcError_t adcError;
}adcChannel_t;


#ifdef __cplusplus
extern "C" {
#endif
adcError_t adc_init(adcChannel_t* aIns);
#ifdef __cplusplus
}
#endif

static inline uint32_t adc_getDataReg(adcChannel_t* aIns) //simplest method to get adc data, works only for single channel adc
{
    return aIns->adc->DR;
}

#endif // ADC_H