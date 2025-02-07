#ifndef I2C_H
#define I2C_H
#include <stdint.h>
#include "stm32l4xx.h"
#include "board.h"

class I2C
{
	private:
		I2C_TypeDef* I2Cx;   
	public:
		I2C(I2C_TypeDef* _I2Cx);
		void sendByte(uint8_t *_byte, uint8_t _address);
		void sendBytes(uint8_t *_bytes, uint8_t _lenght, uint8_t _address);
		uint8_t recieveByte (uint8_t _address); 
};


#endif // I2C_H
