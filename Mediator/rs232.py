#:coding=utf-8:

import time
import serial

PORT = "/dev/cu.PL2303-000012FD"

def rs232(device):
    return serial.Serial(
        port=device,
        baudrate=9600,
        bytesize=8,
        parity='N',
        stopbits=1
    )

if __name__ == "__main__":
    ser = rs232(PORT)
    ser.isOpen()
    time.sleep(0.25)
    print ser.readline()

    ser.write('a')
    print ser.read()
    ser.write('b')
    print ser.read()
