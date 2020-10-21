#include "mbed.h"
#include "TB6612.h"
#include "LineTrace.h"
#include "IrRemoteControl.h"
#include "UniqueValue.h"
#include <cstring>

#define TURN_INTERVAL_TIME 5.0f

int move2value(char *move);

void setRgbLed(DigitalOut *rgb_led, int *color);

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

    line_trace.set_base_color(Color::Brack);

    for (int i = 0; controller.get_reader_code() <= 5; i++)
    {
        led = i;
        ThisThread::sleep_for(10ms);
    }

    controller.reset_reader_code();

    tb6612.standby(1);

    // 各ターンの動作
    for (int turn = 0; (turn < MOVE_LENGTH) || (turn < COLOR_LENGTH); turn++)
    {
        if (controller.get_reader_code() >= 5)
            break;

        Timer t;
        t.start();

        if (turn < COLOR_LENGTH)
            setRgbLed(rgb_led, color[turn]);

        led = turn;

        if (turn < MOVE_LENGTH)
        {
            // index 0 -> 電源側, 1 -> センサコネクタ側
            Motor motor[2];

            switch (move2value(move[turn]))
            {
            case 0:
                // Stay
                while ((controller.get_reader_code() <= 5) &&
                       (std::chrono::duration<float>{t.elapsed_time()}.count() < TURN_INTERVAL_TIME))
                {
                    ThisThread::sleep_for(10ms);
                }
                break;
            case 1:
                // Up
                motor[0].set_state(State::CCW);
                motor[0].set_duty_cycle(0.40f);

                motor[1].set_state(State::CW);
                motor[1].set_duty_cycle(0.40f);

                tb6612.set(motor[0], 0);
                tb6612.set(motor[1], 1);

                ThisThread::sleep_for(200ms);
                break;
            case 2:
                // Turn left
                motor[0].set_state(State::CW);
                motor[0].set_duty_cycle(0.00f);

                motor[1].set_state(State::CW);
                motor[1].set_duty_cycle(0.50f);

                tb6612.set(motor[0], 0);
                tb6612.set(motor[1], 1);

                ThisThread::sleep_for(700ms);
                break;
            case 3:
                // Down
                while ((line_trace.read() != 0) &&
                       (controller.get_reader_code() <= 5) &&
                       (std::chrono::duration<float>{t.elapsed_time()}.count() < TURN_INTERVAL_TIME))
                {
                    motor[0].set_state(State::CW);
                    motor[0].set_duty_cycle(0.40f);

                    motor[1].set_state(State::CCW);
                    motor[1].set_duty_cycle(0.40f);

                    tb6612.set(motor[0], 0);
                    tb6612.set(motor[1], 1);

                    ThisThread::sleep_for(10ms);
                }

                motor[0].set_state(State::Brake);
                motor[0].set_duty_cycle(0.00f);

                motor[1].set_state(State::Brake);
                motor[1].set_duty_cycle(0.00f);

                tb6612.set(motor[0], 0);
                tb6612.set(motor[1], 1);

                while ((controller.get_reader_code() <= 5) &&
                       (std::chrono::duration<float>{t.elapsed_time()}.count() < TURN_INTERVAL_TIME))
                    ThisThread::sleep_for(10ms);

            case 4:
                // // Turn right
                motor[0].set_state(State::CCW);
                motor[0].set_duty_cycle(0.50f);

                motor[1].set_state(State::Brake);
                motor[1].set_duty_cycle(0.00f);

                tb6612.set(motor[0], 0);
                tb6612.set(motor[1], 1);

                ThisThread::sleep_for(700ms);
                break;

            default:
                break;
            }
        }

        // ライントレース(直進)のコード
        while ((controller.get_reader_code() <= 5) &&
               (std::chrono::duration<float>{t.elapsed_time()}.count() < TURN_INTERVAL_TIME))
        {
            line_trace.read();

            Motor motor[2];
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
    led = 3;

    if (controller.get_reader_code() >= 5)
        rgb_led[0] = rgb_led[1] = rgb_led[2] = 1;

    sleep();
}

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
