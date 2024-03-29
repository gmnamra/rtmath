include (common)
include (commonRtmathApps)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(HDF5 COMPONENTS C CXX REQUIRED)
if(DEFINED HDF5_C_LIBRARIES)
	set (h5libs ${HDF5_C_LIBRARIES} ${HDF5_CXX_LIBRARIES})
else()
	set (h5libs hdf5 hdf5_cpp)
endif()
#FIND_PACKAGE(SZIP)
#	message("${SZIP_INCLUDE_DIRS}")
#	message("${SZIP_LIBRARIES}")
#IF (DEFINED SZIP_LIBRARIES)
#	include_directories(${SZIP_INCLUDE_DIRS})
#		SET (h5libs ${h5libs} szip)
#ENDIF()

add_subdirectory (../../related/rtmath_hdf5_cpp "${CMAKE_CURRENT_BINARY_DIR}/rtmath_hdf5_cpp")

include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../rtmath")


add_library (rtmath_plugin_hdf5 SHARED 
	plugin-hdf5.cpp
	plugin-hdf5.h
	hdf5-ddoutput.cpp
	hdf5-ddoutput-read.cpp
	hdf5-ddscat-ddpar.cpp
	hdf5-ddscat-ddpar-read.cpp
	#hdf5-phaseFunc.cpp
	#hdf5-phaseFunc-read.cpp
	hdf5-shapefile.cpp
	hdf5-shapefile-read.cpp
	hdf5-shapestats.cpp
	hdf5-shapestats-read.cpp
	hdf5-voronoi.cpp
	hdf5-voronoi-read.cpp
	)
addlib(rtmath_plugin_hdf5 SHARED)
message("${HDF5_INCLUDE_DIR_C} ${HDF5_INCLUDE_DIR_CPP} ${HDF5_INCLUDE_DIRS}")
target_include_directories(rtmath_plugin_hdf5 ${HDF5_INCLUDE_DIR_C} ${HDF5_INCLUDE_DIR_CPP})
#add_definitions(${HDF5_DEFINITIONS})
set_target_properties(rtmath_plugin_hdf5 PROPERTIES COMPILE_DEFINITIONS "${HDF5_DEFINITIONS}")


target_link_libraries(rtmath_plugin_hdf5 
	${rtmath_ddscat-libs} rtmath_core rtmath_ddscat
	${h5libs}
	rtmath_hdf5_cpp
	)
addplugin(rtmath_plugin_hdf5 io rtmath)

