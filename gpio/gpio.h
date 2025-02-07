#ifndef GPIO_H
#define GPIO_H
#include "stm32l4xx.h"

enum class GPIOmode
{
    Input,
    Output,
    AF,
    Analog
};

enum class GPIOtype
{   
    PushPull,
    OpenDrain
    
};

enum class GPIOspeed
{
    Low,
    Medium,
    High,
    VeryHigh
};

enum class GPIOpull
{
    None,
    PullUp,
    PullDown
};
class GPIO
{   
GPIO_TypeDef* GPIOx;
uint8_t pin;

public:
    GPIO (GPIO_TypeDef* _GPIOx, uint8_t _pin, GPIOmode _GPIOmode = GPIOmode::Output, GPIOtype _GPIOtype=GPIOtype::PushPull, GPIOspeed _GPIOspeed = GPIOspeed::Low, GPIOpull _pull=GPIOpull::None, uint8_t _AFnumber = 0);
    void set();
    void reset ();
    void toggle();
    void setPinMode(GPIOmode _mode);



};
#endif // GPIO_H