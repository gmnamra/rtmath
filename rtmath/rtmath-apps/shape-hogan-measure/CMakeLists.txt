include (common)
include (commonRtmathApps)

# Not using Ceres solver, because it needs to be rebuilt on mizu
#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})
#find_package(Ceres REQUIRED)
#include_directories(${CERES_INCLUDE_DIRS})
find_package(lmfit REQUIRED)
include_directories(${LMFIT_INCLUDE_DIRS})

add_executable (rtmath-hogan-measure hogan-measure.cpp
	#llsq.hpp llsq.cpp
	)
addapp( rtmath-hogan-measure Shapes)
target_link_libraries(rtmath-hogan-measure
	${rtmath_ddscat-libs} rtmath_core rtmath_ddscat
	${LMFIT_LIBRARIES}
	# ${CERES_LIBRARIES}
	)


