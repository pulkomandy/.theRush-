DEBUG=no
CC=g++
RANLIB=ranlib
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall   -g -I../../3rdparty/RakNet-3.6201/Source -I/usr/include/CEGUI/ -I../ -I../../3rdparty/bullet-2.74/src -I/usr/local/include/fmodex -Ih:\qt\directx\include\ 
	LDFLAGS=-lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=librenderD.a

else
	CFLAGS=-W -Wall -O3 -I../../3rdparty/RakNet-3.6201/Source -I/usr/include/CEGUI/ -I../ -I../../3rdparty/bullet-2.74/src -I/usr/local/include/fmodex -Ih:/qt/directx/include/
	LDFLAGS= -O3 -lm
	AR = ar
	ARFLAGS = rcs
	LIBS = -ltp
	LIB=librender.a
endif


SRC= stdafx.cpp\
	ZDisplayDeviceNULL.cpp\
	ZDisplayDeviceOGL.cpp \
	ZIndexArray.cpp \
	ZVertexArray.cpp \
	ZIndexArrayOGL.cpp \
	ZVertexArrayOGL.cpp \
	ddsOGL.cpp \
	ZRenderQueue.cpp \
	GLee.cpp \
	CEGuiRenderer.cpp \
	FFxData.cpp \
	FFxImport.cpp \
	FFxParam.cpp \
	FFxParamable.cpp \
	FFxPass.cpp \
	FFxProgram.cpp \
	FFxTechnique.cpp \
	ZTexture.cpp \
	ZFx.cpp \
	ZMaterial.cpp \
	ZRenderTargetOGL.cpp
	
	
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
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o
	@rm -f ../../libs/$(LIB)

mrproper: clean
	@rm -rf $(EXEC)
