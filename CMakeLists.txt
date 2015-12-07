# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DPSim CXX C)

# Put executables into a bin
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Command line options
OPTION(DEBUG_IN "Output massive initializing debugging info" OFF)
OPTION(DEBUG_TR "Output massive tracking debugging info" OFF)
OPTION(DEBUG_IO "Output massive I/O debugging info" OFF)
OPTION(DEBUG "General debug flag for non-production compiles" ON)

IF(DEBUG_IN)
  ADD_DEFINITIONS(-DDEBUG_IN)
ENDIF(DEBUG_IN)
IF(DEBUG_TR)
  ADD_DEFINITIONS(-DDEBUG_TR)
ENDIF(DEBUG_TR)
IF(DEBUG_IO)
  ADD_DEFINITIONS(-DDEBUG_IO)
ENDIF(DEBUG_IO)
IF(NOT(DEBUG_IN OR DEBUG_TR OR DEBUG_IO OR DEBUG))
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF(NOT(DEBUG_IN OR DEBUG_TR OR DEBUG_IO OR DEBUG))

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
add_executable(DPSim DPSim.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)

# Geant4 and ROOT
find_package(Geant4 REQUIRED gdml vis_all)
execute_process(COMMAND geant4-config --cflags OUTPUT_VARIABLE GEANT4_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND geant4-config --libs   OUTPUT_VARIABLE GEANT4_LINK   OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs   OUTPUT_VARIABLE ROOT_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

# LHAPDF
execute_process(COMMAND lhapdf-config --cflags OUTPUT_VARIABLE LHAPDF_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND lhapdf-config --libs   OUTPUT_VARIABLE LHAPDF_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Pythia
execute_process(COMMAND pythia8-config --cxxflags OUTPUT_VARIABLE PYTHIA_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pythia8-config --libs     OUTPUT_VARIABLE PYTHIA_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

# MySQL
execute_process(COMMAND mysql_config --cflags   OUTPUT_VARIABLE MYSQL_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs     OUTPUT_VARIABLE MYSQL_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Warning suppression
string(REGEX REPLACE "-W[a-z-]*" "" GEANT4_CFLAGS ${GEANT4_CFLAGS})
string(REPLACE "-pedantic" "" GEANT4_CFLAGS ${GEANT4_CFLAGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GEANT4_CFLAGS} ${ROOT_CFLAGS} ${PYTHIA_CFLAGS} ${LHAPDF_CFLAGS} ${MYSQL_CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GEANT4_LINK} ${ROOT_LINK} ${PYTHIA_LINK} ${LHAPDF_LINK} ${MYSQL_LINK}")
target_link_libraries(DPSim ${Geant4_LIBRARIES})

# test stuff
#add_executable(test_debug test/test.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)
#target_link_libraries(test_debug ${Geant4_LIBRARIES})
