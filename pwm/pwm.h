#ifndef PWM_H
#define PWM_H
#include "stm32l4xx.h"
#include "UART.h"
#include "board.h"
typedef struct
{
    TIM_TypeDef* tim;
    uint32_t frequency;  
    uint32_t autoReloadValue;
    uint32_t halfAutoReloadValue;
}pwm_t;

static inline void getAutoReloadValue (pwm_t* pwm)
{
    pwm->autoReloadValue = pwm->tim->ARR;
    pwm->halfAutoReloadValue = pwm->autoReloadValue/2;
}

static inline void pwm_init (pwm_t* pwm)
{
    //NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  
	pwm->tim->CCMR1 |= 0b110<< TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE ; // PWM mode 1 channel 1
	pwm->tim->CCMR1 |= 0b110<< TIM_CCMR1_OC2M_Pos | TIM_CCMR1_OC2PE ; // PWM mode 1 channel 2
	pwm->tim->CCMR2 |= 0b110<< TIM_CCMR2_OC3M_Pos | TIM_CCMR2_OC3PE ; // PWM mode 1 channel 3
	pwm->tim->CCMR2 |= 0b0110<< TIM_CCMR2_OC4M_Pos;
	pwm->tim->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE;
	pwm->tim->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;
	pwm->tim->CCER |= TIM_CCER_CC3E | TIM_CCER_CC3NE;
	//TIM1->CR2 |= 0b010<<TIM_CR2_MMS_Pos;   // Set TRGO on Update Event
	pwm->tim->PSC = 0;  // tim_clk = cpu_clk
    (160000000/pwm->frequency/2<=0xFFFF) ? (pwm->tim->ARR = 160000000/pwm->frequency/2) : (pwm->tim->ARR = 0xFFFF);
	pwm->tim->CCR1 = 0;
	pwm->tim->CCR2 = 0;
	pwm->tim->CCR3 = 0;
	pwm->tim->CCR4 = 1000;
	pwm->tim ->DIER |=  TIM_DIER_UIE; // interrupt enable
	pwm->tim->CR2 = 0b0111<<TIM_CR2_MMS_Pos;   // Set TRGO on Compare 4
	pwm->tim->CR1 |= TIM_CR1_ARPE | 0b01<<TIM_CR1_CMS_Pos;
	pwm->tim->EGR |= TIM_EGR_UG;
	pwm->tim->BDTR |= TIM_BDTR_MOE | 0b00100000<<TIM_BDTR_DTG_Pos;
	pwm->tim->CR1 |= TIM_CR1_CEN;

    getAutoReloadValue (pwm);
	pwm->tim->CCR4= pwm->halfAutoReloadValue;
}


static inline void pwm_set3Phase_pu(pwm_t* pwm, float* duty3P_pu) //-1 to 1 corresponds to 0 to 100% duty cycle
{
    pwm->tim->CCR1 = pwm->halfAutoReloadValue * duty3P_pu[1] + pwm->halfAutoReloadValue;
	pwm->tim->CCR2 = pwm->halfAutoReloadValue * duty3P_pu[0] + pwm->halfAutoReloadValue;
	pwm->tim->CCR3 = pwm->halfAutoReloadValue * duty3P_pu[2] + pwm->halfAutoReloadValue;
	
    // uint32_t Ua = Ualpha + 500;
	// uint32_t Ub = -0.5f * Ualpha  + M_SQRT3_2 * Ubeta + 500;
	// uint32_t Uc = -0.5f * Ualpha - M_SQRT3_2 * Ubeta + 500;
}
static inline void pwmOutOff(pwm_t* pwm)
{
	pwm->tim->CCR1 = 0;
	pwm->tim->CCR2 = 0;
	pwm->tim->CCR3 = 0;
	pwm->tim->CCMR1 &= ~(0b1111<< TIM_CCMR1_OC1M_Pos);
	pwm->tim->CCMR1 &= ~(0b1111<< TIM_CCMR1_OC2M_Pos);
	pwm->tim->CCMR2 &= ~(0b1111<< TIM_CCMR2_OC3M_Pos);
}
static inline void pwmOutOn(pwm_t* pwm)
{
	pwm->tim->CCR1 = 0;
	pwm->tim->CCR2 = 0;
	pwm->tim->CCR3 = 0;
	pwm->tim->CCMR1 |= 0b110<< TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE ; // PWM mode 1 channel 1
	pwm->tim->CCMR1 |= 0b110<< TIM_CCMR1_OC2M_Pos | TIM_CCMR1_OC2PE ; // PWM mode 1 channel 2
	pwm->tim->CCMR2 |= 0b110<< TIM_CCMR2_OC3M_Pos | TIM_CCMR2_OC3PE ; // PWM mode 1 channel 3
}

#endif // PWM_H