all: read init

init: init.o
	g++ init.o -o init -I ./ `pkg-config --libs opencv`

read: testread.o Read.o
	g++ Read.o testread.o -o read -I ./ -lopencv_highgui `pkg-config --libs opencv`

init.o: init.cpp
	g++ -c -std=c++11 -Wall init.cpp -I ./

Read.o: Read.cpp
	g++ -c -std=c++11 -Wall Read.cpp -I ./

testread.o: testread.cpp
	g++ -c -std=c++11 -Wall testread.cpp -I ./

clean:
	rm -rf *o init read
