CC = gcc
CFLAGS = -pthread -shared -fPIC
TARGET = ddetector.so
INC = -I../DataStructures/include -I.

all : 
	$(CC) $(CFLAGS) -o $(TARGET) ddetector.c data.c detector_func.c ../DataStructures/src/linked_list.c ../DataStructures/src/graph.c $(INC) -ldl

debug :
	$(CC) $(CFLAGS) -o $(TARGET) ddetector.c data.c detector_func.c ../DataStructures/src/linked_list.c ../DataStructures/src/graph.c $(INC) -DDEBUG -ldl

clean : 
	rm $(TARGET)
