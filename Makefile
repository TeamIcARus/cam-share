all: read init

init: init.o
	g++ init.o -o init -I ./ `pkg-config --libs opencv`

read: read.o
	g++ read.o -o read -I ./ -lopencv_highgui `pkg-config --libs opencv`

init.o: init.cpp
	g++ -c -std=c++11 -Wall init.cpp -I ./

read.o: read.cpp
	g++ -c -std=c++11 -Wall read.cpp

clean:
	rm -rf *o init read
