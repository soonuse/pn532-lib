CC=gcc
DLIBS=-lwiringPi
INC_DIR=../../
all:rpi_read_gpio.exe rpi_write_gpio.exe rpi_dump_mifare.exe rpi_get_uid.exe
rpi_read_gpio.exe:rpi_read_gpio.o pn532.o pn532_rpi.o
	$(CC) -Wall -o $@ $^ $(DLIBS)
rpi_read_gpio.o:rpi_read_gpio.c
	$(CC) -Wall -c $^ $(DLIBS) -I$(INC_DIR)
rpi_write_gpio.exe:rpi_write_gpio.o pn532.o pn532_rpi.o
	$(CC) -Wall -o $@ $^ $(DLIBS)
rpi_write_gpio.o:rpi_write_gpio.c
	$(CC) -Wall -c $^ $(DLIBS) -I$(INC_DIR)
rpi_dump_mifare.exe:rpi_dump_mifare.o pn532.o pn532_rpi.o
	$(CC) -Wall -o $@ $^ $(DLIBS)
rpi_dump_mifare.o:rpi_dump_mifare.c
	$(CC) -Wall -c $^ $(DLIBS) -I$(INC_DIR)
rpi_get_uid.exe:rpi_get_uid.o pn532.o pn532_rpi.o
	$(CC) -Wall -o $@ $^ $(DLIBS)
rpi_get_uid.o:rpi_get_uid.c
	$(CC) -Wall -c $^ $(DLIBS) -I$(INC_DIR)
pn532.o pn532_rpi.o:$(INC_DIR)pn532.c $(INC_DIR)pn532_rpi.c
	$(CC) -Wall -c $(INC_DIR)pn532.c
	$(CC) -Wall -c $(INC_DIR)pn532_rpi.c
clean:
	rm *.o *.exe
