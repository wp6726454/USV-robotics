# mkdir build
# cd build
# cmake ..
# make

cmake_minimum_required(VERSION 3.5.1)
project (SQLlitetest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#For the shared library:
#set ( PROJECT_LINK_LIBS libtestStudent.so )
#link_directories( ~/exploringBB/extras/cmake/studentlib_shared/build )

#For the static library:
#set ( PROJECT_LINK_LIBS libcrc.a )
#link_directories( ../libcrc/lib )

#Bring the headers and source files into the project
#include_directories(../include)

# If necessary, use the RELATIVE flag, otherwise each source file may be listed
# with full pathname. RELATIVE may makes it easier to extract an executable name
# automatically.
set(APP_SOURCES mydbtest.cpp )

foreach( testsourcefile ${APP_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    add_executable( ${testname} ${testsourcefile} )
    # Make sure PROJECT_LINK_LIBS is linked to each app
    target_link_libraries( ${testname} sqlite3 )
endforeach( testsourcefile ${APP_SOURCES} )
