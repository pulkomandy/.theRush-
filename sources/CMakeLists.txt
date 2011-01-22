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
