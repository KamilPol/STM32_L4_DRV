#include "clock_manager.h"
#include "stm32g4xx.h"
#include "board.h"
volatile uint32_t milis;
uint32_t ClockManager::coreClock = ClockManager::hsiClock;
uint32_t ClockManager::ahbClock = ClockManager::hsiClock;
uint32_t ClockManager::apbClock = ClockManager::hsiClock;
uint32_t ClockManager::timerClock = ClockManager::hsiClock;
pllClk ClockManager::pllClock;


extern "C"
{
    void SysTick_Handler()
	{
		milis++;
		TIM6->EGR = TIM_EGR_UG;
	}
}

void ClockManager::hseInit()
{
	if (!(RCC->CR & RCC_CR_HSERDY))
	{
		RCC->CR |= RCC_CR_HSEON;
		while(!(RCC->CR & RCC_CR_HSERDY));
	}
}

void ClockManager::internalFreqUpdate(uint32_t _targetFreq)
{
	uint32_t flashDiv = 35000000;

	if (_targetFreq < 26000000)
	{
		
		PWR->CR1 = 0x2 << PWR_CR1_VOS_Pos;
		while(PWR->SR2 & PWR_SR2_VOSF);
		flashDiv = 12000000;
	}
	else if (_targetFreq < 150000000)
	{
		
		PWR->CR1 = 0x1 << PWR_CR1_VOS_Pos;
		while(PWR->SR2 & PWR_SR2_VOSF);
		PWR->CR5 = PWR_CR5_R1MODE;
		flashDiv = 30000000;
	}
	else
	{
		
		PWR->CR1 = 0x1 << PWR_CR1_VOS_Pos;
		while(PWR->SR2 & PWR_SR2_VOSF);
		PWR->CR5 = 0;
		flashDiv = 34000000;
	}

	ahbClock = _targetFreq;

	uint32_t waitStates = ahbClock / flashDiv < 15 ? ahbClock / flashDiv : 15;
	uint32_t flashConf = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | waitStates;
	
	FLASH->ACR = flashConf;
	while(FLASH->ACR != flashConf)
		;


	apbClock = ahbClock;
	timerClock = apbClock;
}

void ClockManager::setSysClk(const clkSrc _src)
{
	uint32_t targetFreq = 0;
	switch(_src)
	{
	case clkSrc::hsi:
		targetFreq = hsiClock;
		break;
	case clkSrc::hse:
		targetFreq = hseClock;
		break;
	case clkSrc::pll:
		targetFreq = pllClock.r;
		break;
	}
	

	if(targetFreq > coreClock)
		internalFreqUpdate(targetFreq);

	RCC->CFGR = (RCC->CFGR & ~0x7) | static_cast<uint32_t>(_src);
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != static_cast<uint32_t>(_src) << RCC_CFGR_SWS_Pos);

	

	if(targetFreq < coreClock)
		internalFreqUpdate(targetFreq);
	coreClock = targetFreq;
}

void ClockManager::pllCfg([[maybe_unused]] int _pllNum, int _targetFreq, int _p, pllDiv _q, pllDiv _r)
{
	uint32_t sourceFreq;

	RCC->CR &= ~RCC_CR_PLLON;
	while (RCC->CR & RCC_CR_PLLRDY);
	if (RCC->CR & RCC_CR_HSERDY)
	{
		sourceFreq = hseClock;
		RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE;
	}
	else
		sourceFreq = RCC_PLLCFGR_PLLSRC_HSI;

	int m = sourceFreq / 4000000; //4 MHz input frequency

	float targetDivider = ((float)_targetFreq) / (sourceFreq / m);
	int plln = (int)(targetDivider + 0.5f);

	RCC->PLLCFGR |= (m - 1) << RCC_PLLCFGR_PLLM_Pos;

	RCC->PLLCFGR |= _p << RCC_PLLCFGR_PLLPDIV_Pos
		| static_cast<uint32_t>(_q) << RCC_PLLCFGR_PLLQ_Pos
		| static_cast<uint32_t>(_r) << RCC_PLLCFGR_PLLR_Pos
		| plln << RCC_PLLCFGR_PLLN_Pos;

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLREN;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));

	pllClock = updatePll();
	//logger.info("PLL", _pllNum, " conf; fin = ", sourceFreq, ", m = ", m, ", n = ", plln, "\r\n");
	// uart.print("PLL");
	// uart.print(_pllNum);
	// uart.print(" conf; fin = ");
	// uart.print((int)sourceFreq);
	// uart.print(", m = ");
	// uart.print(m);
	// uart.print(", n = ");
	// uart.print(plln);
	// uart.print("\r\n");
	
}

