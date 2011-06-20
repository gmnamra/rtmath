/* debug_mem.h
* Use these provided functions to debug the memory!
* Should only be used in debug builds, as they are SLOW.
*/
#pragma once


#include <stdio.h>
#include <map>
#include <stdlib.h>

namespace rtmath {
	namespace debug {
		namespace memcheck {
			extern const char* __file__;
			extern size_t __line__;
			extern const char* __caller__;
			extern bool enabled;
			bool __Track(int option, void* p, size_t size, const char* file, int line, const char* caller);
			inline bool setloc(const char* _file, int _line, const char* _caller)
			{
				using namespace rtmath::debug::memcheck;
				__file__ = _file;
				__line__ = _line;
				__caller__ = _caller;
				return false;
			}
		};
	};
};

#ifdef HEAP_CHECK

inline void* operator new(size_t size)
{
	void *p = malloc(size);
	if (p==0) // Fail
		throw std::bad_alloc();
	using namespace rtmath::debug::memcheck;
	if (!__caller__) return p;
	if (!enabled) return p;
	bool res;
	res = __Track(1,p,size,__file__,__line__,__caller__);
	//AddTrack(p, size, file, line);
	if (__caller__) fprintf(stderr,"\nHEAP: new called from: %s\n", __caller__);
	if (__file__) fprintf(stderr, " file: %s\n", __file__);
	if (__line__) fprintf(stderr, " line: %d\n", (int) __line__);
	fprintf(stderr, " size: %d\n", (int) size);
	int lengthInt = size / sizeof(int);
	int *end = &(((int*) p)[lengthInt]);
	fprintf(stderr, " memory range: %p - %p\n", p, end);
	if (res)
	{
		__file__ = 0;
		__line__ = 0;
		__caller__ = 0;
	}
	return p;
}

inline void* operator new[](size_t size)
{
	void *p = malloc(size);
	if (p==0) // Fail
		throw std::bad_alloc();
	using namespace rtmath::debug::memcheck;
	if (!__caller__) return p;
	if (!enabled) return p;
	bool res;
	res = __Track(1,p,size,__file__,__line__,__caller__);
	//AddTrack(p, size, file, line);
	if (__caller__) fprintf(stderr,"\nHEAP: new called from: %s\n", __caller__);
	if (__file__) fprintf(stderr, " file: %s\n", __file__);
	if (__line__) fprintf(stderr, " line: %d\n", (int) __line__);
	fprintf(stderr, " size: %d\n", (int) size);
	int lengthInt = size / sizeof(int);
	int *end = &(((int*) p)[lengthInt]);
	fprintf(stderr, " memory range: %p - %p\n", p, end);
	if (res)
	{
		__file__ = 0;
		__line__ = 0;
		__caller__ = 0;
	}
	return p;
}

inline void operator delete(void *p)
{
	free(p);
	using namespace rtmath::debug::memcheck;
	if (!__caller__) return;
	if (!enabled) return;
	printf("Delete called on memory %p\n", p);
	using namespace rtmath::debug::memcheck;
	bool res;
	res = __Track(2,p,0,0,0,0);
	//RemoveTrack(p);
	if (res) {
		__file__ = 0;
		__line__ = 0;
		__caller__ = 0;
	}
}

inline void operator delete[](void *p)
{
	free(p);
	using namespace rtmath::debug::memcheck;
	if (!__caller__) return;
	if (!enabled) return;
	printf("Delete called on memory %p\n", p);
	using namespace rtmath::debug::memcheck;
	bool res;
	res = __Track(2,p,0,0,0,0);
	//RemoveTrack(p);
	if (res) {
		__file__ = 0;
		__line__ = 0;
		__caller__ = 0;
	}
}


#ifdef __GNUC__
//#define DEBUG_NEW new(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#define new (rtmath::debug::memcheck::setloc(__FILE__,__LINE__,__PRETTY_FUNCTION__)) ? NULL : new
#define delete (rtmath::debug::memcheck::setloc(__FILE__,__LINE__,__PRETTY_FUNCTION__)) ? NULL : delete
//define dnew (__file=__FILE__,__line__=__LINE__,__caller__=__PRETTY_FUNCTION__) && 0 ? NULL : new
#endif
#ifdef _MSC_FULL_VER
#define new (rtmath::debug::memcheck::setloc(__FILE__,__LINE__,__FUNCSIG__)) ? NULL : new
#define delete (rtmath::debug::memcheck::setloc(__FILE__,__LINE__,__FUNCSIG__)) ? NULL : delete
#endif

	// End special heap check code
#endif


//#endif