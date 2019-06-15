CC=gcc
DLIBS=-lwiringPi
TARGET=$(notdir $(CURDIR))
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
$(TARGET):$(OBJS)
	$(CC) -Wall -o $@ $^ $(DLIBS)
%.o:%.c
	$(CC) -Wall -c $^ $(DLIBS)
clean:
	rm *.o $(TARGET)
