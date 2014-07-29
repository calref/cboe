include ../.config.vars

CPP  = $(PREFIX)g++
CC   = $(PREFIX)gcc
WINDRES = $(PREFIX)windres
RES  = Blades_of_Exile_private.res
INCS =  -I"include" 
CXXINCS = 
BIN  = boesounds.dll
CXXFLAGS = $(CXXINCS) -Wall -O2 -g -std=c++0x -DWIN32
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before boesounds.dll all-after


clean: clean-custom
	${RM} dllmain.o $(BIN)

$(BIN): BOESOUNDS.res dllmain.o
	$(CPP) -shared -Wl,--dll  dllmain.o  BOESOUNDS.res  -o boesounds.dll 

dllmain.o: dllmain.cpp
	$(CPP) -c dllmain.cpp -o dllmain.o $(CXXFLAGS)

BOESOUNDS.res: BOESOUNDS.RC
	$(WINDRES) -J rc -O coff -i BOESOUNDS.RC -o BOESOUNDS.res
