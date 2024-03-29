#ifndef SDBR_DEFS
#define SDBR_DEFS

/* defs.h - Header containing typedefs and other defines for many things */

#undef SHARED_EXPORT
#undef SHARED_IMPORT
#undef SHARED_INTERNAL
#undef SHARED_PRIVATE

/// \todo CMakeLists needs to set the appropriate flags EXPORTING_SDBR on all libraries and 
/// SHARED_SDBR_* for the libraries that are shared.

/// \def EXPORTING_SDBR is set in each library. Absence of flag indicates possible import of shared code.
/// \def SHARED_SDBR is set when building using shared libraries. 

/// \def DLEXPORT_SDBR takes three values. dllexport and dllimport are Windows-only, used with shared libraries.
/// Otherwise, this tag is removed by the preprocessor.

/// \def DEPRECATED acts to deprecate a function or a class
/// \def UNIMPLEMENTED acts as a tag indicating that a function is not ready for use
#if defined _MSC_FULL_VER
#pragma warning( disable : 4251 ) // DLL C-interface MSVC warning
#define DEPRECATED __declspec(deprecated)
#define WARN_UNIMPLEMENTED
#define ERR_UNIMPLEMENTED __declspec(deprecated("Function is unimplemented / is commented out"))
#elif defined __clang__
#define DEPRECATED __attribute__ ((deprecated))
#define WARN_UNIMPLEMENTED __attribute__ ((unavailable("Unimplemented function is used")))
#define ERR_UNIMPLEMENTED __attribute__ ((unavailable("Unimplemented function is used")))
#elif defined __GNUC__
#define DEPRECATED __attribute__ ((deprecated))
#define WARN_UNIMPLEMENTED __attribute__ ((warning("Unimplemented function is used")))
#define ERR_UNIMPLEMENTED __attribute__ ((error("Unimplemented function is used")))
#else
#define DEPRECATED
#define WARN_UNIMPLEMENTED
#define ERR_UNIMPLEMENTED
#endif


#if defined _MSC_FULL_VER
#define COMPILER_EXPORTS_VERSION_A
#elif defined __INTEL_COMPILER
#define COMPILER_EXPORTS_VERSION_B
#elif defined __GNUC__
#define COMPILER_EXPORTS_VERSION_B
#elif defined __MINGW32__
#define COMPILER_EXPORTS_VERSION_B
#elif defined __clang__
#define COMPILER_EXPORTS_VERSION_B
#else
#define COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

// Defaults for static libraries
#define SHARED_EXPORT
#define SHARED_IMPORT
#define SHARED_INTERNAL
#define SHARED_PRIVATE

#if defined COMPILER_EXPORTS_VERSION_A
	#undef SHARED_EXPORT
	#undef SHARED_IMPORT
	#define SHARED_EXPORT __declspec(dllexport)
	#define SHARED_IMPORT __declspec(dllimport)
#elif defined COMPILER_EXPORTS_VERSION_B
	#undef SHARED_EXPORT
	#undef SHARED_IMPORT
	#undef SHARED_INTERNAL
	#undef SHARED_PRIVATE
	#define SHARED_EXPORT __attribute__ ((visibility("default")))
	#define SHARED_IMPORT __attribute__ ((visibility("default")))
	#define SHARED_INTERNAL __attribute__ ((visibility("hidden")))
	#define SHARED_PRIVATE __attribute__ ((visibility("internal")))
#else
	#pragma message("defs.h warning: compiler is unrecognized")
#endif

// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT should be used.


#if SHARED_SDBR
	#if EXPORTING_SDBR
		#define DLEXPORT_SDBR SHARED_EXPORT
	#else
		#define DLEXPORT_SDBR SHARED_IMPORT
	#endif
#else
	#define DLEXPORT_SDBR
#endif

// End the header block
#endif

