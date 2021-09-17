cmake_minimum_required(VERSION 3.15)

# Be sure to change these with each update
SET(VERSION_MAJOR 2)
SET(VERSION_MINOR 6)
SET(VERSION_PATCH 0)
SET(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

option(BUILD_SAMPLES "wether to build samples" OFF)

project(CVars)

add_subdirectory(deps/glfw)

# Set version values in config.h  (see config.h.in)
configure_file(src/config.h.in ${CMAKE_SOURCE_DIR}/include/cvars/config.h IMMEDIATE)

# Build cvars library
set(DEP_DIR ${CMAKE_SOURCE_DIR}/deps)
set(GLAD_DIR ${DEP_DIR}/glad)
set(GLAD_INCLUDE_DIR ${GLAD_DIR}/include)
set(GLAD_SRC_DIR ${GLAD_DIR}/src)

set(GLFW_INCLUDE_DIR ${DEP_DIR}/glfw/include)
set(TINYXML2_DIR ${DEP_DIR}/tinyxml2)

include_directories(
    ${DEP_DIR}
    ${TINYXML2_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${GLAD_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
)

file(GLOB CVAR_HDRS "${CMAKE_SOURCE_DIR}/include/cvars/*.h")
file(GLOB CVAR_SRCS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(APPEND CVAR_SRCS  ${GLAD_SRC_DIR}/glad.c ${TINYXML2_DIR}/tinyxml2.cpp)

add_definitions(-DTIXML_USE_STL)

include_directories(${USER_INC})

add_library(cvars STATIC ${CVAR_SRCS} ${CVAR_HDRS})
# target_link_libraries(cvars ${LINK_LIBS})
# target_link_libraries(cvars glfw)
set_target_properties(cvars PROPERTIES VERSION "${VERSION}" SOVERSION "${VERSION}")

# Headers for GLConsole
set(GLCONSOLE_HEADERS
        include/GLConsole/GLFont.h
        include/GLConsole/GLColor.h
        include/GLConsole/GLConsole.h
        include/GLConsole/GLConsoleFunction.h
)


if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
