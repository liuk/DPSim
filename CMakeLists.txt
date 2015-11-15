# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DPSim CXX C)

# Put executables into a bin
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# ROOT dict generation
add_custom_command (
  OUTPUT ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx
  #DEPENDES ${PROJECT_SOURCE_DIR}/inc/DPMCRawEvent.h ${PROJECT_SOURCE_DIR}/inc/DPMCRawEventLinkDef.h
  COMMAND rootcint
  ARGS -f ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx -c ${PROJECT_SOURCE_DIR}/inc/DPMCRawEvent.h ${PROJECT_SOURCE_DIR}/inc/DPMCRawEventLinkDef.h
  )

add_custom_command (
  OUTPUT ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx
  #DEPENDES ${PROJECT_SOURCE_DIR}/inc/DPSimConfig.h ${PROJECT_SOURCE_DIR}/inc/DPSimConfigLinkDef.h
  COMMAND rootcint
  ARGS -f ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx -c ${PROJECT_SOURCE_DIR}/inc/DPSimConfig.h ${PROJECT_SOURCE_DIR}/inc/DPSimConfigLinkDef.h
  )

# GMC source code
include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)
add_executable(DPSim DPSim.cxx ${sources} ${headers} ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.cxx ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.cxx)

# Geant4 and ROOT
find_package(Geant4 REQUIRED gdml vis_all)
execute_process(COMMAND geant4-config --prefix OUTPUT_VARIABLE GEANT4_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND geant4-config --libs OUTPUT_VARIABLE GEANT4_LINK OUTPUT_STRIP_TRAILING_WHITESPACE)
include_directories(${GEANT4_PREFIX}/include/Geant4)
target_link_libraries(DPSim ${GEANT4_LINK})
target_link_libraries(DPSim ${Geant4_LIBRARIES})

execute_process(COMMAND root-config --prefix OUTPUT_VARIABLE ROOT_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs   OUTPUT_VARIABLE ROOT_LINK   OUTPUT_STRIP_TRAILING_WHITESPACE)
include_directories(${ROOT_PREFIX}/include)
target_link_libraries(DPSim ${ROOT_LINK})

# BOOST         For LHAPDF
include_directories($ENV{BOOST_ROOT})

# LHAPDF
execute_process(COMMAND lhapdf-config --libs OUTPUT_VARIABLE LHAPDF_LINK OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND lhapdf-config --incdir OUTPUT_VARIABLE LHAPDF_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND lhapdf-config --libdir OUTPUT_VARIABLE LHAPDF_RLINK OUTPUT_STRIP_TRAILING_WHITESPACE)
include_directories(${LHAPDF_INCLUDE})
target_link_libraries(DPSim ${LHAPDF_LINK})

# Pythia
execute_process(COMMAND pythia8-config --includedir OUTPUT_VARIABLE PYTHIA_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pythia8-config --libs OUTPUT_VARIABLE PYTHIA_LINK OUTPUT_STRIP_TRAILING_WHITESPACE)

# include_directories command does not like -I prefix
string(REPLACE "-I" "" PYTHIA_INCLUDE ${PYTHIA_INCLUDE})
include_directories(${PYTHIA_INCLUDE})
target_link_libraries(DPSim ${PYTHIA_LINK})

# MySQL
execute_process(COMMAND mysql_config --include OUTPUT_VARIABLE MYSQL_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs OUTPUT_VARIABLE MYSQL_LINK OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "-I" "" MYSQL_INCLUDE ${MYSQL_INCLUDE})
include_directories(${MYSQL_INCLUDE})
target_link_libraries(DPSim ${MYSQL_LINK})

# additional clean
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_SOURCE_DIR}/src/DPMCRawEventDict.h)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_SOURCE_DIR}/src/DPSimConfigDict.h)
