CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

all: vehicle_ecu dashboard_ecu

vehicle_ecu: src/vehicle_ecu.c
	$(CC) $(CFLAGS) -o $@ $<

dashboard_ecu: src/dashboard_ecu.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f vehicle_ecu dashboard_ecu

.PHONY: all clean
