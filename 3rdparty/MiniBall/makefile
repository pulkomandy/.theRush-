DEBUG=no
CC=g++
RANLIB=ranlib

ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall  -g
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libminiballD.a
else
	CFLAGS=-W -Wall -O3
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=libminiball.a
endif

SRC= 	Point.cpp \
	Quaternion.cpp \
	Matrix.cpp \
	Sphere.cpp \
	Vector.cpp 

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
