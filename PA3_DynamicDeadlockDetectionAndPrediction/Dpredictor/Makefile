CC = gcc
CFLAGS = -pthread
TARGET = dpredictor
INC = -I../DataStructures/include -I.

all : 
	$(CC) $(CFLAGS) -o $(TARGET) dpredictor.c seg_data.c ../DataStructures/src/tree.c $(INC)

debug :
	$(CC) $(CFLAGS) -o $(TARGET) dpredictor.c seg_data.c ../DataStructures/src/tree.c $(INC) -DDEBUG

clean : 
	rm $(TARGET)
