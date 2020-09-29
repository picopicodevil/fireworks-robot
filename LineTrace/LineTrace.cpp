#include "LineTrace.h"
#include "mbed.h"
#include "Motor.h"

LineTrace::LineTrace(PinName left, PinName center, PinName right)
    : _left(left), _center(center), _right(right)
{
}

void LineTrace::read()
{
    color[TracePosition::Left] = get_color(_left);
    color[TracePosition::Center] = get_color(_center);
    color[TracePosition::Right] = get_color(_right);

    if ((color[TracePosition::Left] == color[TracePosition::Center]) && (color[TracePosition::Center] == color[TracePosition::Left]) && (color[TracePosition::Right] == Color::Brack))
    {
        motor[Left].set_state(State::Brake);
        motor[Left].set_duty_cycle(0.00f);

        motor[Right].set_state(State::Brake);
        motor[Right].set_duty_cycle(0.00f);
    }
    else if (color[TracePosition::Center] == Color::Brack)
    {
        motor[Left].set_state(State::CW);
        motor[Left].set_duty_cycle(1.00f);

        motor[Right].set_state(State::CW);
        motor[Right].set_duty_cycle(0.50f);
    }
    else if (color[TracePosition::Center] == Color::Brack)
    {
        motor[Left].set_state(State::CW);
        motor[Left].set_duty_cycle(0.50f);

        motor[Right].set_state(State::CW);
        motor[Right].set_duty_cycle(1.00f);
    }
}

State LineTrace::get_left_state()
{
    return motor[Left].get_state();
}

float LineTrace::get_left_duty_cycle()
{
    return motor[Left].get_duty_cycle();
}

State LineTrace::get_right_state()
{
    return motor[Right].get_state();
}

float LineTrace::get_right_duty_cycle()
{
    return motor[Right].get_duty_cycle();
}

Color LineTrace::get_color(AnalogIn pin)
{
    float value = pin.read();

    if (value >= threshold)
    {
        return Color::White;
    }
    else
    {
        return Color::Brack;
    }
}
