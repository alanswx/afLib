CPP=g++
CFLAGS=-I.
DEPS = hellomake.h
OBJ = afLib.o Command.o StatusCommand.o gpiolib.o

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

testAfLib: test.o $(OBJ)
	g++ -o testAfLib test.o $(OBJ) 

clean:
	rm *.o testAfLib
