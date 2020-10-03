#include "mbed.h"
#include "TB6612.h"
#include "LineTrace.h"

// main() runs in its own thread in the OS
int main()
{
    TB6612 tb6612(PA_5, PA_4, PA_8, PA_7, PB_0, PB_1, PA_6);
    // LineTrace line_trace(PA_0, PA_1, PA_3);

    AnalogIn ain1(PA_0);
    AnalogIn ain2(PA_1);
    AnalogIn ain3(PA_3);

    DigitalOut led1(PB_6);
    DigitalOut led2(PB_7);

    tb6612.standby(1);

    for (int i = 0; i < 30; i++)
    {
        led1 = !led1;
        led2 = !led2;
        ThisThread::sleep_for(100ms);
    }

    while (true)
    {
        float value = ain3.read();

        if (value <= 0.625f)
        {
            led2 = 0, led1 = 0;
        }
        else if (value <= 0.75f)
        {
            led2 = 0, led1 = 1;
        }
        else if (value <= 0.875f)
        {
            led2 = 1, led1 = 0;
        }
        else if (value <= 1.0f)
        {
            led2 = 1, led1 = 1;
        }

        wait_us(1000000);

        // Motor motor[2];

        // // line_trace.read();

        // // motor[0].set_state(line_trace.get_left_state());
        // // motor[0].set_duty_cycle(line_trace.get_left_duty_cycle());
        // // motor[1].set_state(line_trace.get_right_state());
        // // motor[1].set_duty_cycle(line_trace.get_right_duty_cycle());

        // motor[0].set_state(State::Brake);
        // motor[0].set_duty_cycle(0.00f);
        // motor[1].set_state(State::Brake);
        // motor[1].set_duty_cycle(0.00f);

        // tb6612.set(motor[0], 0);
        // tb6612.set(motor[1], 1);

        // led1 = !led1;
        // led2 = !led2;
        // ThisThread::sleep_for(2s);

        // motor[0].set_state(State::CW);
        // motor[0].set_duty_cycle(0.50f);
        // motor[1].set_state(State::CW);
        // motor[1].set_duty_cycle(0.50f);

        // tb6612.set(motor[0], 0);
        // tb6612.set(motor[1], 1);

        // led1 = !led1;
        // led2 = !led2;
        // ThisThread::sleep_for(2s);
    }
}
