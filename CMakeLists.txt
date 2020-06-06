cmake_minimum_required(VERSION 3.12)

project(INF443Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("external" "src")

find_package(glfw3 3.3 REQUIRED)
find_package(assimp REQUIRED)

file(GLOB_RECURSE SRCS "src/*" "external/*" "resources/*")

# taken from https://stackoverflow.com/a/31987079
foreach(FILE ${SRCS})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    
    # Remove common directory prefix to make the group 
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

# disable iterator debugging because it slows down a lot our app
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

add_executable(INF443Project ${SRCS})
target_link_libraries(INF443Project glfw assimp::assimp)
