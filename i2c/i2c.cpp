#include  "i2c.h"


I2C::I2C(I2C_TypeDef* _I2Cx): I2Cx(_I2Cx)
{
	switch (uint32_t(I2Cx)) // sprawdzić czy działa dla innych interfejsow niz i2c2
	{
		case I2C1_BASE:
			RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
			break;
		case I2C2_BASE:
			RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;
			break;
		case I2C3_BASE:
			RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;
			break;
	}

	
	I2Cx->CR1 &= ~I2C_CR1_PE; 
	I2Cx->TIMINGR = 					0x0070276B; // Set I2C2 timing register value
	I2Cx->CR1 |= I2C_CR1_PE; // Enable I2C2 peripheral
}

void I2C::sendByte(uint8_t *_byte, uint8_t _address)
{
	I2Cx->CR2 &= ~(0x1FFFF << 10);
	I2Cx->CR2 |= I2C_CR2_AUTOEND;
	I2Cx -> CR2 = _address<<1;
	I2Cx ->CR2 |= 1<<I2C_CR2_NBYTES_Pos; 
	I2Cx -> CR2 |= I2C_CR2_START; 
	while(!(I2Cx->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF))); // check if TXDR is empty
	I2Cx->TXDR = *_byte; // Send data to I2C
	while (!(I2Cx->ISR & I2C_ISR_TC));
	// I2Cx->CR2 |= I2C_CR2_STOP; // Generate stop condition
}

void I2C::sendBytes(uint8_t *_bytes, uint8_t _lenght, uint8_t _address) // coś działa chujowo
{
	I2Cx->CR2 &= ~(0x1FFFF << 10);
	I2Cx->CR2 |= I2C_CR2_AUTOEND;
	I2Cx -> CR2 = _address<<1;
	I2Cx ->CR2 |= _lenght<<I2C_CR2_NBYTES_Pos; // Set number of bytes to send
	I2Cx -> CR2 |= I2C_CR2_START; 
	
 
	for(int i=0; i<_lenght; i++) 
	{
		while(!(I2Cx->ISR & I2C_ISR_TXIS));  // check if TXDR is empty
		I2Cx->TXDR = *_bytes; // Send data to I2C
		_bytes++;
		
		
	}
	while (!(I2Cx->ISR & I2C_ISR_STOPF));
	//I2Cx->CR2 |= I2C_CR2_STOP; // Generate stop condition //nie trzxa
}

uint8_t I2C::recieveByte (uint8_t _address)
{
	I2Cx -> CR2 = _address<<1;
	I2Cx -> CR2 |= 1 << I2C_CR2_NBYTES_Pos; 
	I2Cx -> CR2 |= I2C_CR2_RD_WRN | I2C_CR2_START;
	while(!(I2Cx->ISR & I2C_ISR_RXNE)); 
	uint8_t recievedByte = I2Cx->RXDR;
	return recievedByte;
}