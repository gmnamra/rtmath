macro(getMSVCappend)
	if(DEFINED MSVC)
		if (MSVC)
			if (MSVC10)
				set (MSVC_APPEND "vc100")
			elseif (MSVC11)
				set (MSVC_APPEND "vc110")
			elseif (MSVC12)
				set (MSVC_APPEND "vc120")
			else()
				set (MSVC_APPEND ${MSVC_VERSION})
			endif()
		endif()
	endif()
endmacro(getMSVCappend)
macro(addBaseProject)

# Enable C++11
# g++
IF(DEFINED CMAKE_COMPILER_IS_GNUCXX)
	SET (COMMON_CFLAGS ${COMMON_CFLAGS} "-std=c++11 -fPIC")
ENDIF()

IF(DEFINED MSVC)
	# MSVC parallel builds by default
	SET(COMMON_CFLAGS ${COMMON_CFLAGS} /MP)
ENDIF()

# If doing a debug build, set the appropriate compiler defines
IF("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	add_definitions(-D_DEBUG)
ENDIF()

	set(configappend "")
	if (CMAKE_CL_64)
		set(configappend "${configappend}_x64")
	else()
		set(configappend "${configappend}_x86")
	endif()
	if (MSVC)
		getMSVCappend()
		set(configappend "${configappend}_${MSVC_APPEND}")
		add_definitions(-DCONF="$(Configuration)${configappend}")
		set(CONF CONF)
	endif()
	if (MINGW)
		set(configappend "${configappend}_mingw")
		add_definitions(-DCONF="${CMAKE_BUILD_TYPE}${configappend}")
		set(CONF "\"${CMAKE_BUILD_TYPE}${configappend}\"")
	endif()
	#message("${configappend}")

endmacro(addBaseProject)


