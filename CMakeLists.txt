CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(MegRest)

# version number
set( LibraryVersion "0.1" )
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

# Minimum compiler version
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++")
endif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-lstdc++fs")

# https://github.com/rest-for-physics/framework/issues/162 & https://github.com/rest-for-physics/framework/issues/236
if (${CMAKE_VERSION} VERSION_LESS "3.23")
    cmake_policy(SET CMP0082 OLD)
endif ()

# REST framework
find_package(REST REQUIRED)

# global variables to hold external dependecies (REST, ROOT, Geant4, Garfield etc.)
set(external_include_dirs ${REST_INCLUDE_DIRS})
set(external_libs "${REST_LIBRARIES}")

# Install path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${REST_PATH} CACHE PATH "" FORCE)
endif()

if (NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "${REST_PATH}")
  message(FATAL_ERROR "Don't set install prefix yourself. MegRest has to be installed under REST directory: ${REST_PATH}")
endif()

# setup cmake module path
set(CMAKE_MODULE_PATH ${REST_PATH}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_MACOSX_RPATH 1)

#####  ROOT libs and includes  #####
set(ROOT_REQUIRED_LIBRARIES Core RIO Geom Gdml Minuit Spectrum XMLIO Eve RGL Gui)

# Auto schema evolution for ROOT
if (NOT DEFINED REST_SE)
    set(REST_SE ON)
endif ()
set(SCHEMA_EVOLUTION ${REST_SE})

find_package(ROOT REQUIRED COMPONENTS ${ROOT_REQUIRED_LIBRARIES})

message(STATUS "ROOT LIBRARIES: ${ROOT_LIBRARIES}")

set(ROOTCINT_EXECUTABLE ${ROOT_rootcint_CMD})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(external_libs "${external_libs};Ws2_32")
    set(CMAKE_CXX_STANDARD 17)
    add_definitions(-D_HAS_STD_BYTE=0)
else ()
    set(external_libs "${external_libs};stdc++fs")

    execute_process(COMMAND root-config --has-cxx20 OUTPUT_VARIABLE HASCXX20)
    execute_process(COMMAND root-config --has-cxx17 OUTPUT_VARIABLE HASCXX17)
    execute_process(COMMAND root-config --has-cxx14 OUTPUT_VARIABLE HASCXX14)
    execute_process(COMMAND root-config --has-cxx11 OUTPUT_VARIABLE HASCXX11)

    execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS)

    set(CMAKE_CXX_STANDARD_REQUIRED YES)

    if (${HASCXX20} MATCHES "yes")
        set(CMAKE_CXX_STANDARD 20)
    elseif (${HASCXX17} MATCHES "yes")
        set(CMAKE_CXX_STANDARD 17)
    else ()
        message(FATAL_ERROR "Minimum C++ standard version is 17,
                while the root version is compiled with an older
                C++ standard, check root-config --cflags ${ROOT_CFLAGS}"
                )
    endif ()
endif ()

set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})
set(external_libs "${external_libs};${ROOT_LIBRARIES}")

# Include directories for compilation
include_directories(${external_include_dirs})
message(STATUS "external include directory: " ${external_include_dirs})
message(STATUS "external libraries to link: " ${external_libs})

# Gather some information
string(TIMESTAMP date "%Y-%m-%d %H:%M")

# Enable testing (-DTEST=ON flag, it is OFF by default)
include(RestTesting)

# include REST's ROOT macro
include(RestRootMacro)

# config compiling
set(contents generator) # list of sub-directories

# special src which does not follow the convension
set(addon_inc)
set(addon_src)
set(addon_CINT)

# '-fPIC' flag necessary on some systems to prevent error
add_compile_options("-fPIC")
link_libraries("-fPIC")

# set(external_libs "${external_libs};-lstdc++fs") # sometimes needed for <filesystem>

COMPILEDIR(MegRest)

# ADD_LIBRARY_TEST()

### installation ###
file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${Headers} DESTINATION include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
  DESTINATION examples/MegRest
  COMPONENT install
)
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
#   DESTINATION .
#   COMPONENT install
# )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION data/MegRest
  COMPONENT install
)

message(STATUS "include directories: ${external_include_dirs}")
message(STATUS "libraries: ${external_libs}")
