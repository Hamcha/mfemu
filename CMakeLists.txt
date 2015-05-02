# Global project settings
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(mfemu)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${mfemu_SOURCE_DIR}/cmake")

# MFEMU
file(GLOB MFEMU_HEADERS src/*.h)
file(GLOB MFEMU_SRC src/*.cpp)
add_executable(${PROJECT_NAME} ${MFEMU_SRC} ${MFEMU_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

# Remove console window on Windows
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(${PROJECT_NAME} PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif(MSVC)