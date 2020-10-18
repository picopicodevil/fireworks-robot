#include "mbed.h"
#include "TB6612.h"
#include "LineTrace.h"
#include "IrRemoteControl.h"
#include "UniqueValue.h"
#include <chrono>
#include <cstring>

#define TURN_INTERVAL_TIME 2.0f

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

    BufferedSerial pc(USBTX, USBRX);

    IrRemoteControl controller(PF_0);
    TB6612 tb6612(PA_5, PA_4, PA_8, PA_7, PB_0, PB_1, PA_6);
    LineTrace line_trace(PA_0, PA_1, PA_3);

    BusOut led(PB_6, PB_7);

    DigitalOut rgb_led[] = {
        DigitalOut(PB_5, 1),
        DigitalOut(PB_3, 1),
        DigitalOut(PB_4, 1),
    };

    // rgb_led[0] = 1;
    // rgb_led[1] = 0;
    // rgb_led[2] = 1;

    // int i = 0;
    // while(1){
    //     led = i++;
    //     ThisThread::sleep_for(1s);
    // }

    // while (controller.get_reader_code() >= 5)
    // {
    //     led1 = !led1;
    //     led2 = !led2;
    //     ThisThread::sleep_for(100ms);
    // }

    controller.reset_reader_code();

    tb6612.standby(1);

    // 各ターンの動作
    for (int i = 0; (i < MOVE_LENGTH) || (i < COLOR_LENGTH); i++)
    {
        Timer t;
        t.start();

        if (i < COLOR_LENGTH)
            setRgbLed(rgb_led, color[i]);

        Motor motor[2];

        led = i;

        if (controller.get_reader_code() >= 5)
            break;

        // @TODO ここで直進、後進、右折、左折のコードを記述する
        // int result = line_trace.read();

        if (i < MOVE_LENGTH)
        {
            int move_value = move2value(move[i]);

            switch (move_value)
            {
            case 0:
                // Stay
                while (1)
                {
                    if (controller.get_reader_code() >= 5)
                        break;

                    if (t.read() > TURN_INTERVAL_TIME)
                        break;

                    ThisThread::sleep_for(100ms);
                }
                break;
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
                motor[0].set_state(State::CW);
                motor[0].set_duty_cycle(0.020f);
                motor[1].set_state(State::CW);
                motor[1].set_duty_cycle(0.50f);

                tb6612.set(motor[0], 0);
                tb6612.set(motor[1], 1);

                ThisThread::sleep_for(700ms);
                break;

            default:
                break;
            }
        }

        while (true)
        {
            if (controller.get_reader_code() >= 5)
                break;

            printf("%f", t.read());

            if (t.read() > TURN_INTERVAL_TIME)
                break;

            motor[0].set_state(line_trace.get_left_state());
            motor[0].set_duty_cycle(line_trace.get_left_duty_cycle());
            motor[1].set_state(line_trace.get_right_state());
            motor[1].set_duty_cycle(line_trace.get_right_duty_cycle());

            tb6612.set(motor[0], 0);
            tb6612.set(motor[1], 1);

            ThisThread::sleep_for(10ms);
        }

        t.stop();
        t.reset();
    }

    tb6612.standby(0);
    rgb_led[0] = rgb_led[1] = rgb_led[2] = 1;
    sleep();
}