pllClk ClockManager::updatePll()
{
	pllClk pllClocks = {0, 0, 0, 0};
	uint32_t sourceFreq;

	if(!(RCC->CR & RCC_CR_PLLRDY))
		return pllClocks;

	if (RCC->CR & RCC_CR_HSERDY)
		sourceFreq = hseClock;
	else
		sourceFreq = hsiClock;

    float finalMult = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;
    finalMult /= ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos) + 1;
    pllClocks.internal = sourceFreq * finalMult;
    pllClocks.p = pllClocks.internal / ((RCC->PLLCFGR & RCC_PLLCFGR_PLLPDIV_Msk) >> RCC_PLLCFGR_PLLPDIV_Pos);
    pllClocks.q = pllClocks.internal / ((1 + ((RCC->PLLCFGR & RCC_PLLCFGR_PLLQ_Msk) >> RCC_PLLCFGR_PLLQ_Pos)) * 2);
    pllClocks.r = pllClocks.internal / ((1 + ((RCC->PLLCFGR & RCC_PLLCFGR_PLLR_Msk) >> RCC_PLLCFGR_PLLR_Pos)) * 2);

	// logger.info("PLL conf; fin = ", sourceFreq,
	// ", int = ", pllClocks.internal,
	// ", p = ", pllClocks.p,
	// ", q = ", pllClocks.q,
	// ", r = ", pllClocks.r,
	// "\r\n");
	// uart.print("PLL conf; fin = ");
	// uart.print((int)sourceFreq);
	// uart.print(", int = ");
	// uart.print((int)pllClocks.internal);
	// uart.print(", p = ");
	// uart.print((int)pllClocks.p);
	// uart.print(", q = ");
	// uart.print((int)pllClocks.q);
	// uart.print(", r = ");
	// uart.print((int)pllClocks.r);
	// uart.print("\r\n");

    return pllClocks;
}

void ClockManager::initTick()
{
	enableClk(RCC->APB1ENR1, RCC_APB1ENR1_TIM6EN);
	int prescaler = timerClock / 1000000;

	TIM6->PSC = prescaler - 1;
	TIM6->ARR = 0x0FFF;
	TIM6->CR1 = TIM_CR1_CEN;
    SysTick_Config(coreClock / 1000);
}

uint64_t ClockManager::micros()
{
    __disable_irq();
    uint64_t result = uint64_t(milis) * 1000 + TIM6->CNT;
    __enable_irq();
    return result;
}

void ClockManager::enableClk(volatile uint32_t &_reg, uint32_t _bits)
{
	_reg |= _bits;
	while((_reg & _bits) != _bits)
		;
}

void ClockManager::clockSummary()
{
   // uart.print("\r\ncpu = ");
    //uart.println((float)coreClock / 1000000);

    uart.print("\r\ncpu = ");
    uart.print((float)coreClock / 1000000);

    uart.print(" MHz\r\nahb = ");
    uart.print((float)ahbClock / 1000000);
    uart.print(" MHz\r\napb = ");
    uart.print((float)apbClock / 1000000);
    uart.print(" MHz\r\ntim = ");
    uart.print((float)timerClock / 1000000);
    uart.print(" MHz\r\n");

	uart.print("\nPLL\r\nkernel: ");
	uart.print(float(pllClock.internal) / 1000000);
	uart.print(" MHz\r\n    p: ");
	uart.print(float(pllClock.p) / 1000000);
	uart.print(" MHz\r\n    q: ");
	uart.print(float(pllClock.q) / 1000000);
	uart.print(" MHz\r\n    r: ");
	uart.print(float(pllClock.r) / 1000000);
	uart.print(" MHz\r\n");
}