#include "UART.h"


Serial::Serial (USART_TypeDef *_USARTx, int _baudRate): baudRate(_baudRate), USARTx(_USARTx)
{
	
	
	//delay(100);

	switch ((uint32_t)USARTx)
	{
	case USART1_BASE:
	RCC->APB2ENR  |= RCC_APB2ENR_USART1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
    NVIC_EnableIRQ(IRQn_Type::USART1_IRQn);
		break;
	case USART2_BASE:
	RCC->APB1ENR1  |= RCC_APB1ENR1_USART2EN;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART2RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART2RST;
    NVIC_EnableIRQ(IRQn_Type::USART2_IRQn);
		break;
	case USART3_BASE:
	RCC->APB1ENR1  |= RCC_APB1ENR1_USART3EN;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART3RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART3RST;
    NVIC_EnableIRQ(IRQn_Type::USART3_IRQn);
		break;
	case UART4_BASE:
	RCC->APB1ENR1  |= RCC_APB1ENR1_UART4EN;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_UART4RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_UART4RST;
    NVIC_EnableIRQ(IRQn_Type::UART4_IRQn);
		break;
	case UART5_BASE:
	RCC->APB1ENR1  |= RCC_APB1ENR1_UART5EN;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_UART5RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_UART5RST;
    NVIC_EnableIRQ(IRQn_Type::UART5_IRQn);
		break;
	default:
		break;
	}




    USARTx->BRR =  (AHB2clockFreq/baudRate);
	USARTx->CR1 |= ( USART_CR1_UE| USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE);
}

void Serial::sendChar (char _char)
{
	USARTx->TDR = _char;
	while(!(USARTx->ISR & USART_ISR_TC));
}

void Serial::print(const char* _string)
{
	while (*_string)
	{
		sendChar(*_string);
		_string++;
	}
}

void Serial::print(int _number)
{ 
	char charTab[11]={};
	typeConverter::IntToString(_number, charTab);
	print(charTab);
}

void Serial::print(double _number, uint8_t _percision)
{
	char charTab[20]={};
	typeConverter::fractionalToString(_number, charTab, _percision);
	print(charTab);
}

void Serial::println(const char* _string)
{
    print(_string);
    print("\n");
}

void Serial::println(int _number)
{
    print(_number);
    print("\n");
}

void Serial::println(double _number, uint8_t _percision)
{
	print(_number, _percision);
	print("\n");
}

char Serial::readChar ()
{
	char tmpChar;
	buffer.read(&tmpChar);
	return tmpChar;
}

bool Serial::readTillEOL (char *_line) // returns 1 if EOL is found, 0 if not, when EOL is found save read data to _line
{
	if (uart.available())
	{		
		
		if(count==0)
		{
				for (int i=0; i<=elementsToFlush; i++)
				{
					char* tempPtr =_line+i;
					*tempPtr = 0;				
				}							
		}		

		char c = uart.readChar();
		if (c == '\n')
		{
			//GPIOA->ODR ^= GPIO_ODR_OD5;
			_line+=count;
			*_line=0;
			elementsToFlush=count;
			count=0;
			return 1;
		}
		else
		{
			_line+=count;
			*_line=c;
			count++;
		}		
	}
	return 0;	
}

bool Serial::available ()
{
	return !buffer.empty();
}



void Serial::interrupt()
{
		if (USARTx->ISR & USART_ISR_RXNE)
		{			
			char c = USARTx->RDR;
			buffer.write(c);
		}
}

void Serial::bufferFlush()
{
	buffer.flush();
}





