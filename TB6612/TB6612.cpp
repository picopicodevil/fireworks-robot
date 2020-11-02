#include "TB6612.h"
#include "Motor.h"
#include "mbed.h"

TB6612::TB6612(PinName AIN1, PinName AIN2, PinName PWMA, PinName BIN1, PinName BIN2, PinName PWMB, PinName STBY)
    : _AIN1(AIN1), _AIN2(AIN2), _PWMA(PWMA), _BIN1(BIN1), _BIN2(BIN2), _PWMB(PWMB), _STBY(STBY)
{
    _PWMA.period(0.0001f);
    _PWMB.period(0.0001f);
}

int TB6612::set(Motor &motor, int number)
{
    set_state(motor.get_state(), number);
    set_duty_cycle(motor.get_duty_cycle(), number);

    return 1;
}

void TB6612::standby(int value)
{
    if (0 <= value && value <= 1)
    {
        _STBY = value;
    }
}

void TB6612::set_state(State type, int number)
{
    if (number == 0)
    {
        switch (type)
        {
        case State::Free:
            _AIN1 = 0, _AIN2 = 0;
            break;
        case State::CW:
            _AIN1 = 1, _AIN2 = 0;
            break;
        case State::CCW:
            _AIN1 = 0, _AIN2 = 1;
            break;
        case State::Brake:
            _AIN1 = 1, _AIN2 = 1;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (type)
        {
        case State::Free:
            _BIN1 = 0, _BIN2 = 0;
            break;
        case State::CW:
            _BIN1 = 1, _BIN2 = 0;
            break;
        case State::CCW:
            _BIN1 = 0, _BIN2 = 1;
            break;
        case State::Brake:
            _BIN1 = 1, _BIN2 = 1;
            break;
        default:
            break;
        }
    }
}

void TB6612::set_duty_cycle(float value, int number)
{
    if (number == 0)
    {
        _PWMA = value;
    }
    else
    {
        _PWMB = value;
    }
}
