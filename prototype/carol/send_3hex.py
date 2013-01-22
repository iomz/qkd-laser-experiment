#:coding=utf-8:

import time
import serial

PORT = "/dev/cu.PL2303-000012FD"

def rs232(device):
    return serial.Serial(
        port=device,
        baudrate=9600,
        bytesize=8,
        stopbits='N',
        parity=1
    )

if __name__ == "__main__":
    ser = rs232(ALICE_PORT)
    ser.open()
    ser.isOpen()
    in = ''
    while ser.inWaiting() > 0:
        in += ser.read(1)

    print in
