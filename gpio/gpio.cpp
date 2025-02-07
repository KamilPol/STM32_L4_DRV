#include "gpio.h"

GPIO::GPIO (GPIO_TypeDef* _GPIOx, uint8_t _pin, GPIOmode _mode, GPIOtype _type, GPIOspeed _speed, GPIOpull _pull, uint8_t _AFnumber):
GPIOx(_GPIOx), pin(_pin)
{
	switch ((uint32_t) _GPIOx)
	{
		case GPIOA_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
			break;
		case GPIOB_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
			break;
		case GPIOC_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
			break;
		case GPIOD_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
			break;
		case GPIOE_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
			break;
		case GPIOF_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
			break;
		case GPIOG_BASE:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
			break;
		default: 
		break;
	}

	GPIOx->MODER &= ~(0b11 << (pin*2));
	GPIOx->MODER |= (static_cast<int>(_mode) << (pin*2));
	GPIOx->OTYPER &= ~(1 << pin);
	GPIOx->OTYPER |= (static_cast<int>(_type) << pin);
	GPIOx->OSPEEDR &= ~(0b11 << (pin*2));
	GPIOx->OSPEEDR |= (static_cast<int>(_speed)<<(pin*2));
	GPIOx->PUPDR &= ~(0b11 << (pin*2));
	GPIOx->PUPDR |= (static_cast<int>(_pull)<<(pin*2));
	GPIOx->AFR[pin/8] &= ~(0xF << ((pin % 8)*4));
	GPIOx->AFR[pin/8] |= (_AFnumber << ((pin % 8)*4));
}
void GPIO::set()
{
	GPIOx->BSRR |= (1<<pin);
}
void GPIO::reset ()
{
	GPIOx->BSRR |= (1<<(pin+16));

}
void GPIO::toggle()
{
	GPIOx->ODR ^= (1<<pin);
}
void GPIO::setPinMode(GPIOmode _mode)
{
	GPIOx->MODER &= ~(0b11<<(pin*2));
	GPIOx->MODER |= static_cast<int>(_mode)<<(pin*2);
}
