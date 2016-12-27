#!/usr/bin/env python
#
# -*- coding: utf-8
#
from pinctl import pinSetValue, pinGetValue
from config import load_config, Configuration


global_config = load_config()


class GPIOControl:
    m_pin_number = 0x0
    p_pin_name = ''

    def __init__(self, pin, pin_name, on_value = 1, off_value = 0):
        self.m_pin_number = pin
        self.m_pin_name = pin_name
        self.on = self.setHigh
        self.off = self.setLow

    def info(self):
        msg = """
Object:   {c_name}
Pin id:   {pin_number}
Pin name: {pin_name}""".format(
                 pin_number = self.m_pin_number, 
                 pin_name = self.m_pin_name,
                 c_name = self.__class__.__name__
        )
        print(msg)
    
    def setHigh(self):
        pinSetValue(self.m_pin_number, 1)
        pass

    def setLow(self):
        pinGetValue(self.m_pin_number, 0)
        pass

    def state(self):
        result = None


class GPIOWaterPump(GPIOControl):
    def __init__(self):
        super(GPIOWaterPump, self).__init__(
            global_config['pins_configuration']['relay2engine']['pin_id'],
            global_config['pins_configuration']['relay2engine']['pin_name']
        )


if __name__ == "__main__":
    cfg = load_config()
    print(cfg)
    gpio1 = GPIOWaterPump()
    gpio1.off()
    gpio1.info()
    pass 

