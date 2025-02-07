#ifndef UART_H
#define UART_H

#include "stm32l4xx.h"
#include "buffer.h"
#include "board.h"
#include "typeConverter.h"
#include <stdint.h>




class Serial
{
private:
	int baudRate;
	USART_TypeDef *USARTx;
	uint8_t bufferIndex=0;
	uint8_t prevBuffIndex=0;
	uint8_t count=0;
	uint8_t elementsToFlush=0;
	Buffer<256> buffer;
	
	
public:

	Serial (USART_TypeDef *_USARTx, int _baudRate);
	void interrupt();
	void sendChar( char _char);
	void print(const char* _string);
	void print(int _number);
	void print(double _number, uint8_t percision=3);
	void println(const char* _string);
	void println(int _number);
	void println(double _number,  uint8_t percision=3);
	bool available ();
	char readChar ();
	bool readTillEOL (char *_line);
	void bufferFlush();

};




#endif // UART_H