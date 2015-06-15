# oboe makefile

CC=clang++
CFLAGS=-ferror-limit=0 -Werror=format -ftemplate-backtrace-limit=0 -Werror=return-type -Werror=parentheses -Werror=dangling-else -isystem/usr/local/Cellar/boost/1.58.0/include -Wfloat-equal -Wno-long-long -std=c++11 -stdlib=libc++
LIBFLAGS  = -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.58.0/lib -lobjc -lz -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lc++ -framework Cocoa -framework CoreFoundation -framework OpenGL
CINCLUDES=-Isrc -Isrc/classes -Isrc/dialogxml -Isrc/dialogxml/xml-parser -Isrc/tools -Isrc/tools/gzstream -Isrc/tools/resmgr

LIB=/usr/local/lib

boost_lib=-lboost_filesystem -lboost_system -L$LIB
boost_include=-I/usr/local/include/boost

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

game: obj/common.o $(BOE_SRC)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o exe/boe $(LIBFLAGS)
	
#main: $(SRCS)
#	$(CC) $(CFLAGS) -c $(SRC) -o objs $(OBJS) $(LIBFLAGS)

#.cpp.o:
#	$(CC) $(CFLAGS) $< -o $@

clean:
	\rm obj/*.o 

#all: $(SRCS) $(TGT)
#	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(OBJS) $(LIBFLAGS)
#$(TGT): $(OBJS)
#	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(OBJS) $(LIBFLAGS)
#	$(CC) $(OBJS) -o $@


#all: clean common mac win unx
#mac: mac_game mac_scen mac_ched
#win: win_game win_scen win_ched
#unx: unx_game unx_scen unx_ched

#common: $(OBJS)
#    $(CPP) $(CPPFLAGS) -o tool $(OBJS) $(LIBFLAGS)


