#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H
#include <stdint.h>

extern volatile uint32_t milis;

enum class clkSrc
{
	hsi = 1,
	hse = 2,
	pll = 3
};

struct pllClk
{
	uint32_t internal;
	uint32_t p;
	uint32_t q;
	uint32_t r;
};

class ClockManager
{
private:
	static void pllSelect(int _pllNum, uint32_t &offset);
	static pllClk updatePll();
	static void internalFreqUpdate(uint32_t _targetFreq);

public:
	enum class pllDiv
	{
		div2,
		div4,
		div6,
		div8
	};

	static const uint32_t hseClock;
	static const uint32_t hsiClock = 16000000;
	static const uint32_t hsi48Clock = 48000000;
	static const uint32_t lseClock;
	static const uint32_t lsiClock = 32000;
	static const uint32_t i2sClock;

	static uint32_t coreClock;
	static uint32_t ahbClock;
	static uint32_t apbClock;
	static uint32_t timerClock;
	static pllClk pllClock;

	static void hseInit();
	static void pllCfg(int _pllNum, int _targetFreq, int _p, pllDiv _q, pllDiv _r);
	static void setSysClk(const clkSrc _src);
    static void initTick();
	static void enableClk(volatile uint32_t &_reg, uint32_t _bits);

	static uint64_t micros();
	static void clockSummary();
};

#endif //CLOCKMANAGER_H