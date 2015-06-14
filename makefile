# cboe makefile

CC=g++
CFLAGS=-ferror-limit=0 -Werror=format -ftemplate-backtrace-limit=0 -Werror=return-type -Werror=parentheses -Werror=dangling-else -isystem/usr/local/Cellar/boost/1.58.0/include -Wfloat-equal -Wno-long-long -std=c++11 -stdlib=libc++ -lc++
LIBFLAGS  = -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.58.0/lib -lobjc -lz -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -framework Cocoa -framework CoreFoundation -framework OpenGL
CINCLUDES=-Isrc -Isrc/classes -Isrc/dialogxml -Isrc/dialogxml/xml-parser -Isrc/tools -Isrc/tools/gzstream -Isrc/tools/resmgr
SRC=
OBJ=$(SRC:.cpp=.o)
#TGT=CBoE

LIB=/usr/local/lib

boost_lib=-lboost_filesystem -lboost_system -L$LIB
boost_include=-I/usr/local/include/boost

game:
	$(CC) $(CFLAGS) $(CINCLUDES) $(SRC) -o obj $(LIBFLAGS)
	
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


