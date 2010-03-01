DEBUG=no
CC=g++
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -ansi  -g -DLINUX -I../librender -I/usr/include/CEGUI/ -I/usr/include -I../../3rdparty/RakNet-3.6201/Source -I../../3rdparty/bullet-2.74/src -I../../3rdparty/zlib-1.2.3 -I/usr/local/include/fmodex
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libplatformD.a

else
	CFLAGS=-W -Wall -ansi  -O3 -DLINUX -I../librender -I/usr/include/CEGUI/ -I/usr/include -I../../3rdparty/RakNet-3.6201/Source -I../../3rdparty/bullet-2.74/src -I../../3rdparty/zlib-1.2.3 -I/usr/local/include/fmodex
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libplatform.a
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
	@rm -f ../../libs/$(LIB)

mrproper: clean
	@rm -rf $(EXEC)
