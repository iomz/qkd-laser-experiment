#:coding=utf-8:

from urllib import urlopen
from os import stat
from re import sub
from time import sleep

file_path = "http://malt.dip.jp/AQUA/ORF/output.txt"


def get_3hex(combo):
    seq = iter([combo.alice, combo.alice_base, combo.bob_base])
    for line in urlopen(file_path).readlines():
        h = seq.next()
        h.set(hex(int(sub(r'\s','',line),2))[2:])

class hexstr:
    def __init__(self):
        self.stringing = ""
    def cat(self, string):
        self.string += string
    def set(self, string):
        self.string = string
    def get(self):
        return self.string
    def show_bin(self):
        print bin(int(self.string, 16))[2:]

class hexcombo:
    def __init__(self):
        self.alice = hexstr()
        self.alice_base = hexstr()
        self.bob_base = hexstr()

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
    current = hexcombo()
    stored = hexcombo()
    get_3hex(stored)  
    stored.show()
    while 1:
        get_3hex(current)
        if current.is_same_as(stored):
            sleep(5)
        else:
            current.show()
            stored.copy(current)
            sleep(10)
