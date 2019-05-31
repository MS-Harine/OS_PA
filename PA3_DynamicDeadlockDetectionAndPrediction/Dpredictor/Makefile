CC = gcc
CFLAGS = -pthread
TARGET = dpredictor
INC = -I../DataStructures/include -I.

all : 
	$(CC) $(CFLAGS) -o $(TARGET) seg_data.c data.c ../DataStructures/src/linked_list.c ../DataStructures/src/graph.c ../DataStructures/src/tree.c dpredictor.c $(INC)

debug :
	$(CC) $(CFLAGS) -o $(TARGET) seg_data.c data.c ../DataStructures/src/linked_list.c ../DataStructures/src/graph.c ../DataStructures/src/tree.c dpredictor.c $(INC) -DDEBUG

clean : 
	rm $(TARGET)
