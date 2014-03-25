all: read init

init: init_unix.o
	g++ init_unix.o -o init -I ./ `pkg-config --libs opencv`

read: testread.o Read_unix.o
	g++ Read_unix.o testread.o -o read -I ./ -lopencv_highgui `pkg-config --libs opencv`

init_unix.o: init_unix.cpp
	g++ -c -std=c++11 -Wall init_unix.cpp -I ./

Read_unix.o: Read_unix.cpp
	g++ -c -std=c++11 -Wall Read_unix.cpp -I ./

testread.o: testread.cpp
	g++ -c -std=c++11 -Wall testread.cpp -I ./

clean:
	rm -rf *o init read
