#:coding=utf-8:

from numpy import array
from os import stat, walk
from os.path import join
from re import sub
from shlex import split
from subprocess import call
from sys import argv
from time import sleep
from urllib import urlopen
import random
import serial
import time
import pickle

KEY_SIZE = 20
BASIS = [60,570,1080,1590] 
COMMAND = ['alice','bob','carol','0','1']
M_FACTOR = 1

# Serial-USART via microUSB init
def serial_init(device):
    return serial.Serial(
        port=device,
        baudrate=9600,
        bytesize=8,
        parity='N',
        stopbits=1,
        timeout=0.2
    )

def auto_calibration(ser):
    thld = {}

    thld['H0'] = [0,-1]
    thld['H1'] = [0,-1]
    thld['L0'] = [1000, -1]
    thld['L1'] = [1000, -1]
    print thld
    ser.write('\n\n')

    for angle in range(0, 1800, 30):
        ser.write('r')
        string = ser.read(64)
        ser.write('%d\n\r'%angle)
        result = ser.readline()
        ser.write('m')
        result = ser.readline()
        val = int(sub('^.+: ', '', result))
        print val
        if thld['H0'][0] < val:
            thld['H0'] = [val, angle]
        if thld['L0'][0] > val:
            thld['L0'] = [val, angle]

    thld['M0'] = [(thld['H0'][0] + thld['L0'][0])/2, -1]

    raw_input("Hit ENTER when the laser is in basis-1 position")
    for angle in range(0, 1800, 30):
        ser.write('r')
        string = ser.read(64)
        ser.write('%d\n\r'%angle)
        result = ser.readline()
        ser.write('m')
        result = ser.readline()
        val = int(sub('^.+: ', '', result))
        print val
        if thld['H1'][0] < val:
            thld['H1'] = [val, angle]
        if thld['L1'][0] > val:
            thld['L1'] = [val, angle]

    thld['M1'] = [(thld['H1'][0] + thld['L1'][0])/2, -1]
    return thld

def alice(ser,start_time):
    print 'Startng QKD'
    start_time+=2
    #fd = open("%s.alice"%start_time, 'w')
    fd = open("alice.tmp", 'w')
    for i in range(20):
        ser.write('r')
        str = ser.read(64)
        angle = BASIS[int(random.choice('0123'))]
        ser.write('%d\n\r'%angle)
        string = ser.readline()
        if angle == BASIS[0]:
            fd.write('0 0')
        elif angle == BASIS[1]:
            fd.write('1 0')
        elif angle == BASIS[2]:
            fd.write('0 1')
        elif angle == BASIS[3]:
            fd.write('1 1')
        fd.write('\n')
        sleep_until_start(start_time+i*M_FACTOR)
    pos_reset(0)
    fd.close()

def bob(ser,start_time,thld):
    print 'Startng QKD'
    start_time+=2
    #fd = open("%s.bob"%start_time, 'w')
    fd = open("bob.tmp", 'w')
    for i in range(20):
        ser.write('r')
        str = ser.read(64)
        measure = [thld['L0'][1],thld['L1'][1]]
        angle = measure[int(random.choice('01'))]
        ser.write('%d\n\r'%angle)
        string = ser.readline()
        
        ser.write('m')
        result = ser.readline()
        val = int(sub('^.+: ', '', result))

        if angle == measure[0]:
            up = abs(val - thld['H0'][0])
            down = abs(val - thld['L0'][0])
            mid = abs(val - thld['M0'][0])
            minimum = min([up, down, mid])
            if minimum == up:
                fd.write('0 1')
            elif minimum == down:
                fd.write('0 0')
            else:
                fd.write('0 %d'%(int(random.choice('01'))))

        elif angle == measure[1]:
            up = abs(val - thld['H1'][0])
            down = abs(val - thld['L1'][0])
            mid = abs(val - thld['M1'][0])
            minimum = min([up, down, mid])
            if minimum == up:
                fd.write('1 1')
            elif minimum == down:
                fd.write('1 0')
            else:
                fd.write('1 %d'%(int(random.choice('01'))))
        fd.write('\n')
        sleep_until_start(start_time+i*M_FACTOR)
    fd.close()
    pos_reset(0)

def sleep_until_start(start_time):
    if int(time.time()) > start_time:
        print "Unix timestamp: %d is passed already"%start_time
        exit()

    while int(time.time()) != start_time:
        print "%d < %d"%(int(time.time()), start_time)
        sleep(0.2)

def pos_reset(pos):
    ser.write('r')
    str = ser.read(64)
    angle = BASIS[pos]
    ser.write('%d\n\r'%angle)
    string = ser.readline()

if __name__ == "__main__":
    if len(argv)<2:
        print "Usage: %s [alice|bob] [unixtime]"%argv[0]
        exit()
    if argv[1] not in COMMAND:
        print "%s is not acceptable."%argv[1]
        print "Usage: %s [alice|bob] [unixtime]"%argv[0]
        exit()

    for root, dirs, files in walk('/dev'):
        for name in files:
            if 'cu.usbserial' in name:
                device = join(root, name)
                confirm = raw_input("Connect to %s ?[y/N]"%device)
                if confirm == 'N':
                    device = None
                else:
                    break

    if device is None:
        print "No device found."
        exit()
    else:
        print "Connecting to %s ..."%device

    try:
        ser = serial_init(device)
    except:
        print "Connection falied!"
        exit()

    print "Device: %s is attached."%device
    ser.isOpen()
    
    if argv[1] == 'alice':
        start_time = int(argv[2])
        sleep_until_start(start_time)
        alice(ser, start_time)
    elif argv[1] == 'bob':
        start_time = int(argv[2])
        sleep_until_start(start_time)
        try:
            thld = pickle.load( open("thld.pkl", "rb"))
        except:
            print "\"thld.pkl\" not found."
            exit()

        bob(ser, start_time, thld)
    elif argv[1] == 'carol':
        thld = auto_calibration(ser)
        pickle.dump(thld, open("thld.pkl", "wb"))
        print "Calibrated threshold saved."
    elif argv[1] == '0':
        pos_reset(0)
    elif argv[1] == '1':
        pos_reset(1)

    ser.close()
