#!/usr/bin/env python
#
# -*- coding: utf-8
#
from pinctl import pinSetValue, pinGetValue, pinGetDirection
from config import load_config, Configuration
from time import sleep

global_config = load_config()


class GPIOControl:
    m_pin_number = 0x0
    m_pin_type = ''
    p_pin_name = ''
    m_pin_ok = False
    attrs = {}

    def __init__(self, on_value = 1, off_value = 0):
        self.on = self.setHigh
        self.off = self.setLow
        self.STATE_HIGH = on_value
        self.STATE_LOW = off_value
        self.initPinDetails()

    def initPinDetails(self):
        for cfg in global_config['pins_configuration']:
            if cfg['id'] == self.m_pin_type:
                self.m_pin_number = cfg['pin_id']
                self.m_pin_name = cfg['pin_name']
                self.m_pin_ok = True
                if 'attrs' in cfg:
                    self.attrs = cfg['attrs']
                break

    def pinOk(self): return self.m_pin_ok

    def info(self):
        state = self.gpioState()
        value_name = 'UNKNOWN'
        if state == self.STATE_LOW:
            value_name = 'LOW'
        elif state == self.STATE_HIGH:
            value_name = 'HIGH'
        msg = """Object    : {c_name}
Pin id    : {pin_number}
Pin name  : {pin_name}
Direction : {pin_direction}
State     : {pin_value} ({value_name})
Access Ok : {pin_ok}""".format(
                 pin_number = self.m_pin_number, 
                 pin_name = self.m_pin_name,
                 c_name = self.__class__.__name__,
                 pin_value = state,
                 value_name = value_name,
                 pin_ok = self.m_pin_ok and state != None,
                 pin_direction = pinGetDirection(self.m_pin_number)
        )
        print(msg)

    def gpioState(self): return pinGetValue(self.m_pin_number)
    def setHigh(self): return pinSetValue(self.m_pin_number, self.STATE_HIGH)
    def setLow(self): return pinSetValue(self.m_pin_number, self.STATE_LOW)



class GPIOWaterPump(GPIOControl):
    m_pin_type = 'water_pump'
    def shortRun(self):
        assert 'running_time_msec' in self.attrs, "Not found attribute 'running_time_msecs'"
        running_time_msec = int(self.attrs['running_time_msec'])
        self.on()
        sleep(running_time_msec)
        self.off()

class GPIOTableLamp(GPIOControl):
    m_pin_type = 'table_lamp'




if __name__ == "__main__":
    import sys
    cfg = load_config()
    gpio1 = GPIOWaterPump()
    gpio2 = GPIOTableLamp()

    if len(sys.argv) >= 3:
        if sys.argv[1] == 'gpio1':   gpio = gpio1
        elif sys.argv[2] == 'gpio2': gpio = gpio2

        if sys.argv[2] == 'on':
            gpio.runEngine()
        elif sys.argv[2] == 'off':
            gpio.off()
        elif sys.argv[2] == 'shortrun':
            gpio.shortRun()

    gpio1.info()
    gpio2.info()
    pass 

