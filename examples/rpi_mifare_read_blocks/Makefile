CC=gcc
DLIBS=-lwiringPi
TARGET=rpi_mifare_read_blocks
$(TARGET):$(TARGET).o pn532_rpi.o pn532.o
	$(CC) -Wall -o $(TARGET) $(TARGET).o pn532_rpi.o pn532.o $(DLIBS)
pn532_rpi.o:pn532_rpi.c pn532_rpi.h 
	$(CC) -Wall -c pn532_rpi.c $(DLIBS)
pn532.o:pn532.c pn532.h
	$(CC) -Wall -c pn532.c $(DLIBS)
$(TARGET).o:$(TARGET).c pn532_rpi.h pn532.h
	$(CC) -Wall -c $(TARGET).c $(DLIBS)
clean:
	rm *.o
	rm $(TARGET)
