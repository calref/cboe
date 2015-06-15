# oboe makefile

CC=clang++
CFLAGS=-ferror-limit=0 -Werror=format -ftemplate-backtrace-limit=0 -Werror=return-type -Werror=parentheses -Werror=dangling-else -isystem/usr/local/Cellar/boost/1.58.0/include -Wfloat-equal -Wno-long-long -std=c++11 -stdlib=libc++
LIBFLAGS  = -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.58.0/lib -lobjc -lz -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lc++ -framework Cocoa -framework CoreFoundation -framework OpenGL
CINCLUDES=-Isrc -Isrc/classes -Isrc/dialogxml -Isrc/dialogxml/xml-parser -Isrc/tools -Isrc/tools/gzstream -Isrc/tools/resmgr

LIB=/usr/local/lib

boost_lib=-lboost_filesystem -lboost_system -L$LIB
boost_include=-I/usr/local/include/boost

include $(PLATFORM).make

all: game pced scened

obj/gzstream.o: src/tools/gzstream/gzstream.cpp src/tools/gzstream/gzstream.h
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/oldstructs.o: src/oldstructs.cpp src/oldstructs.hpp
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/tools_%.o: src/tools/%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/tools_%.o: src/tools/%.mm 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/class_%.o: src/classes/%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/dialog_%.o: src/dialogxml/%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/tinyxml%.o: src/dialogxml/xml-parser/tinyxml%.cpp
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS) -DTIXML_USE_TICPP

obj/ti%.o: src/dialogxml/xml-parser/ti%.cpp src/dialogxml/xml-parser/ti%.h
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS) -DTIXML_USE_TICPP

obj/boe.%.o: src/boe.%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/boe.%.o: src/boe.%.mm 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/pc.%.o: src/pcedit/pc.%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/pc.%.o: src/pcedit/pc.%.mm 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/scen.%.o: src/scenedit/scen.%.cpp 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/scen.%.o: src/scenedit/scen.%.mm 
	$(CC) -c $(CINCLUDES) -o $@ $< $(CFLAGS)

obj/common.o: obj/gzstream.o obj/oldstructs.o $(COMMON_SRC)
	ld -r $^ -o obj/common.o

exe/bin/boe: obj/common.o $(BOE_SRC)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o exe/bin/boe $(LIBFLAGS)

exe/bin/pced: obj/common.o $(PC_SRC)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o exe/bin/pced $(LIBFLAGS)

exe/bin/scened: obj/common.o $(ED_SRC)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o exe/bin/scened $(LIBFLAGS)

game: exe/bin/boe resources game-menus

pced: exe/bin/pced resources pc-menus

scened: exe/bin/scened resources scen-menus bases

resources: sounds.exa graphics.exd strings dialogs fonts scenarios shaders

shaders: src/tools/mask.vert src/tools/mask.frag
	mkdir -p 'exe/Blades of Exile/data/shaders'
	cp -fp src/tools/mask.* 'exe/Blades of Exile/data/shaders/'

sounds.exa:
	cp -Rfp rsrc/sounds.exa 'exe/Blades of Exile/Scenario Editor/'

graphics.exd:
	cp -Rfp rsrc/graphics.exd 'exe/Blades of Exile/Scenario Editor/'

strings:
	cp -Rfp rsrc/strings/ 'exe/Blades of Exile/data/'

dialogs:
	mkdir -p 'exe/Blades of Exile/data/dialogs'
	cp -fp rsrc/dialogs/*.xml 'exe/Blades of Exile/data/dialogs/'

fonts:
	cp -Rfp rsrc/fonts/ 'exe/Blades of Exile/data/'

scenarios:
	cp -fp 'rsrc/Blades of Exile Scenarios'/*.{exs,meg} 'exe/Blades of Exile/Blades of Exile Scenarios/'

bases:
	mkdir -p 'exe/Blades of Exile/Scenario Editor/Blades of Exile Base'
	cp -fp 'rsrc/Blades of Exile Bases'/*.exs 'exe/Blades of Exile/Scenario Editor/Blades of Exile Base'

clean:
	rm -rf obj exe/bin

