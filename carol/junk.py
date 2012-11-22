#:coding=utf-8:

from numpy import array
from os import stat
from re import sub
from shlex import split
from subprocess import call
from sys import argv
from time import sleep
from urllib import urlopen
import random
import serial

FILE_PATH = "http://aqua-cat.sfc.wide.ad.jp/ORF2012/output.txt"
CGI_PATH = "http://aqua-cat.sfc.wide.ad.jp/ORF2012/result.cgi"
KEY_SIZE = 20
NMEASURES = 10
NCALIBRATIONS = 33 # Must be an odd number

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
        self.bit_list = [0]*KEY_SIZE
    def set(self, string):
        for i in range(0, KEY_SIZE):
            self.bit_list[i] = string[i]
    def setof(self, offset, char):
        self.bit_list[offset] = char
    def get(self):
        return self.bit_list
    def getof(self, offset):
        return self.bit_list[offset]
    def to_string(self):
        string = ''
        for i in range(0, KEY_SIZE):
            string += self.bit_list[i]
        return string

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
        print "Alice's bit: " + self.alice_bit.to_string()
        print "Alice's base: " + self.alice_base.to_string()
        print "Alice's bit: " + self.alice_bit.to_string()
        print "Eve's base: " + self.eve_base.to_string()

    def copy(self, another):
        self.alice_bit.set(another.alice_bit.to_string())
        self.alice_base.set(another.alice_base.to_string())
        self.bob_base.set(another.bob_base.to_string())
        self.eve_base.set(another.eve_base.to_string())


def measure_bit(bit):
    pass


#Return 2 threshold for 0 and 1 as array
def auto_calibration(ser):
    thld = [0]*2
    bit_tmp = [0]*NCALIBRATIONS

    ser.write('b')
    for i in range(0,NCALIBRATIONS):
        tmp = sub(r'\D','',ser.readline())
        if tmp == '':
            tmp = '0'
        bit_tmp[i] = int(tmp)
        sleep(0.1)

    bit_tmp.sort()
    print bit_tmp
    thld[0] = bit_tmp[NCALIBRATIONS/2]
   
    sleep(3)
    ser.write('o')
    for i in range(0,NCALIBRATIONS):
        tmp = sub(r'\D','',ser.readline())
        if tmp == '':
            tmp = '0'
        bit_tmp[i] = int(tmp)
        sleep(0.1)

    bit_tmp.sort()
    print bit_tmp
    thld[1] = bit_tmp[NCALIBRATIONS/2]

    return thld


def start_qkd():
    bob_bit = "00110011001111000111"
    cmd = "wget -q -O - --no-check-certificate --post-data 'result=" + bob_bit + "' " + CGI_PATH 
    call(split(cmd))


if __name__ == "__main__":
    current = bitcombo()
    stored = bitcombo()
    retrieve(stored)
    stored.show()

    while 1:
        retrieve(current)
        stored.copy(current)
        current.show()
        bob_bit = bitline()
        start_qkd()
