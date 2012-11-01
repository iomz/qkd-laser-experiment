#:coding=utf-8:

import serial
import struct
import sys
import time
from flask import Flask


app = Flask(__name__)

@app.route("/0")
def arduino_1():
    ser.write("1")
    time.sleep(0.1)
    return "LED 1 ON!"

if __name__ == "__main__":
    tmp_msg = "/dev/tty.usbserial-A800eGnD"
    ser = serial.Serial(tmp_msg)
    ser.baudrate = 115200
    ser.timeout = 1
    print ser.portstr

    app.run("127.0.0.1",1234)
    ser.close()
