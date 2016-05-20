# Global project settings
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(mfemu)
set(MAJOR "0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${mfemu_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# MFEMU Core
file(GLOB MFEMU_CORE_HEADERS src/*.h)
file(GLOB MFEMU_CORE_SRC Core/*.cpp Core/MBC/*.cpp)
source_group("Headers" FILES ${MFEMU_HEADERS})
add_library(Core STATIC ${MFEMU_CORE_SRC} ${MFEMU_CORE_HEADERS})

# MFEMU cmd line
file(GLOB MFEMU_LAUNCHER launcher/*.cpp)
add_executable(${PROJECT_NAME}_cmd ${MFEMU_LAUNCHER})
target_link_libraries(${PROJECT_NAME}_cmd Core)

# MFEMU tests
file(GLOB MFEMU_TEST test/*.cpp)
add_executable(${PROJECT_NAME}_test ${MFEMU_TEST} ${MFEMU_HEADERS})
target_link_libraries(${PROJECT_NAME}_test Core)

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(Core PRIVATE cxx_range_for)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(Core ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(Core ${GLEW_LIBRARIES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(Core ${SDL2_LIBRARY})

# Straight from dolphin-emu/dolphin
include(FindGit OPTIONAL)
if(GIT_FOUND)
    execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
        OUTPUT_VARIABLE CURRENTREV
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "(-0)?-[^-]+((-dirty)?)$" "\\2" CURRENTREV "${CURRENTREV}")
else()
    set(CURRENTREV "unknown")
endif()

# Set version's DEFINE
add_definitions(-DVERSION=${MAJOR} -DCOMMIT=\"${CURRENTREV}\" -DDEBUG_OPS=1 -DDEBUG_ROM=1)

# Enable as many warnings as possible
if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# Remove console window on Windows
#if(MSVC)
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
#    set_target_properties(${PROJECT_NAME}_cmd PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
#endif(MSVC)
