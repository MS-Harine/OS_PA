all:
	gcc -pthread -shared -fPIC -o ~/ddetector.so *.c ../DataStructures/src/* -I. -I../DataStructures/include/ -ldl -g

debug:
	gcc -pthread -shared -fPIC -o ~/ddetector.so *.c ../DataStructures/src/* -I. -I../DataStructures/include/ -ldl -DDEBUG -g
