#:coding=utf-8:

from os import stat
from time import sleep

file_path = "test.txt"


def get_array(alice, alice_base, bob_base):
    for line in open(file_path).readlines():
        for array in [alice, alice_base, bob_base]:
            for i in range(0,20):
                array[i] = int(line[i])

def get_3hex(a, ab, bb):
    seq = iter([a, ab, bb])
    for line in open(file_path).readlines():
        h = seq.next()
        # for byte in [line[i:i+8] for i in range(0, len(line), 8)]:
        #     if len(byte) != 8:
        #         break
        #     h.cat(byte)
        h.set(hex(int(line,2))[2:])
        print line[:-1]

class hexstr:
    def __init__(self):
        self.obj = ""
    def cat(self, obj):
        self.obj += obj
    def set(self, obj):
        self.obj = obj
    def get(self):
        return self.obj
    def show_bin(self):
        print bin(int(self.obj, 16))[2:]

if __name__ == "__main__":
    lst_mod = stat(file_path).st_mtime
    # alice = [0 for _ in range(20)]
    # alice_base = [0 for _ in range(20)]
    # bob_base = [0 for _ in range(20)]
   
    while 1:
        present = stat(file_path).st_mtime
        if lst_mod != present:
            # get_array(alice, alice_base, bob_base)
            alice = hexstr()
            alice_base = hexstr()
            bob_base = hexstr()
            get_3hex(alice, alice_base, bob_base)
            print "Alice:", alice.get()
            print "Alice's base:", alice_base.get()
            print "Bob's base:", bob_base.get()
            alice.show_bin()
            alice_base.show_bin()
            bob_base.show_bin()
            lst_mod = present
        else:
            sleep(5)
