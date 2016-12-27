#!/usr/bin/env python
#
# -*- coding: utf-8
#
import yaml

config_file = 'config.yaml'
Configuration = {}

def load_config():
    with open(config_file, 'r') as stream:
        try:
           Configuration = yaml.load(stream)
        except yaml.YAMLError as exc:
            logging.error(exc)
    return Configuration

