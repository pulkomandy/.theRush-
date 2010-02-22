DEBUG=no
CC=g++
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -ansi  -g -DLINUX -I../librender -I../../3rdparty/ -I/usr/include -I../../3rdparty/tinyxml -I../../3rdparty/RakNet-3.6201/Source -I../../3rdparty/bullet-2.74/src -I/usr/local/include/fmodex
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libworldD.a

else
	CFLAGS=-W -Wall -ansi  -O3 -DLINUX -I../librender -I../../3rdparty/ -I/usr/include -I../../3rdparty/tinyxml -I../../3rdparty/RakNet-3.6201/Source -I../../3rdparty/bullet-2.74/src -I/usr/local/include/fmodex
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libworld.a
endif


SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: $(LIB)
ifeq ($(DEBUG),yes)
	@echo "Generating debug"
else
	@echo "Generating release"
endif

$(LIB): $(OBJ)
	$(AR) r ../../libs/$(LIB) $(OBJ)
	$(RANLIB) ../../libs/$(LIB)


%.o: %.cpp
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o
	@rm ../../libs/$(LIB)

mrproper: clean
	@rm -rf $(EXEC)
