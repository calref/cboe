include ../.config.vars

CPP  = $(PREFIX)g++
CC   = $(PREFIX)gcc
WINDRES = $(PREFIX)windres
RES  = Blades_of_Exile_private.res
OBJ  = boe.actions.o boe.main.o boe.combat.o boe.dlgutil.o boe.fields.o boe.fileio.o global.o boe.graphics.o boe.graphutil.o boe.infodlg.o boe.itemdata.o boe.items.o boe.locutils.o boe.monster.o boe.newgraph.o boe.party.o boe.specials.o boe.text.o boe.town.o globvar.o tools/soundtool.o tools/soundvars.o tools/dlogtool.o tools/mathutil.o classes/item.o classes/location.o classes/pc.o $(RES)
LINKOBJ  = boe.actions.o boe.main.o boe.combat.o boe.dlgutil.o boe.fields.o boe.fileio.o global.o boe.graphics.o boe.graphutil.o boe.infodlg.o boe.itemdata.o boe.items.o boe.locutils.o boe.monster.o boe.newgraph.o boe.party.o boe.specials.o boe.text.o boe.town.o globvar.o tools/soundtool.o tools/soundvars.o tools/dlogtool.o tools/mathutil.o classes/item.o classes/location.o classes/pc.o $(RES)
LIBS = -mwindows -lwinmm -static-libgcc -static-libstdc++
INCS =  -I"include" 
CXXINCS = 
BIN  = "Blades of Exile.exe"
CXXFLAGS = $(CXXINCS) -Wall -O2 -v
CFLAGS = $(INCS)   -O2
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before "Blades of Exile.exe" all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "Blades of Exile.exe" $(LIBS)

boe.actions.o: boe.actions.cpp
	$(CPP) -c boe.actions.cpp -o boe.actions.o $(CXXFLAGS)

boe.main.o: boe.main.cpp
	$(CPP) -c boe.main.cpp -o boe.main.o $(CXXFLAGS)

boe.combat.o: boe.combat.cpp
	$(CPP) -c boe.combat.cpp -o boe.combat.o $(CXXFLAGS)

boe.dlgutil.o: boe.dlgutil.cpp
	$(CPP) -c boe.dlgutil.cpp -o boe.dlgutil.o $(CXXFLAGS)

boe.fields.o: boe.fields.cpp
	$(CPP) -c boe.fields.cpp -o boe.fields.o $(CXXFLAGS)

boe.fileio.o: boe.fileio.cpp
	$(CPP) -c boe.fileio.cpp -o boe.fileio.o $(CXXFLAGS)

global.o: global.cpp
	$(CPP) -c global.cpp -o global.o $(CXXFLAGS)

boe.graphics.o: boe.graphics.cpp
	$(CPP) -c boe.graphics.cpp -o boe.graphics.o $(CXXFLAGS)

boe.graphutil.o: boe.graphutil.cpp
	$(CPP) -c boe.graphutil.cpp -o boe.graphutil.o $(CXXFLAGS)

boe.infodlg.o: boe.infodlg.cpp
	$(CPP) -c boe.infodlg.cpp -o boe.infodlg.o $(CXXFLAGS)

boe.itemdata.o: boe.itemdata.cpp
	$(CPP) -c boe.itemdata.cpp -o boe.itemdata.o $(CXXFLAGS)

boe.items.o: boe.items.cpp
	$(CPP) -c boe.items.cpp -o boe.items.o $(CXXFLAGS)

boe.locutils.o: boe.locutils.cpp
	$(CPP) -c boe.locutils.cpp -o boe.locutils.o $(CXXFLAGS)

boe.monster.o: boe.monster.cpp
	$(CPP) -c boe.monster.cpp -o boe.monster.o $(CXXFLAGS)

boe.newgraph.o: boe.newgraph.cpp
	$(CPP) -c boe.newgraph.cpp -o boe.newgraph.o $(CXXFLAGS)

boe.party.o: boe.party.cpp
	$(CPP) -c boe.party.cpp -o boe.party.o $(CXXFLAGS)

boe.specials.o: boe.specials.cpp
	$(CPP) -c boe.specials.cpp -o boe.specials.o $(CXXFLAGS)

boe.text.o: boe.text.cpp
	$(CPP) -c boe.text.cpp -o boe.text.o $(CXXFLAGS)

boe.town.o: boe.town.cpp
	$(CPP) -c boe.town.cpp -o boe.town.o $(CXXFLAGS)

globvar.o: globvar.cpp
	$(CPP) -c globvar.cpp -o globvar.o $(CXXFLAGS)

tools/soundtool.o: tools/soundtool.cpp
	$(CPP) -c tools/soundtool.cpp -o tools/soundtool.o $(CXXFLAGS)

tools/soundvars.o: tools/soundvars.cpp
	$(CPP) -c tools/soundvars.cpp -o tools/soundvars.o $(CXXFLAGS)

tools/dlogtool.o: tools/dlogtool.cpp
	$(CPP) -c tools/dlogtool.cpp -o tools/dlogtool.o $(CXXFLAGS)

tools/mathutil.o: tools/mathutil.cpp
	$(CPP) -c tools/mathutil.cpp -o tools/mathutil.o $(CXXFLAGS)

classes/item.o: classes/item.cpp
	$(CPP) -c classes/item.cpp -o classes/item.o $(CXXFLAGS)

classes/location.o: classes/location.cpp
	$(CPP) -c classes/location.cpp -o classes/location.o $(CXXFLAGS)

classes/pc.o: classes/pc.cpp
	$(CPP) -c classes/pc.cpp -o classes/pc.o $(CXXFLAGS)

Blades_of_Exile_private.res: Blades_of_Exile_private.rc BLADES.RC GAMEDLOG.RC GAMESTR.RC 
	$(WINDRES) -i Blades_of_Exile_private.rc --input-format=rc -o Blades_of_Exile_private.res -O coff -D_INCLUDED_RC
