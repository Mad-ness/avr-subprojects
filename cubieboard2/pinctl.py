#!/usr/bin/env python
# -*- coding: utf-8

import logging

sysctl_gpio_rootdir = '/sys/class/gpio'


def pin_syspath(pin_number): return '%s/gpio%d' % (sysctl_gpio_rootdir, pin_number)


def pinGetValue(pin_number, default=-1):
    result = default
    value_path = '%s/value' % pin_syspath(pin_number)
    try:
        with open(value_path, 'r') as fd:
            result = fd.readline().strip()
    except IOError as e:
        logging.error("{errmsg}".format(errmsg=e))
    return result


def pinSetValue(pin_number, new_value):
    result = False
    new_value = 1 if new_value > 0 else 0
    value_path = '%s/value' % pin_syspath(pin_number)
    try:
        with open(value_path, 'w') as fd:
            fd.write(str(new_value))
            result = True
    except IOError as e:
        logging.error(e.errno + ' ' + e)
    return result


if __name__ == "__main__":
    val = pinGetValue(50)
    print(val)
    pinSetValue(50, 1)


