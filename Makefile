CPP=g++
CPPFLAGS=-I. -Ilinux  -Ihue
DEPS =  afErrors.h afLib.h afSPI.h arduinoSPI.h Command.h iafLib.h linux/Arduino.h linux/gpiolib.h linux/linuxLog.h linux/linuxSPI.h linux/SPI.h linux/stream.h msg_types.h StatusCommand.h
OBJ = linux/linuxLog.o linux/linuxSPI.o Command.o StatusCommand.o afLib.o linux/gpiolib.o 

HUEDEPS = hue/http.h hue/huelightresponse.h hue/request.h hue/hueipresponse.h
HUEOBJ = hue/http.o hue/request.o hue/huelightresponse.o hue/hueipresponse.o
HUELDFLAGS = -lcurl -ljansson

AFSTINGDEPS = 
AFSTINGOBJ = 
AFSTINGLDFLAGS = -lcurl -ljansson -lmosquitto

TARGET_LIB = libafLib.so
LDFLAGS = -shared

# - this isn't working for some reason
%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

all: afBlink hueSample afSting ${TARGET_LIB}

$(TARGET_LIB): $(OBJ)
	$(CPP) ${LDFLAGS} -o $@ $^

afBlink: linux/afBlink.o $(OBJ) $(DEPS)
	$(CPP) -o afBlink linux/afBlink.o $(OBJ) 

hueSample: hue/hueSample.o $(OBJ) $(HUEOBJ) $(DEPS) $(HUEDEPS)
	$(CPP) -o hueSample hue/hueSample.o $(OBJ) $(HUEOBJ) $(HUELDFLAGS)

afSting: afsting/afsting.o $(OBJ) $(AFSTINGOBJ) $(DEPS) $(AFSTINGDEPS)
	$(CPP) -o afSting afsting/afsting.o $(OBJ) $(AFSTINGOBJ) $(AFSTINGLDFLAGS)

install:
	cp ${TARGET_LIB} /usr/local/lib/
	ldconfig

clean:
	rm *.o linux/*.o hue/*.o afsting/*.o afBlink hueSample afSting
