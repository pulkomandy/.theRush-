set(SOURCE_FILES 
  ZCEGui.cpp
  ZDirectInput8.cpp
  ZenFWDecoderNode.cpp
  ZenFWGame.cpp
  ZenFWLane.cpp
  ZenFWLoaderNode.cpp
  ZenFWManager.cpp
  ZenFWRenderer.cpp
  ZenFWSDLWindow.cpp
  ZenFWViewer.cpp
  ZenFWVRAMService.cpp
  ZInputSDL.cpp
  ZSoundFMOD.cpp
)

add_library(
	platform
	${SOURCE_FILES}
)

