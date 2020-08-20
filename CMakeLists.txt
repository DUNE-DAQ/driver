cmake_minimum_required(VERSION 3.12)
project(artdaq)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

find_package(artdaq)

set(DAQ_LIBRARIES_PACKAGE ${CETLIB} ${CETLIB_EXCEPT} appfwk ${FHICLCPP} ${MF_MESSAGELOGGER} )

##############################################################################
point_build_to( include )  
set(PATH_DIRS ${CMAKE_CURRENT_BINARY_DIR}/scripts:${CMAKE_CURRENT_BINARY_DIR}/apps:${CMAKE_CURRENT_BINARY_DIR}/test)
set(LIB_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src:${CMAKE_CURRENT_BINARY_DIR}/test)


##############################################################################
point_build_to( src )

add_library(artdaq_FragmentGenerator_duneDAQModule src/FragmentGenerator.cpp)
target_link_libraries(artdaq_FragmentGenerator_duneDAQModule ${DAQ_LIBRARIES_PACKAGE} Folly::folly ${ARTDAQ_GENERATORS})
add_library(artdaq_HDF5Output_duneDAQModule src/HDF5Output.cpp)
target_link_libraries(artdaq_HDF5Output_duneDAQModule ${DAQ_LIBRARIES_PACKAGE} Folly::folly ${ARTDAQ_HDF5})

##############################################################################
point_build_to( test )

add_executable(driver test/driver.cxx)
target_link_libraries(driver artdaq_HDF5Output_duneDAQModule artdaq_FragmentGenerator_duneDAQModule  ${DAQ_LIBRARIES_UNIVERSAL_EXE} ${DAQ_LIBRARIES_PACKAGE} Folly::folly)

file(COPY test/driver.json DESTINATION test)

##############################################################################
point_build_to( unittest )


