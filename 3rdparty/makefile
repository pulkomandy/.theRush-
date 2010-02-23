export DEBUG=no
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
	@(cd MiniBall && $(MAKE))
	@(cd tinyxml && $(MAKE))
	@(cd zlib-1.2.3 && $(MAKE))

clean:
	@(cd MiniBall && $(MAKE) $@)
	@(cd tinyxml && $(MAKE) $@)
	@(cd zlib-1.2.3 && $(MAKE) $@)

