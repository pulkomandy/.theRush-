DEBUG=no
CC=gcc
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall  -g -DLINUX
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=zlibD.a

else
	CFLAGS=-W -Wall -O3 -DLINUX
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=zlib.a
endif


SRC= 	adler32.c \
	zutil.c \
	uncompr.c \
	trees.c \
	inftrees.c \
		inflate.c \
		inffast.c \
		infback.c \
		gzio.c \
		deflate.c \
		crc32.c \
		compress.c 
	

OBJ= $(SRC:.c=.o)

all: $(LIB)
ifeq ($(DEBUG),yes)
	@echo "Generating debug"
else
	@echo "Generating release"
endif

$(LIB): $(OBJ)
	$(AR) r ../../libs/$(LIB) $(OBJ)
	$(RANLIB) ../../libs/$(LIB)


%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o
	@rm ../../libs/$(LIB)

mrproper: clean
	@rm -rf $(EXEC)
