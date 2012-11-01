#:coding=utf-8:

import time
import serial

ALICE_PORT = "/dev/ttyUSB1"

def rs232(device):
    return serial.Serial(
        port=device,
        baudrate=9600,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )

if __name__ == "__main__":
    alice = rs232(ALICE_PORT)
    alice.open()
    alice.isOpen()
    alice.write('abcd0' + '\r\n')
    out = ''
    time.sleep(1)
    while ser.inWaiting() > 0:
        out += ser.read(1)

    if out != '':
       print '>>' + out

    print 'Done!'
