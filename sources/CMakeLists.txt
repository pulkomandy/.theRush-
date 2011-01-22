cmake_minimum_required(VERSION 2.6) # Pour éviter un warning au lancement de CMake, on doit préciser la version minimale

# Nom du projet
project(TheRush)

# ???
set(SOURCE_FILES 
  CMakeFiles/CompilerIdC/CMakeCCompilerId.c
  CMakeFiles/CompilerIdCXX/CMakeCXXCompilerId.cpp
)

# Make sure the PLATFORM_HEADER_FILES variable is always defined
 set( PLATFORM_HEADER_FILES "" )

# Make sure the PLATFORM_SOURCE_FILES variable is always defined
 set( PLATFORM_SOURCE_FILES "" )

IF (WIN32)
  SET(BULLET_ROOT "h:\\Program files\\BULLET_PHYSICS\\")
  Find_Package(Bullet REQUIRED)
  Find_Package(DirectX REQUIRED)
  Find_Package(FMOD REQUIRED)
ELSE (WIN32)
  set(BULLET_ROOT /usr)
  set(BULLET_INCLUDE_DIRS ${BULLET_ROOT}/include/bullet)
  set(FMOD_INCLUDE_DIR /usr/include/fmodex)
ENDIF (WIN32)

include_directories(
	${BULLET_INCLUDE_DIRS}
	${DX9_INCLUDE_PATH}
	${FMOD_INCLUDE_DIR}
	../3rdparty/tinyxml
	.
)
link_directories(/usr/lib)

 add_subdirectory(libbase)
 add_subdirectory(libedit)
 add_subdirectory(libgame)
 add_subdirectory(libplatform)
 add_subdirectory(librender)
 add_subdirectory(libworld)
 add_subdirectory(metameta)
 add_subdirectory(rushed)
 add_subdirectory(testunit)
 add_subdirectory(therush)
