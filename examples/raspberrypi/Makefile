CC = gcc
DLIBS = -lwiringPi
INC_DIR = ../../
SRCS = $(wildcard *.c)
all: $(SRCS:.c=.exe)
.PHONY: clean
%.exe: %.o pn532.o pn532_rpi.o
	$(CC) -Wall -o $@ $^ $(DLIBS)
%.o: %.c
	$(CC) -Wall -c $^ $(DLIBS) -I$(INC_DIR)
pn532.o pn532_rpi.o: $(INC_DIR)pn532.c $(INC_DIR)pn532_rpi.c
	$(CC) -Wall -c $(INC_DIR)pn532.c
	$(CC) -Wall -c $(INC_DIR)pn532_rpi.c
clean:
	rm *.o *.exe
