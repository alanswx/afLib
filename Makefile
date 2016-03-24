CPP=g++
CPPFLAGS=-I. -Ilinux
DEPS =  afErrors.h afLib.h afSPI.h arduinoSPI.h Command.h iafLib.h linux/Arduino.h linux/gpiolib.h linux/linuxLog.h linux/linuxSPI.h linux/SPI.h linux/stream.h msg_types.h StatusCommand.h
OBJ = linux/linuxLog.o linux/linuxSPI.o Command.o StatusCommand.o afLib.o linux/gpiolib.o 

HUEDEPS = hue/http.h hue/huelightresponse.h hue/request.h hue/hueipresponse.h
HUEOBJ = hue/http.o hue/request.o hue/huelightresponse.o hue/hueipresponse.o
HUELDFLAGS = -lcurl -ljansson

# - this isn't working for some reason
%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

all: afBlink hueSample

afBlink: linux/afBlink.o $(OBJ) $(DEPS)
	g++ -o afBlink linux/afBlink.o $(OBJ) 

hueSample: hue/hueSample.o $(OBJ) $(HUEOBJ) $(DEPS) $(HUEDEPS)
	g++ -o hueSample hue/hueSample.o $(OBJ) $(HUEOBJ) $(HUELDFLAGS)

clean:
	rm *.o linux/*.o hue/*.o afBlink hueSample
