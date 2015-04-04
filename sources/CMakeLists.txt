cmake_minimum_required(VERSION 2.6) 
	# Pour éviter un warning au lancement de CMake, on doit préciser la version minimale

project(TheRush)

# Tell cmake where to find additional modules.
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Make sure the PLATFORM_HEADER_FILES variable is always defined
set( PLATFORM_HEADER_FILES "" )

# Make sure the PLATFORM_SOURCE_FILES variable is always defined
set( PLATFORM_SOURCE_FILES "" )

IF (WIN32)
  Find_Package(DirectX REQUIRED)
ENDIF (WIN32)

Find_Package(Bullet REQUIRED)
Find_Package(FMOD REQUIRED)
Find_Package(CEGUI REQUIRED)

include_directories(
	${BULLET_INCLUDE_DIRS}
	${DX9_INCLUDE_PATH}
	${FMOD_INCLUDE_DIR}
	${CEGUI_INCLUDE_DIR}
	../3rdparty/RakNet/Source
	../3rdparty/tinyxml
    ../3rdparty
	.
)

 add_subdirectory(libbase)
 add_subdirectory(libedit)
 add_subdirectory(libgame)
 add_subdirectory(libplatform)
 add_subdirectory(librender)
 add_subdirectory(libworld)
 add_subdirectory(rushed)
 add_subdirectory(testunit)
 add_subdirectory(therush)
