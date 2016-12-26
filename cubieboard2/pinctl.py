#!/usr/bin/env python
# -*- coding: utf-8

import logging

sysctl_gpio_rootdir = '/sys/class/gpio'

def pin_syspath(pin_number): return '%s/gpio%d' % (sysctl_gpio_rootdir, pin_number)

def pinGetValue(pin_number, default=-1):
    result = default
    value_path = '%s/value' % pin_syspath(pin_number)
    with open(value_path, 'r') as fd:
        try:
            with open(value_path, 'r') as fd:
                result = fd.readline().strip()
        except IOError as e:
            logging.error(e.errno + ' ' + e)
    return result


if __name__ == "__main__":
    val = pinGetValue(50)
    print(val)


