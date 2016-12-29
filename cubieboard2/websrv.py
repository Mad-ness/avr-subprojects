#!/usr/bin/env python

import os, sys

#os.environ['PATH'] += os.path.abspath(sys.argv[1]).split('/')[0:-1]
#os.environ['PATH'] += ':' + '/'.join(os.path.abspath(__file__).split('/')[0:-2])

from flask import Flask, request, session, g, redirect, url_for, abort, render_template, flash
from devctl import GPIODevices

templates_dir = '/'.join(os.path.abspath(__file__).split('/')[0:-1]) + '/wbserv/templates'

app = Flask(__name__, template_folder=templates_dir)

@app.route("/devices", methods=['GET'])
def show_devices():
    return render_template('show_devices.html', devices=GPIODevices().listDevices())

app.run(host='0.0.0.0')

