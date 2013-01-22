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

#FILE_PATH = "http://lain.sfc.wide.ad.jp/qkd/output.txt"
#CGI_PATH = "http://lain.sfc.wide.ad.jp/qkd/result.cgi"
FILE_PATH = "http://aqua-cat.sfc.wide.ad.jp/FIRST2012/output.txt"
CGI_PATH = "http://aqua-cat.sfc.wide.ad.jp/FIRST2012/result.cgi"
KEY_SIZE = 20
#NMEASURES = 10
#NCALIBRATIONS = 10 # Must be an odd number

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

    # A0:1, B0, E2, Block State
    ser.write('m')
    ser.write('0')
    ser.write('1')
    ser.write('0')
    ser.write('2')
    for i in range(0,NCALIBRATIONS):
        tmp = sub(r'\D','',ser.readline())
        if tmp == '':
            tmp = '0'
        elif 999 < int(tmp):
            continue
        bit_tmp[i] = int(tmp)
        sleep(0.1)

    bit_tmp.sort()
    print bit_tmp
    thld[0] = bit_tmp[NCALIBRATIONS/2]
   
    sleep(3)
    # A0:0, B0, E2, Open State
    ser.write('m')
    ser.write('0')
    ser.write('0')
    ser.write('0')
    ser.write('2')
    for i in range(0,NCALIBRATIONS):
        tmp = sub(r'\D','',ser.readline())
        if tmp == '':
            tmp = '0'
        elif 999 < int(tmp):
            continue
        bit_tmp[i] = int(tmp)
        sleep(0.1)

    bit_tmp.sort()
    print bit_tmp
    thld[1] = bit_tmp[NCALIBRATIONS/2]

    return thld


def start_qkd(ser, cur, thld):
    ser.write('s')
    print 'QKD demo started!'
    for i in range(0,KEY_SIZE):
        ser.write(cur.alice_bit.getof(i))
        print cur.alice_bit.getof(i)
        sleep(0.2)
        ser.write(cur.alice_base.getof(i))
        print cur.alice_base.getof(i)
        sleep(0.2)
        ser.write(cur.bob_base.getof(i))
        print cur.bob_base.getof(i)
        sleep(0.2)
        ser.write(cur.eve_base.getof(i))
        print cur.eve_base.getof(i)
        sleep(0.6)
        for i in range(0,5):
            print ser.readline()
#
#        bit_tmp = 0
#        for j in range(0,NMEASURES):
#            tmp = sub(r'\D','',ser.readline())
#            if tmp =='':
#               tmp = '0'
#            bit_tmp += int(tmp)
#        bit_tmp = bit_tmp/10.0
        
#        if bit_tmp <= thld[0]:
#            bob_bit += '0'
#        elif thld[1] <= bit_tmp:
#            bob_bit += '1'
#        else:
#            bob_bit += random.choice('01')
    
    bob_bit = "11111111000000011101"
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

    #thld = auto_calibration(ser) # Threshold for 0 and 1
    thld = [10,210]
    print "Calibrated threshold is: [0]<=%d, %d<=[1]" % (thld[0],thld[1])

    while 1:
        retrieve(current)
        if current.is_same_as(stored):
            sleep(5)
            continue
        
        stored.copy(current)
        current.show()
        bob_bit = bitline()
        start_qkd(ser, current, thld)
