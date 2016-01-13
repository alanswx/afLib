CPP=g++
CFLAGS=-I.
DEPS = hellomake.h
OBJ = afLib.o Command.o StatusCommand.o

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

testAfLib: test.o $(OBJ)
	g++ -o testAfLib test.o $(OBJ) -lwiringPi

clean:
	rm *.o testAfLib
