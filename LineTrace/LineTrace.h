#ifndef LINE_TRACE_H
#define LINE_TRACE_H

#include "mbed.h"
#include "Motor.h"

enum class Color
{
    Brack = 0,
    White = 1,
};

class LineTrace
{
public:
    enum TracePosition
    {
        Right = 0,
        Center = 1,
        Left = 2,
    };

    LineTrace(PinName left, PinName center, PinName right);

    void read();

    State get_left_state();
    float get_left_duty_cycle();
    State get_right_state();
    float get_right_duty_cycle();

private:
    AnalogIn _right;
    AnalogIn _center;
    AnalogIn _left;

    Color color[3];
    Motor motor[3]; // TracePositionの列挙型を使いたいから

    Color get_color(AnalogIn pin);

    static constexpr float threshold = 0.6f;
};

#endif
