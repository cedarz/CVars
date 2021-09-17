INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
#LINK_DIRECTORIES("../../CVars/" "../../GLConsole/")
ADD_EXECUTABLE(GlfwConsole GlfwConsoleDemo.cpp)
set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT GlfwConsole)
TARGET_LINK_LIBRARIES(GlfwConsole glfw cvars)

