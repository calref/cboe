include ../.config.vars

CPP  = $(PREFIX)g++
CC   = $(PREFIX)gcc
WINDRES = $(PREFIX)windres
RES  = Blades_of_Exile_Scenario_Editor_private.res
OBJ  = buttonmg.o dlogtool.o global.o graphutl.o keydlgs.o tfileio.o townout.o edsound.o graphics.o scenario.o blscened.o tactions.o $(RES)
LINKOBJ  = buttonmg.o dlogtool.o global.o graphutl.o keydlgs.o tfileio.o townout.o edsound.o graphics.o scenario.o blscened.o tactions.o $(RES)
LIBS =  -mwindows -lwinmm --strip-all -static-libgcc -static-libstdc++
INCS =  -I"include" 
CXXINCS = 
BIN  = "Blades of Exile Scenario Editor.exe"
CXXFLAGS = $(CXXINCS) -fno-exceptions -fno-rtti -std=c++0x -DWIN32
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before "Blades of Exile Scenario Editor.exe" all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "Blades of Exile Scenario Editor.exe" $(LIBS)

buttonmg.o: buttonmg.cpp
	$(CPP) -c buttonmg.cpp -o buttonmg.o $(CXXFLAGS)

dlogtool.o: dlogtool.cpp
	$(CPP) -c dlogtool.cpp -o dlogtool.o $(CXXFLAGS)

global.o: global.cpp
	$(CPP) -c global.cpp -o global.o $(CXXFLAGS)

graphutl.o: graphutl.cpp
	$(CPP) -c graphutl.cpp -o graphutl.o $(CXXFLAGS)

keydlgs.o: keydlgs.cpp
	$(CPP) -c keydlgs.cpp -o keydlgs.o $(CXXFLAGS)

tfileio.o: tfileio.cpp
	$(CPP) -c tfileio.cpp -o tfileio.o $(CXXFLAGS)

townout.o: townout.cpp
	$(CPP) -c townout.cpp -o townout.o $(CXXFLAGS)

edsound.o: edsound.cpp
	$(CPP) -c edsound.cpp -o edsound.o $(CXXFLAGS)

graphics.o: graphics.cpp
	$(CPP) -c graphics.cpp -o graphics.o $(CXXFLAGS)

scenario.o: scenario.cpp
	$(CPP) -c scenario.cpp -o scenario.o $(CXXFLAGS)

blscened.o: blscened.cpp
	$(CPP) -c blscened.cpp -o blscened.o $(CXXFLAGS)

tactions.o: tactions.cpp
	$(CPP) -c tactions.cpp -o tactions.o $(CXXFLAGS)

Blades_of_Exile_Scenario_Editor_private.res: Blades_of_Exile_Scenario_Editor_private.rc STRINGS.RC BLSCENED.RC GAMEDLOG.RC 
	$(WINDRES) -i Blades_of_Exile_Scenario_Editor_private.rc --input-format=rc -o Blades_of_Exile_Scenario_Editor_private.res -O coff  -D_INCLUDED_RC
