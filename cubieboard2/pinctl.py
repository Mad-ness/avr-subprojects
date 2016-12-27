#!/usr/bin/env python
# -*- coding: utf-8

import logging

sysctl_gpio_rootdir = '/sys/class/gpio'


def pin_syspath(pin_number): return '%s/gpio%d' % (sysctl_gpio_rootdir, pin_number)


def pinGetValue(pin_number, data_type=int):
    result = None
    value_path = '%s/value' % pin_syspath(pin_number)
    try:
        with open(value_path, 'r') as fd:
            result = data_type(fd.readline().strip())
    except IOError as e:
        logging.error("{errmsg}".format(errmsg=e))
    return result


def pinGetDirection(pin_number):
    result = None
    value_path = '%s/direction' % pin_syspath(pin_number)
    try:
        with open(value_path, 'r') as fd:
            result = fd.readline().strip().upper()
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


