# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DPSim CXX C)

# Put executables into a bin
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_MACOSX_RPATH 1)  # only needed to supress cmake warning

# Command line options
option(DEBUG_IN "Output massive initializing debugging info" OFF)
option(DEBUG_TR "Output massive tracking debugging info" OFF)
option(DEBUG_IO "Output massive I/O debugging info" OFF)
option(DEBUG    "General debug flag for non-production compiles" ON)
option(LIBONLY  "Only build the shared library for the ROOT event structure" OFF)

# STRING options
set(EXTRAS  ""  CACHE STRING "Extra executables needed to be compiled")
string(REPLACE "," ";" EXTRALIST ${EXTRAS})

if(DEBUG_IN)
  add_definitions(-DDEBUG_IN)
endif()
if(DEBUG_TR)
  add_definitions(-DDEBUG_TR)
endif()
if(DEBUG_IO)
  add_definitions(-DDEBUG_IO)
endif()
if(NOT(DEBUG_IN OR DEBUG_TR OR DEBUG_IO OR DEBUG))
  add_definitions(-DNDEBUG)
endif()

# Add version number from git if exists
if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
  execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND git rev-parse --short HEAD      OUTPUT_VARIABLE GIT_HASH   OUTPUT_STRIP_TRAILING_WHITESPACE)
  add_definitions("-DGIT_VERSION=\"${GIT_BRANCH}-${GIT_HASH}\"")
else()
  add_definitions("-DGIT_VERSION=\"1.0.0\"")
endif()

# ROOT dict generation
add_custom_command (
  OUTPUT ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.h
  #DEPENDES ${PROJECT_SOURCE_DIR}/inc/DPMCRawEvent.h ${PROJECT_SOURCE_DIR}/inc/DPMCRawEventLinkDef.h
  COMMAND rootcint
  ARGS -f ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx -c ${PROJECT_SOURCE_DIR}/inc/DPMCRawEvent.h ${PROJECT_SOURCE_DIR}/inc/DPMCRawEventLinkDef.h
  )

add_custom_command (
  OUTPUT ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.h
  #DEPENDES ${PROJECT_SOURCE_DIR}/inc/DPSimConfig.h ${PROJECT_SOURCE_DIR}/inc/DPSimConfigLinkDef.h
  COMMAND rootcint
  ARGS -f ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx -c ${PROJECT_SOURCE_DIR}/inc/DPSimConfig.h ${PROJECT_SOURCE_DIR}/inc/DPSimConfigLinkDef.h
  )

# DPSim source code
include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)
if(NOT(LIBONLY))
  add_executable(DPSim DPSim.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)
endif()
add_library(DPMCRawEvent SHARED ${PROJECT_SOURCE_DIR}/src/DPMCRawEvent.cxx ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfig.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)

# ROOT
find_program(ROOTCONF "root-config")
if(ROOTCONF)
  message("-- Detecting ROOT:    found at ${ROOTCONF}")
else()
  message(FATAL_ERROR "-- Detecting ROOT:    not found")
endif()
execute_process(COMMAND root-config --prefix OUTPUT_VARIABLE ROOT_PREFIX  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs   OUTPUT_VARIABLE ROOT_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

# install rules, not necessarily needed
install(TARGETS DPMCRawEvent DESTINATION "${ROOT_PREFIX}/lib")
install(FILES ${PROJECT_SOURCE_DIR}/inc/DPMCRawEvent.h ${PROJECT_SOURCE_DIR}/inc/DPSimConfig.h DESTINATION "${ROOT_PREFIX}/include")

# the rest packages are not needed for libonly mode
if(NOT(LIBONLY))
  # Geant4
  find_program(G4CONF "geant4-config")
  if(G4CONF)
    message("-- Detecting Geant4:  found at ${G4CONF}")
  else()
    message(FATAL_ERROR "-- Detecting Geant4:  not found")
  endif()
  find_package(Geant4 REQUIRED gdml vis_all)
  execute_process(COMMAND geant4-config --cflags OUTPUT_VARIABLE GEANT4_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND geant4-config --libs   OUTPUT_VARIABLE GEANT4_LINK   OUTPUT_STRIP_TRAILING_WHITESPACE)

  # LHAPDF
  find_program(LHAPDFCONF "lhapdf-config")
  if(LHAPDFCONF)
    message("-- Detecting LHAPDF:  found at ${LHAPDFCONF}")
  elseif(NOT(LIBONLY))
    message(FATAL_ERROR "-- Detecting LHAPDF:  not found")
  endif()
  execute_process(COMMAND lhapdf-config --cflags OUTPUT_VARIABLE LHAPDF_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND lhapdf-config --libs   OUTPUT_VARIABLE LHAPDF_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Pythia
  find_program(PYTHIACONF "pythia8-config")
  if(PYTHIACONF)
    message("-- Detecting Pythia8: found at ${PYTHIACONF}")
  elseif(NOT(LIBONLY))
    message(FATAL_ERROR "-- Detecting Pythia8: not found")
  endif()
  execute_process(COMMAND pythia8-config --cxxflags OUTPUT_VARIABLE PYTHIA_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND pythia8-config --libs --lhapdf OUTPUT_VARIABLE PYTHIA_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # MySQL
  find_program(MYSQLCONF "mysql_config")
  if(MYSQLCONF)
    message("-- Detecting MySQL:   found at ${MYSQLCONF}")
  elseif(NOT(LIBONLY))
    message(FATAL_ERROR "-- Detecting MySQL:   not found")
  endif()
  execute_process(COMMAND mysql_config --cflags OUTPUT_VARIABLE MYSQL_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND mysql_config --libs   OUTPUT_VARIABLE MYSQL_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Warning suppression
  string(REGEX REPLACE "-W[a-z-]*" "" GEANT4_CFLAGS ${GEANT4_CFLAGS})
  string(REPLACE "-pedantic" "" GEANT4_CFLAGS ${GEANT4_CFLAGS})

else()  #only find BOOST in libonly mode
  find_package(Boost)
  if(Boost_FOUND)
    message("-- Detecting BOOST:   found at ${Boost_INCLUDE_DIRS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${Boost_INCLUDE_DIRS}")
  elseif(EXISTS $ENV{BOOST_ROOT})
    message("-- Detecting BOOST:   using Boost installation at $ENV{BOOST_ROOT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I$ENV{BOOST_ROOT}/include")
  else()
    message(FATAL_ERROR "-- Detecting BOOST:   not found")
  endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GEANT4_CFLAGS} ${ROOT_CFLAGS} ${PYTHIA_CFLAGS} ${LHAPDF_CFLAGS} ${MYSQL_CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GEANT4_LINK} ${ROOT_LINK} ${PYTHIA_LINK} ${LHAPDF_LINK} ${MYSQL_LINK}")
set(CMAKE_SHARED_LINKER_FLAGS "${ROOT_LINK}" )
if(NOT(LIBONLY))
  target_link_libraries(DPSim ${ROOT_LINK} ${PYTHIA_LINK} ${LHAPDF_LINK} ${Geant4_LIBRARIES})
endif()

# extra compiles
foreach(EXTRA ${EXTRALIST})
    message("-- Compiling ${EXTRA}...")
    add_executable(${EXTRA} ${EXTRA}.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)
    target_link_libraries(${EXTRA} ${ROOT_LINK} ${PYTHIA_LINK} ${LHAPDF_LINK} ${Geant4_LIBRARIES})
endforeach()

# test stuff
#add_executable(test_debug test/test.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)
#target_link_libraries(test_debug ${Geant4_LIBRARIES})
