#:coding=utf-8:

import time
import serial
import sys

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    # port='/dev/ttyUSB1',
    port=sys.argv[1],
    baudrate=9600,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

ser.isOpen()

while 1 :
    while ser.inWaiting() > 0:
        out = ser.read(1)
        
        if out != '':
            print "val: %d"%ord(out)
    time.sleep(1)
