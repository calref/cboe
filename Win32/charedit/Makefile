include ../.config.vars

CPP  = $(PREFIX)g++
CC   = $(PREFIX)gcc
WINDRES = $(PREFIX)windres
RES  = Blades_of_Exile_Character_Editor_private.res
OBJ  = graphutl.o bladpced.o dlogtool.o edaction.o edfileio.o editors.o edsound.o global.o graphics.o $(RES)
LINKOBJ  = graphutl.o bladpced.o dlogtool.o edaction.o edfileio.o editors.o edsound.o global.o graphics.o $(RES)
LIBS =  -lwinmm -mwindows -lwinmm -static-libgcc -static-libstdc++
INCS =  -I"include" 
CXXINCS =  
BIN  = "Blades of Exile Character Editor.exe"
CXXFLAGS = $(CXXINCS) -Wall    -O2 
CFLAGS = $(INCS)   -O2
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before "Blades of Exile Character Editor.exe" all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "Blades of Exile Character Editor.exe" $(LIBS)

graphutl.o: graphutl.cpp
	$(CPP) -c graphutl.cpp -o graphutl.o $(CXXFLAGS)

bladpced.o: bladpced.cpp
	$(CPP) -c bladpced.cpp -o bladpced.o $(CXXFLAGS)

dlogtool.o: dlogtool.cpp
	$(CPP) -c dlogtool.cpp -o dlogtool.o $(CXXFLAGS)

edaction.o: edaction.cpp
	$(CPP) -c edaction.cpp -o edaction.o $(CXXFLAGS)

edfileio.o: edfileio.cpp
	$(CPP) -c edfileio.cpp -o edfileio.o $(CXXFLAGS)

editors.o: editors.cpp
	$(CPP) -c editors.cpp -o editors.o $(CXXFLAGS)

edsound.o: edsound.cpp
	$(CPP) -c edsound.cpp -o edsound.o $(CXXFLAGS)

global.o: global.cpp
	$(CPP) -c global.cpp -o global.o $(CXXFLAGS)

graphics.o: graphics.cpp
	$(CPP) -c graphics.cpp -o graphics.o $(CXXFLAGS)

Blades_of_Exile_Character_Editor_private.res: Blades_of_Exile_Character_Editor_private.rc BLADPCED.RC 
	$(WINDRES) -i Blades_of_Exile_Character_Editor_private.rc --input-format=rc -o Blades_of_Exile_Character_Editor_private.res -O coff 
