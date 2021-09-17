INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
#LINK_DIRECTORIES("../../CVars/" "../../GLConsole/")
ADD_EXECUTABLE(GlfwConsole GlfwConsoleDemo.cpp)
TARGET_LINK_LIBRARIES(GlfwConsole cvars)

