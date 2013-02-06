OBJS = fileTransceiver ./source/blowfish.o

fileTransceiver: ./source/fileTransceiver.c ./source/blowfish.o
	gcc ./source/fileTransceiver.c ./source/blowfish.o -o ./fileTransceiver
./source/blowfish.o:
	gcc -c ./source/blowfish.c -o ./source/blowfish.o

clean:
	rm -f $(OBJS)