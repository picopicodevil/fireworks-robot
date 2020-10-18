#include "mbed.h"
#include "TB6612.h"
#include "LineTrace.h"
#include "IrRemoteControl.h"
#include "UniqueValue.h"
#include <chrono>
#include <cstring>

#define TURN_INTERVAL_TIME 7.0f

int move2value(char *move)
{
    if (strcmp(move, "Stay") == 0)
        return 0;
    else if (strcmp(move, "Up") == 0)
        return 1;
    else if (strcmp(move, "Left") == 0)
        return 2;
    else if (strcmp(move, "Down") == 0)
        return 3;
    else if (strcmp(move, "Right") == 0)
        return 4;

    return -1;
}

void setRgbLed(DigitalOut *rgb_led, int *color)
{
    for (int i = 0; i < 3; i++)
    {
        rgb_led[i].write(color[i] > 127 ? 0 : 1);
    }
}


// main() runs in its own thread in the OS
int main()
{
    // DigitalIn pin(PF_0);
    char move[MOVE_LENGTH][6] = REPLACE_MOVE;
    int color[COLOR_LENGTH][3] = REPLACE_COLOR;

    IrRemoteControl controller(PF_0);
    TB6612 tb6612(PA_5, PA_4, PA_8, PA_7, PB_0, PB_1, PA_6);
    LineTrace line_trace(PA_0, PA_1, PA_3);

    DigitalOut led1(PB_6);
    DigitalOut led2(PB_7);

    DigitalOut rgb_led[] = {
        DigitalOut(PB_5),
        DigitalOut(PB_3),
        DigitalOut(PB_4),
    };

    for (int i = 0; i < 10; i++)
    {
        led1 = !led1;
        led2 = !led2;
        ThisThread::sleep_for(100ms);
    }

    while (controller.get_reader_code() <= 5)
    {
        led1 = !led1;
        led2 = !led2;
        ThisThread::sleep_for(10ms);
    }

    controller.reset_reader_code();

    tb6612.standby(1);



    // 各ターンの動作
    for (int i = 0; i < MOVE_LENGTH; i++)
    {
        Timer t;
        t.start();

        setRgbLed(rgb_led, color[i]);

        Motor motor[2];

        if (controller.get_reader_code() <= 5)
            break;

        // @TODO ここで直進、後進、右折、左折のコードを記述する
        // int result = line_trace.read();

        int move_value = move2value(move[i]);

        switch (move_value)
        {
        case 2:
            // // TurnRight
            motor[0].set_state(State::CCW);
            motor[0].set_duty_cycle(0.50f);
            motor[1].set_state(State::CCW);
            motor[1].set_duty_cycle(0.20f);

            tb6612.set(motor[0], 0);
            tb6612.set(motor[1], 1);

            ThisThread::sleep_for(700ms);
            break;

        case 4:
            // turn left
            motor[0].set_state(State::Brake);
            motor[0].set_duty_cycle(0.00f);
            motor[1].set_state(State::CW);
            motor[1].set_duty_cycle(0.50f);

            tb6612.set(motor[0], 0);
            tb6612.set(motor[1], 1);

            ThisThread::sleep_for(700ms);
            break;

        default:
            break;
        }

        while (true)
        {
            if (controller.get_reader_code() <= 5)
                break;

            if (t.read() > TURN_INTERVAL_TIME)
                break;
            
            motor[0].set_state(line_trace.get_left_state());
            motor[0].set_duty_cycle(line_trace.get_left_duty_cycle());
            motor[1].set_state(line_trace.get_right_state());
            motor[1].set_duty_cycle(line_trace.get_right_duty_cycle());

            tb6612.set(motor[0], 0);
            tb6612.set(motor[1], 1);

            led1 = !led1;
            led2 = !led2;
            ThisThread::sleep_for(10ms);
        }

        t.stop();
        t.start();
    }

    tb6612.standby(0);
    sleep();
}
