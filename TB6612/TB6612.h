#ifndef TB6612_H
#define TB6612_H

#include "Motor.h"
#include "mbed.h"

class TB6612
{
public:
    TB6612(PinName AIN1, PinName AIN2, PinName PWMA, PinName BIN1, PinName BIN2, PinName PWMB, PinName STBY);

    int set(Motor &motor, int number);
    void standby(int value);

private:
    DigitalOut _AIN1;
    DigitalOut _AIN2;
    PwmOut _PWMA;

    DigitalOut _BIN1;
    DigitalOut _BIN2;
    PwmOut _PWMB;

    DigitalOut _STBY;

    void set_state(State type, int number);
    void set_duty_cycle(float value, int number);
};

#endif
