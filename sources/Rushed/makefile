DEBUG=no
CC=g++
BULLETLIBPATHS=../../3rdparty/bullet-2.74/lib/
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -ansi  -g -DLINUX -I../libbase -I../libplatform -I../librender -I../../3rdparty/bullet-2.74/src -I../../3rdparty/RakNet-3.6201/Source -I/usr/local/include/fmodex `wx-config --cflags`
	LDFLAGS=
	DEBUGSUFFIX=D
	
else
	CFLAGS=-W -Wall -ansi  -O3 -DLINUX -I../libbase -I../libplatform -I../librender -I../../3rdparty/bullet-2.74/src -I../../3rdparty/RakNet-3.6201/Source -I/usr/local/include/fmodex `wx-config --cflags`
	LDFLAGS=
	DEBUGSUFFIX=
endif
EXEC=rushed
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)
ifeq ($(DEBUG),yes)
	@echo "Generating debug"
else
	@echo "Generating release"
endif

testunit: $(OBJ)
	$(CC) -o ../../bin/$@ $^   ../../libs/libworld$(DEBUGSUFFIX).a ../../libs/libgame$(DEBUGSUFFIX).a ../../libs/libbase$(DEBUGSUFFIX).a ../../libs/libplatform$(DEBUGSUFFIX).a  ../../libs/libworld$(DEBUGSUFFIX).a ../../libs/miniball.a  ../../libs/libbase$(DEBUGSUFFIX).a ../../libs/librender$(DEBUGSUFFIX).a  $(LDFLAGS) -lrt -lSDL -lGL -lCEGUIBase -lCEGUITinyXMLParser -lCEGUIFalagardWRBase -lCEGUIOpenGLRenderer -lCg -lCgGL $(BULLETLIBPATHS)libbulletdynamics.a $(BULLETLIBPATHS)libbulletcollision.a $(BULLETLIBPATHS)libGIMPACTUtils.a $(BULLETLIBPATHS)libbulletmath.a $(BULLETLIBPATHS)libbulletmultithreaded.a $(BULLETLIBPATHS)libBulletColladaConverter.a $(BULLETLIBPATHS)libbulletsoftbody.a $(BULLETLIBPATHS)libcolladadom.a $(BULLETLIBPATHS)libconvexdecomposition.a $(BULLETLIBPATHS)libiff.a $(BULLETLIBPATHS)liblibxml.a ../../libs/zlib.a ../../libs/tinyxml.a ../../libs/libgame$(DEBUGSUFFIX).a ../../libs/libworld$(DEBUGSUFFIX).a -lraknet -lfmodex64 `wx-config --libs`
	

rushed.o: rushed.cpp

%.o: %.cpp
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
