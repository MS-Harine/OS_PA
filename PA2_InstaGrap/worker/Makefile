CC = gcc
CFLAGS = -W -Wall -pthread
TARGET = worker
OBJECTS = worker.o worker_main.o
INC = -I../include

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(INC)

.c.o :
	$(CC) -c $(INC) $(CFLAGS) $<

clean:
	rm -rf $(OBJECTS) $(TARGET)

worker.o : worker.c
worker_main.o : worker_main.c
