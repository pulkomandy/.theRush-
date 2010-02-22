export DEBUG=yes
export CC=g++
export RANLIB=ranlib
ifeq ($(DEBUG),yes)
	export CFLAGS=-W -Wall -ansi -pedantic -g -DLINUX
	export LDFLAGS=-lm
	export AR = ar
	export ARFLAGS = rcs
	export LIBS = -ltp
else
	export CFLAGS=-W -Wall -ansi -pedantic -O3 -DLINUX
	export LDFLAGS= -O3 -lm
	export AR = ar
	export ARFLAGS = rcs
	export LIBS = -ltp
endif

all: 
	@(cd libbase && $(MAKE))
	@(cd libplatform && $(MAKE))
	@(cd librender && $(MAKE))
	@(cd libworld && $(MAKE))
	@(cd libgame && $(MAKE))
	@(cd testunit && $(MAKE))

clean:
	@(cd libbase && $(MAKE) $@)
	@(cd libplatform && $(MAKE) $@)
	@(cd librender && $(MAKE) $@)                   
	@(cd libworld && $(MAKE) $@)
	@(cd libgame && $(MAKE) $@)
	@(cd testunit && $(MAKE) $@)
