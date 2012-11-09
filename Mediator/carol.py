#:coding=utf-8:

from os import stat
from re import sub
from time import sleep
from urllib import urlopen
import serial


FILE_PATH = "http://malt.dip.jp/AQUA/ORF/output.txt"

PORT = "/dev/cu.PL2303-000013FA"
# PORT = "/dev/tty.PL2303-000012FD"

def rs232(device):
    return serial.Serial(
        port=device,
        baudrate=9600,
        bytesize=8,
        parity='N',
        stopbits=1
    )

def retrieve(combo):
    seq = iter([combo.alice, combo.alice_base, combo.bob_base])
    for line in urlopen(FILE_PATH).readlines():
        h = seq.next()
        h.set(sub(r'\s','',line))

class bitline:
    def __init__(self):
        self.stringing = ""
    def cat(self, string):
        self.string += string
    def set(self, string):
        self.string = string
    def get(self):
        return self.string
    def getof(self, offset):
        return int(self.string[offset])
    def show_bin(self):
        print bin(int(self.string, 16))[2:]

class bitcombo:
    def __init__(self):
        self.alice = bitline()
        self.alice_base = bitline()
        self.bob_base = bitline()

    def is_same_as(self, another):
        if (self.alice.get() == another.alice.get()
            and self.alice_base.get() == another.alice_base.get()
            and self.bob_base.get() == another.bob_base.get()):
            return True
        else:
            return False

    def show(self):
        print "Alice: " + self.alice.get()
        print "Alice's base: " + self.alice_base.get()
        print "Bob's base: " + self.bob_base.get()

    def copy(self, another):
        self.alice.set(another.alice.get())
        self.alice_base.set(another.alice_base.get())
        self.bob_base.set(another.bob_base.get())

if __name__ == "__main__":
    try:
        ser = rs232(PORT)
    except:
        print "Connection falied!"
        exit()

    ser.isOpen()

    current = bitcombo()
    stored = bitcombo()
    retrieve(stored)
    stored.show()

    while 1:
        retrieve(current)
        if current.is_same_as(stored):
            sleep(5)
            continue

        stored.copy(current)
        current.show()
        for i in range(0,20):
            base = current.alice_base.getof(i)
            bit = current.alice.getof(i)
            # print "[i, base, bit]", i, base, bit
            if base == 0 and bit == 0:
                ser.write('1')
            elif base == 0 and bit == 1:
                ser.write('2')
            elif base == 1 and bit == 0:
                ser.write('3')
            elif base == 1 and bit == 1:
                ser.write('4')
            else:
                tmp = ser.read()
            sleep(1)


