CC = gcc
CFLAGS = -pthread -shared -fPIC
TARGET = dmonitor.so
INC = -I../DataStructures/include -I.

all : 
	$(CC) $(CFLAGS) -o $(TARGET) dmonitor.c $(INC) -ldl

debug :
	$(CC) $(CFLAGS) -o $(TARGET) dmonitor.c $(INC) -DDEBUG -ldl

clean : 
	rm $(TARGET)
