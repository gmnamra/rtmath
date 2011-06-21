#include "Stdafx.h"
#include <iostream>
#include "error.h"
#include <sstream>

namespace rtmath {
	namespace debug {
		void (*xError::_errHandlerFunc)(const char*) = NULL;

		xError::xError()
		{
			using namespace rtmath::debug::memcheck;
			file = __file__;
			line = __line__;
			caller = __caller__;
		}

		xError::~xError()
		{
		}

		void xError::message(std::string &message) const 
		{
			std::ostringstream out;
			if (_message.size())
			{
				out << "\n" << _message;
			} else {
				out << "\nUnknown Error" << std::endl;
			}
			if (caller)
			{
				out << "File: " << file << std::endl;
				out << "Line: " << line << std::endl;
				out << "Caller: " << caller << std::endl;
			}
			message = out.str();
		}

		void xError::Display() const
		{
			std::string out;
			this->message(out);
			if (_errHandlerFunc)
			{
				// Convert to const char array and send to function
				_errHandlerFunc(out.c_str());
			} else {
				// Send to std. err.
				std::cerr << out;
			}
		}

		void xError::setHandler(void (*func)(const char*))
		{
			_errHandlerFunc = func;
		}

		void xAssert::_setmessage()
		{
			_message = "ERROR: Assertion failed!\n Assertion is: ";
			_message.append(_m);
			_message.append("\n");
		}

		void xBadInput::_setmessage()
		{
			_message = "ERROR: The values passed to this function did not make sense.\n";
			_message.append("TODO: give a more precise error here.\n");
		}

		void xEmptyInputFile::_setmessage()
		{
			_message = "ERROR: Reading an empty input file. \nInput file: ";
			_message.append(_m);
			_message.append("\n");
		}

		void xUnimplementedFunction::_setmessage()
		{
			_message = "ERROR: Unimplemented function\n";
		}


	};
};