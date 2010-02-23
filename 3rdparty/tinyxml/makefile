DEBUG=no
CC=g++
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall  -g -DLINUX
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=tinyxmlD.a

else
	CFLAGS=-W -Wall -O3 -DLINUX
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=tinyxml.a
endif


SRC= 	tinystr.cpp \
	tinyxml.cpp \
	tinyxmlparser.cpp \
	tinyxmlerror.cpp
	

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
