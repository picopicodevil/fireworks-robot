#include "IrRemoteControl.h"
#include "mbed.h"

IrRemoteControl::IrRemoteControl(PinName p1)
    : _in(p1)
{
    _in.rise(callback(this, &IrRemoteControl::timer_stop));
    _in.fall(callback(this, &IrRemoteControl::timer_start));
}

void IrRemoteControl::timer_start()
{
    _timer.reset();
    _timer.start();
}

void IrRemoteControl::timer_stop()
{
    _timer.stop();
    unsigned int ms = std::chrono::duration_cast<std::chrono::milliseconds>(_timer.elapsed_time()).count();
    if ((8 <= ms) && (ms <= 10))
    {
        _get_reader_code++;
    }
}

int IrRemoteControl::get_reader_code()
{
    return _get_reader_code;
}

void IrRemoteControl::reset_reader_code()
{
    _get_reader_code = 0;
}