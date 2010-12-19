DEBUG=no
CC=g++
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -g -I../librender -I/usr/include/CEGUI/ -I/usr/include -I../../3rdparty/bullet-2.74/src -I../../3rdparty/zlib-1.2.3 -I../../3rdparty/tinyxml -I../../3rdparty/RakNet-3.6201/Source -Ih:/Qt/FMOD/api/Inc/ -Ih:/Qt/directx/Include/
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libgameD.a

else
	CFLAGS=-W -Wall -O3 -I../librender -I/usr/include/CEGUI/ -I/usr/include -I../../3rdparty/bullet-2.74/src -I../../3rdparty/zlib-1.2.3 -I../../3rdparty/tinyxml -I../../3rdparty/RakNet-3.6201/Source -Ih:/Qt/FMOD/api/Inc -Ih:/Qt/directx/include/
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libgame.a
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
