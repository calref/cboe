
CC        = gcc
CPP       = g++
RM        = rm -f
CPPFLAGS  = -ferror-limit=0 -Werror=format -ftemplate-backtrace-limit=0 -Werror=return-type -Werror=parentheses -Werror=dangling-else -isystem/usr/local/Cellar/boost/1.57.0/include -Wfloat-equal -Wno-long-long
LDFLAGS   = -lboost_filesystem -lboost_system -L/usr/local/Cellar/boost/1.57.0/lib

SRCS      = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS      = $(subst .cc,.o,$(SRCS))

all: tool

tool: $(OBJS)
    $(CPP) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
    rm -f ./.depend
    $(CPP) $(CPPFLAGS) -MM $^>>./.depend;

clean:
    $(RM) $(OBJS)

dist-clean: clean
    $(RM) *~ .depend

include .depend

