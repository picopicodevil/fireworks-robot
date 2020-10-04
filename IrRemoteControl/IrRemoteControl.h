#ifndef IR_REMOTE_CONTROL_H
#define IR_REMOTE_CONTROL_H

#include "mbed.h"

class IrRemoteControl{
public:
    IrRemoteControl(PinName p1);

    int get_reader_code();

    void reset_reader_code();

private:
    InterruptIn _in;
    Timer _timer;

    int _get_reader_code;

    void timer_start();
    void timer_stop();
};

#endif