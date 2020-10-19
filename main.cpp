#include "mbed.h"
#include "TB6612.h"
#include "LineTrace.h"
#include "IrRemoteControl.h"
#include <cstring>

#define TURN_INTERVAL_TIME 2.0f

int move2value(char *move);

void setRgbLed(DigitalOut *rgb_led, int *color);

void check_rgb_led();
void check_led();
void check_md();
void check_line_trace();
void check_sps440();

// main() runs in its own thread in the OS
int main()
{
    BufferedSerial pc(USBTX, USBRX);

    pc.write("\n\nSTART\n", 8);

    pc.write("RGB\n", 4);
    check_rgb_led();

    pc.write("LED\n", 4);
    check_led();

    pc.write("MD\n", 3);
    check_md();

    pc.write("LINE\n", 5);
    check_line_trace();

    pc.write("IR\n", 3);
    check_sps440();

    pc.write("FINISH\n", 6);
    sleep();
}

void check_rgb_led()
{
    DigitalOut rgb_led[] = {
        DigitalOut(PB_5, 1),
        DigitalOut(PB_3, 1),
        DigitalOut(PB_4, 1),
    };

    int color[] = {0, 0, 0};

    color[0] = 0, color[1] = 0, color[2] = 0;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 0, color[1] = 0, color[2] = 255;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 0, color[1] = 255, color[2] = 0;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 0, color[1] = 255, color[2] = 255;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 255, color[1] = 0, color[2] = 0;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 255, color[1] = 0, color[2] = 255;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 255, color[1] = 255, color[2] = 0;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 255, color[1] = 255, color[2] = 255;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);

    color[0] = 0, color[1] = 0, color[2] = 0;
    setRgbLed(rgb_led, color);
    ThisThread::sleep_for(500ms);
}

void check_led()
{
    BusOut led(PB_6, PB_7);

    for (int i = 0; i < 5; i++)
    {
        led = i;
        ThisThread::sleep_for(500ms);
    }
}

void check_md()
{
    TB6612 tb6612(PA_5, PA_4, PA_8, PA_7, PB_0, PB_1, PA_6);
    tb6612.standby(1);

    Motor motor[2];

    for (float duty_cycle = 0.0f; duty_cycle <= 0.70f; duty_cycle += 0.035f)
    {
        motor[0].set_state(State::CW);
        motor[0].set_duty_cycle(duty_cycle);
        motor[1].set_state(State::CCW);
        motor[1].set_duty_cycle(duty_cycle);

        tb6612.set(motor[0], 0);
        tb6612.set(motor[1], 1);

        ThisThread::sleep_for(10ms);
    }

    motor[0].set_state(State::Brake);
    motor[0].set_duty_cycle(0.0f);
    motor[1].set_state(State::Brake);
    motor[1].set_duty_cycle(0.0f);

    tb6612.set(motor[0], 0);
    tb6612.set(motor[1], 1);
    ThisThread::sleep_for(510ms);

    for (float duty_cycle = 0.0f; duty_cycle <= 0.70f; duty_cycle += 0.035f)
    {
        motor[0].set_state(State::CCW);
        motor[0].set_duty_cycle(duty_cycle);
        motor[1].set_state(State::CW);
        motor[1].set_duty_cycle(duty_cycle);

        tb6612.set(motor[0], 0);
        tb6612.set(motor[1], 1);

        ThisThread::sleep_for(10ms);
    }

    tb6612.set(motor[0], 0);
    tb6612.set(motor[1], 1);
    ThisThread::sleep_for(510ms);
    tb6612.standby(0);
}

void check_line_trace()
{
    LineTrace line_trace(PA_0, PA_1, PA_3);

    line_trace.set_base_color(Color::Brack);

    BusOut led(PB_6, PB_7);

    while (1)
    {
        led = line_trace.read();
        ThisThread::sleep_for(10ms);
    }
}

void check_sps440()
{
    IrRemoteControl controller(PF_0);
    controller.reset_reader_code();

    BusOut led(PB_6, PB_7);
    for (int i = 1;; i++)
    {
        if (controller.get_reader_code() >= 5)
            break;
        led = i;
        ThisThread::sleep_for(100ms);
    }
    led = 0;
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
