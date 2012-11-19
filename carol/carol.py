#:coding=utf-8:

from numpy import array
from os import stat
from re import sub
from shlex import split
from subprocess import call
from sys import argv
from time import sleep
from urllib import urlopen
import serial

FILE_PATH = "http://malt.dip.jp/AQUA/ORF/output.txt"
CGI_PATH = "http://malt.dip.jp/AQUA/ORF/result.cgi"

# RS232C init
def rs232(device):
    return serial.Serial(
        port=device,
        baudrate=4800,
        bytesize=8,
        parity='N',
        stopbits=1
    )


# Fetch the combination from the web
def retrieve(combo):
    seq = iter([combo.alice_bit, combo.alice_base, combo.bob_base, combo.eve_base])
    for line in urlopen(FILE_PATH).readlines():
        h = seq.next()
        h.set(sub(r'\s','',line))


# Bit/Base string container
class bitline:
    def __init__(self):
        self.string = ""
    def cat(self, string):
        self.string += string
    def set(self, string):
        self.string = string
    def setof(self, offset, char):
        self.string[offset] = char
    def get(self):
        return self.string
    def getof(self, offset):
        return self.string[offset]
    def show_bin(self):
        print bin(int(self.string, 16))[2:]


# A complete set of Bit/Base combination 
class bitcombo:
    def __init__(self):
        self.alice_bit = bitline()
        self.alice_base = bitline()
        self.bob_base = bitline()
        self.eve_base = bitline()

    def is_same_as(self, another):
        if (self.alice_bit.get() == another.alice_bit.get()
            and self.alice_base.get() == another.alice_base.get()
            and self.bob_base.get() == another.bob_base.get()
            and self.eve_base.get() == another.eve_base.get()):
            return True
        else:
            return False

    def show(self):
        print "Alice's bit: " + self.alice_bit.get()
        print "Alice's base: " + self.alice_base.get()
        print "Bob's base: " + self.bob_base.get()
        print "Eve's base: " + self.eve_base.get()

    def copy(self, another):
        self.alice_bit.set(another.alice_bit.get())
        self.alice_base.set(another.alice_base.get())
        self.bob_base.set(another.bob_base.get())
        self.eve_base.set(another.eve_base.get())


def measure_bit(bit):
    pass


#Return 2 threshold for 0 and 1 as array
def auto_calibration(ser):
    thld = []*2
    bit_tmp = []*77

    ser.write('b')
    for i in range(0,77):
        tmp = sub(r'\D','',ser.readline())
        if tmp == '':
            tmp = '0'
        bit_tmp[i] = int(tmp)
    thld[0]
    
    ser.write('o')
    for i in range(0,77):
        tmp = sub(r'\D','',ser.readiline())
        if tmp == '':
            tmp = '0'
        bit_tmp[i] = int(tmp)
    thld[1] = bit_tmp[38]

    return thld


def start_qkd(ser, cur, bob_bit, thld):
    ser.write('s')
    for i in range(0,20):
        ser.write(cur.alice_bit.getof(i))
        ser.write(cur.alice_base.getof(i))
        ser.write(cur.bob_base.getof(i))
        ser.write(cur.eve_base.getof(i))
        sleep(1)

        bit_tmp = 0
        for j in range(0,10):
            tmp = sub(r'\D','',ser.readiline())
            bit_tmp += int(tmp)
        bit_tmp = bit_tmp/10.0
        
        if bit_tmp <= thld[0]:
            bob_bit.setof(i, '0')
        elif thld[1] <= bit_tmp:
            bob_bit.setof(i, '1')
        else:
            bob_bit.setof(i, random.choice('01'))
    
    print bob_bit
    cmd = "wget -q -O - --no-check-certificate --post-data 'result=" + bob_bit + "' " + CGI_PATH 
    call(split(cmd))


if __name__ == "__main__":
    if len(argv) != 2:
        print "Usage: %s <serial port>"%argv[0]

    try:
        ser = rs232(argv[1])
    except:
        print "Connection falied!"
        exit()

    ser.isOpen()

    current = bitcombo()
    stored = bitcombo()
    retrieve(stored)
    stored.show()

    thld = auto_calibration(ser) # Threshold for 0 and 1
    print "Calibrated hreshold is: 0<=%d, %d<=1" % (thld[0],thld[1])

    while 1:
        retrieve(current)
        #if current.is_same_as(stored):
        #    sleep(5)
        #    continue

        stored.copy(current)
        current.show()
        bob_bit = bitline()
        start_qkd(ser, current, bob_bit, thld)
