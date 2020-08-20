cmake_minimum_required(VERSION 3.12)
project(driver VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(DAQ)

daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(artdaq REQUIRED)

##############################################################################
daq_point_build_to( include )  


##############################################################################
daq_point_build_to( src )

add_library(artdaq_FragmentGenerator_duneDAQModule src/FragmentGenerator.cpp)
target_link_libraries(artdaq_FragmentGenerator_duneDAQModule appfwk::appfwk ${ARTDAQ_GENERATORS})
add_library(artdaq_HDF5Output_duneDAQModule src/HDF5Output.cpp)
target_link_libraries(artdaq_HDF5Output_duneDAQModule appfwk::appfwk ${ARTDAQ_HDF5})

##############################################################################
daq_point_build_to( test )

file(COPY test/driver.json DESTINATION test)

##############################################################################
daq_point_build_to( unittest )

daq_install(TARGETS artdaq_HDF5Output_duneDAQModule artdaq_FragmentGenerator_duneDAQModule)
