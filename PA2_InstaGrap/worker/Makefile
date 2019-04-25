CC = gcc
CFLAGS = -W -Wall -pthread
TARGET = worker
OBJECTS = worker.o worker_main.o
INC = -I../include

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(INC)
	rm -rf $(OBJECTS)

debug:
	$(CC) $(CFLAGS) -o $(TARGET) worker.c worker_main.c $(INC) -DDEBUG
	rm -rf $(OBJECTS)

.c.o :
	$(CC) -c $(INC) $(CFLAGS) $<

clean:
	rm -rf $(OBJECTS) $(TARGET)

worker.o : worker.c
worker_main.o : worker_main.c
