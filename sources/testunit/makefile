DEBUG=no
CC=g++
BULLETLIBPATHS=../../3rdparty/bullet-2.74/lib/
ifeq ($(DEBUG),yes)
	CFLAGS=-W -Wall -g -I../libbase -I../libplatform -I../librender -I../../3rdparty/bullet-2.74/src -I../../3rdparty/RakNet-3.6201/Source -I/usr/local/include/fmodex -Ih:/Qt/directx/include
	LDFLAGS=
	DEBUGSUFFIX=D
	
else
	CFLAGS=-W -Wall -O3 -I../libbase -I../libplatform -I../librender -I../../3rdparty/bullet-2.74/src -I../../3rdparty/RakNet-3.6201/Source -I/usr/local/include/fmodex -Ih:/Qt/directx/include
	LDFLAGS=
	DEBUGSUFFIX=
endif
EXEC=testunit
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)
ifeq ($(DEBUG),yes)
	@echo "Generating debug"
else
	@echo "Generating release"
endif

testunit: $(OBJ)
	$(CC) -o ../../bin/$@ $^   ../../libs/libworld$(DEBUGSUFFIX).a ../../libs/libgame$(DEBUGSUFFIX).a ../../libs/libbase$(DEBUGSUFFIX).a ../../libs/libplatform$(DEBUGSUFFIX).a  ../../libs/libworld$(DEBUGSUFFIX).a ../../libs/miniball.a  ../../libs/libbase$(DEBUGSUFFIX).a ../../libs/librender$(DEBUGSUFFIX).a  $(LDFLAGS) -lSDL $(BULLETLIBPATHS)libbulletdynamics.a $(BULLETLIBPATHS)libbulletcollision.a $(BULLETLIBPATHS)libGIMPACTUtils.a $(BULLETLIBPATHS)libBulletColladaConverter.a $(BULLETLIBPATHS)libbulletsoftbody.a $(BULLETLIBPATHS)libcolladadom.a $(BULLETLIBPATHS)libconvexdecomposition.a ../../libs/zlib.a ../../libs/tinyxml.a ../../libs/libgame$(DEBUGSUFFIX).a ../../libs/libworld$(DEBUGSUFFIX).a -lraknet -lWs2_32 $(BULLETLIBPATHS)liblinearmath.a -lcg -lcggl -lopengl32 ../../libs/tinyxml.a -lfmodex -lceguibase -lceguitinyxmlparser -lwinmm

TestUnit.o: TestUnit.cpp

%.o: %.cpp
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
