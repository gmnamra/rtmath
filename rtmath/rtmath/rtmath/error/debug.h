/* debug.h - The debugging system for the library */
//#pragma once



#pragma once

#include <iostream>
#include <string>
#include "error.h"


namespace rtmath
{
	namespace debug
	{
		// Generate the startup message giving 
		// library information
		void debug_preamble(std::ostream &out = std::cerr);
		int rev(void);
		void timestamp(bool show = false, std::ostream &out = std::cerr);
		struct keymap
		{
		public:
			std::string file;
			unsigned int line;
			std::string function;
			keymap() {};
			keymap(const char* file, int line, const char* func)
			{
				this->file = std::string(file);
				this->line = line;
				this->function = std::string(func);
			}
		};
		void DEBUG_TIMESTAMP(void);
		unsigned int getuniqueobj(const char* file, int line, const char* func);
		void listuniqueobj(std::ostream &out = std::cerr);

		class uidtracker
		{
		public:
			uidtracker();
			virtual ~uidtracker();
			inline unsigned int uid() { return _uid; }
		private:
			unsigned int _uid;
			static unsigned int _uidcount;
		};

		// rtmath-apps application entry point. Not fully necessary, but it sets
		// several really convenient settings.
		void appEntry(int argc, char** argv); // in os_functions.cpp
		bool waitOnExit(); // in os_functions.cpp
		void appExit(); // Convenient default atexit function

		// App warning functions
		//void warn(const char* msg);
	}; // end namespace debug

	// see assert.h for how this is made

#ifdef __GNUC__
#define GETOBJKEY() debug::getuniqueobj(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
#ifdef _MSC_FULL_VER
#define GETOBJKEY() debug::getuniqueobj(__FILE__, __LINE__, __FUNCSIG__)
#endif

#ifdef DEBUG

#ifdef __GNUC__
#define MARK() {std::cerr << "MARK " << debug::getuniqueobj(-1, "DEBUG", 0, "MARK") << ": " << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << "\n";};
#define MARKUID() {std::cerr << "MARK: " << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": " << this->_uid << "\n";};
#define MARKFUNC() {std::cerr << "MARK: " << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << "\n";};
#endif
#ifdef _MSC_FULL_VER
#define MARK() {std::cerr << "MARK " << debug::getuniqueobj(-1, "DEBUG", 0, "MARK") << ": " << __FILE__ << ":" << __LINE__ << ": " << __FUNCSIG__ << "\n";};
#define MARKUID() {std::cerr << "MARK: " << __FILE__ << ":" << __LINE__ << ": " << __FUNCSIG__ << ": " << this->_uid << "\n";};
#define MARKFUNC() {std::cerr << "MARK: " << __FILE__ << ":" << __LINE__ << ": " << __FUNCSIG__ << "\n";};
#endif

#else
#define MARK() { getuniqueobj(-1, "DEBUG", 0, "MARK"); };
#define MARKUID()
#define MARKFUNC()
#endif

// Throw unimplemented function

#ifdef __GNUC__
#define UNIMPLEMENTED() { throw debug::xUnimplementedFunction(__PRETTY_FUNCTION__); };
#endif
#ifdef _MSC_FULL_VER
#define UNIMPLEMENTED() { throw debug::xUnimplementedFunction(__FUNCSIG__); };
#endif

// Die on error
#ifdef __GNUC__
#define DIE() { throw debug::diemsg(__FILE__,__LINE__,__PRETTY_FUNCTION__); };
#endif
#ifdef _MSC_FULL_VER
#define DIE() { throw debug::diemsg(__FILE__,__LINE__,__FUNCSIG__); };
#endif

}; // end namespace rtmath

