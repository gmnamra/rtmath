include (common)
include (commonRtmathApps)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ryan-debug/)

add_library (rtmath_plugin_tsv SHARED 
	plugin-tsv.cpp
	plugin-tsv.h
	tsv-ddoutput.cpp
	tsv-ddoutput-iso.cpp
	tsv-ddoutput-iso-small.cpp
	tsv-ddoutput-ori.cpp
	tsv-ddoutput-stats.cpp
	tsv-image-basicstats.cpp
	tsv-sacr.cpp
	tsv-shapefile.cpp
	tsv-shapefile-sfc.cpp
	tsv-shapestats.cpp
	tsv-shapestats-rotdata.cpp
	tsv-shapestats-voro.cpp
	tsv-voro-sfc.cpp
	)
addlib(rtmath_plugin_tsv SHARED)

addplugin(rtmath_plugin_tsv export rtmath)

target_link_libraries(rtmath_plugin_tsv ${rtmath_images-libs} rtmath_images ${rtmath_ddscat-libs} rtmath_core rtmath_ddscat)

