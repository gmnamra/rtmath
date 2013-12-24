/**
* \brief Contains registry functions for extending functionality through DLLs.
*
* Contains both general and OS-specific functions.
**/

#include "Stdafx-core.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <Ryan_Debug/debug.h>
#include "../rtmath/config.h"
#include "../rtmath/splitSet.h"
#include "../rtmath/error/debug.h"
#include "../rtmath/error/debug_mem.h"
#include "../rtmath/error/error.h"
#include "../rtmath/registry.h"

#ifdef _WIN32
#include "windows.h"
#endif
#ifdef __unix__
#include <dlfcn.h>
#endif

#ifdef _WIN32
typedef HINSTANCE dlHandleType;
#endif
#ifdef __APPLE__
typedef void* dlHandleType;
#endif
#ifdef __unix__
typedef void* dlHandleType;
#endif

/// DLL information tables
namespace {
	class DLLhandle;
	/// Lists the paths of all loaded dlls
	std::set<std::string> DLLpathsLoaded;
	/// DLL information structure
	std::vector<rtmath::registry::DLLpreamble> preambles;
	/// Container for the handles of all loaded dlls
	std::vector<boost::shared_ptr<DLLhandle> > handles;

	/// Recursive and single-level DLL loading paths
	std::set<boost::filesystem::path> searchPathsRecursive, searchPathsOne;
	//bool autoLoadDLLs = true;

	/// Checks if a file is a dll file
	bool isDynamic(const boost::filesystem::path &f)
	{
		using namespace boost::filesystem;
		boost::filesystem::path p = f;
		while (p.has_extension())
		{
			path ext = p.extension();
			if (ext.string() == ".so" || ext.string() == ".dll"
				|| ext.string() == ".dylib") return true;
			p.replace_extension();
		}
		return false;
	};

	/// Checks if a dll file matches the build settings of the rtmath library, by file path
	bool correctVersionByName(const std::string &slower)
	{
		using namespace std;
		// The DLL case probably sould never occur.
#ifdef _DLL
		if (slower.find("static") != string::npos) return false;
#else
		if (slower.find("dynamic") != string::npos) return false;
#endif
		// Debug vs release dlls
#ifdef _DEBUG
		if (slower.find("release") != string::npos) return false;
#else
		if (slower.find("debug") != string::npos) return false;
#endif
		/// Check for x86 vs x64
#if __amd64 || _M_X64
		if (slower.find("x86") != string::npos) return false;
#else
		if (slower.find("x64") != string::npos) return false;
#endif
		/// \todo Check against windows system crt vs version-specific one
		/// \todo Figure out how to get crt lib name from loaded dll
		return true;
	}

	/**
	 * \brief Determines the search paths for dlls in the rtmath.conf file and in environment variables
	 *
	**/
	void constructSearchPaths(bool use_rtmath_conf = true, bool use_environment = true)
	{
		using std::vector;
		using std::set;
		using std::string;

		// Checking rtmath.conf
		if (use_rtmath_conf)
		{
			auto rtconf = rtmath::config::loadRtconfRoot();
			string srecursivePaths, sonePaths;
			rtconf->getVal("General/Plugins/Recursive", srecursivePaths);
			rtconf->getVal("General/Plugins/OneLevel", sonePaths);
			// Split loading paths based on semicolons and commas. Do not trim spaces.
			set<string> CrecursivePaths, ConePaths;
			rtmath::config::splitSet(srecursivePaths, CrecursivePaths);
			rtmath::config::splitSet(sonePaths, ConePaths);
			for (auto &p : CrecursivePaths)
				searchPathsRecursive.emplace(boost::filesystem::path(p));
			for (auto &p : ConePaths)
				searchPathsOne.emplace(boost::filesystem::path(p));
		}

		// Checking environment variables
		if (use_environment)
		{
			using namespace Ryan_Debug;
			boost::shared_ptr<const processInfo> info(getInfo(getPID()), freeProcessInfo);

			size_t sEnv = 0;
			const char* cenv = getEnviron(info.get(), sEnv);
			std::string env(cenv, sEnv);

			//Ryan_Debug::processInfo info = Ryan_Debug::getInfo(Ryan_Debug::getPID());
			std::map<std::string, std::string> mEnv;
			rtmath::config::splitNullMap(env, mEnv);
			//std::vector<std::string> mCands;
			auto searchFunc = [](const std::pair<std::string, std::string> &pred, const std::string &mKey)
			{
				std::string key = pred.first;
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				if (key == mKey) return true;
				return false;
			};
			auto it = std::find_if(mEnv.cbegin(), mEnv.cend(),
				std::bind(searchFunc,std::placeholders::_2,"rtmath_dlls_recursive"));
			if (it != mEnv.cend())
			{
				typedef boost::tokenizer<boost::char_separator<char> >
					tokenizer;
				boost::char_separator<char> sep(";");

				std::string ssubst;
				tokenizer tcom(it->second, sep);
				for (auto ot = tcom.begin(); ot != tcom.end(); ot++)
				{
					using namespace boost::filesystem;
					path testEnv(it->second);
					if (exists(testEnv))
					{
						filename = it->second;
						return;
					}
				}
			}
		}
	}

	/**
	 * \brief Locates all DLLs in the search path and loads them.
	 *
	 * The search path may be specified / manipulated from several locations:
	 * - precompiled hints (from cmake)
	 * - rtmath.conf
	 * - the command line
	 *
	 * This takes all of the starting points in the initial search paths and recurses through the 
	 * directories, selecting dll and so files for matching. The load routine is aware of the 
	 * library build mode (Debug, Release, MinSizeRel, RelWithDebInfo). If one of these terms appears 
	 * in the path (folder tree + filename) of a library, then the dll is only loaded if its 
	 * build mode matches the library's mode. When the dll is actually loaded (other function), 
	 * the build mode is reported by the DLL more directly and checked again.
	 *
	 * \see searchPaths
	 * \see loadSearchPaths
	 * \see rtmath::registry::process_static_options
	**/
	void searchDLLs(std::vector<std::string> &dlls)
	{
		using namespace boost::filesystem;
		using namespace std;
		for (const auto &sbase : searchPaths)
		{
			path base(sbase);
			base = rtmath::debug::expandSymlink(base);
			if (is_regular_file(base)) dlls.push_back(base.string());
			else if (is_directory(base))
			{
				vector<path> recur;
				copy(recursive_directory_iterator(base,symlink_option::recurse),
					recursive_directory_iterator(), back_inserter(recur));
				for (const auto &p : recur)
				{
					if (!is_regular_file(p)) continue;
					// Convenient function to recursively check extensions to see if one is so or dll.
					// Used because of versioning.
					
					if (isDynamic(p))
					{
						// Check to see if build type is present in the path
						std::string slower = p.string();
						// Convert to lower case and do matching from there
						std::transform(slower.begin(), slower.end(), slower.begin(), ::tolower);

						if (correctVersionByName(p.string()))
							dlls.push_back(p.string());
					}
				}
			}
		}
	}

	/** \brief Class that loads the DLL in an os-independent manner.
	*
	* Construction is allocation.
	* This class enables the safe closiong of dlHandle if registration 
	* fails to initialize properly. Previously, a dangling handle was
	* produced.
	**/
	class DLLhandle
	{
	public:
		DLLhandle(const std::string &filename) : dlHandle(nullptr)
		{
			open(filename);
		}
		DLLhandle() : dlHandle(nullptr)
		{
		}
		void open(const std::string &filename)
		{
			if (DLLpathsLoaded.count(filename)) throw rtmath::debug::xDuplicateHook(fname.c_str());
			if (dlHandle) throw rtmath::debug::xHandleInUse(fname.c_str());
			fname = filename;
#ifdef __unix__ // Indicates that DLSYM is provided (unix, linux, mac, etc. (sometimes even windows))
			//Check that file exists here
			this->dlHandle = dlopen(filename.c_str(), RTLD_LAZY);
			if (dlerror()) throw rtmath::debug::xMissingFile(filename.c_str());
#endif
#ifdef _WIN32
			this->dlHandle = LoadLibrary(filename.c_str());
			if (this->dlHandle == NULL) throw rtmath::debug::xMissingFile(filename.c_str());
#endif
			DLLpathsLoaded.insert(filename);
		}
		void close()
		{
			if (!dlHandle) return;
			DLLpathsLoaded.erase(fname);
#ifdef __unix__
			dlclose(this->dlHandle);
#endif
#ifdef _WIN32
			FreeLibrary(this->dlHandle);
#endif
		}
		~DLLhandle()
		{
			if (dlHandle) close();
		}
		void* getSym(const char* symbol)
		{
			if (dlHandle == NULL) throw rtmath::debug::xHandleNotOpen(fname.c_str());
			void* sym;
#ifdef __unix__
			sym = dlsym(dlHandle, symbol);
#endif
#ifdef _WIN32
			sym = GetProcAddress(dlHandle, symbol);
#endif
			if (!sym) throw rtmath::debug::xSymbolNotFound(symbol, fname.c_str());
			return (void*)sym;
		}
	private:
		std::string fname;
		dlHandleType dlHandle;
	};
}

namespace rtmath
{
	namespace registry
	{

		void add_options(
			boost::program_options::options_description &cmdline,
			boost::program_options::options_description &config,
			boost::program_options::options_description &hidden)
		{
			namespace po = boost::program_options;
			using std::string;

			cmdline.add_options()
				;

			config.add_options()
				;

			hidden.add_options()
				("dll-load", po::value<std::vector<std::string> >(),
				"Specify dlls to load. If passed a directory, it loads all dlls present (one-level). "
				"Will use search paths in name resolution.")
			//	("dll-no-default-locations", "Prevent non-command line dll locations from being read")
				("print-dll-loaded", "Prints the table of loaded DLLs.")
				("print-dll-search-paths", "Prints the search paths used when loading dlls.")
				("rtmath-conf", po::value<std::vector<std::string> >(),
				"Override location to rtmath.conf file.")
				;
		}

		void process_static_options(
			boost::program_options::variables_map &vm)
		{
			namespace po = boost::program_options;
			using std::string;

			if (vm.count("rtmath-conf"))
				rtmath::config::loadRtconfRoot(vm["rtmath-conf"].as<string>());

			//if (vm.count("dll-no-default-locations"))
			//	autoLoadDLLs = false;

			if (vm.count("dll-load"))
			{
				using namespace boost::filesystem;
				std::vector<std::string> sPaths = vm["dll-load"].as<std::vector<std::string> >();
				for (const auto s : sPaths)
				{
					path op(s);

					// Attempt to find the path if not absolute
					if (!op.is_absolute())
					{
						auto search = [&](const path& op, path& res) -> bool
						{
							for (const path &sp : searchPaths)
							{
								if (exists(sp / op))
								{
									res = sp / op;
									return true;
								}
							}
							return false;
						};
						if (!search(op, op)) throw debug::xMissingFile(op.string().c_str());
					}

					// Expand symlinks
					path p = debug::expandSymlink(op);
					if (is_directory(p))
					{
						// Open all libs under one level of this directory, following symlinks
						std::vector<path> cands;
						copy(directory_iterator(p),
							directory_iterator(), back_inserter(cands));
						for (auto f = cands.begin(); f != cands.end(); ++f)
						{
							path pf = *f;
							pf = debug::expandSymlink(pf);
							path pext = pf.extension();
							if (pext.string() == ".dll" || pf.string().find(".so") != string::npos)
							{
								loadDLL(pf.string());
							}
						}
					} else {
						if (exists(p))
							loadDLL(p.string());
						else {
							// Search for matching files in the same directory with the dll or so extension.
							// TODO
							throw debug::xMissingFile(p.string().c_str());
						}
					}
				}
			}

			if (vm.count("print-dll-search-paths"))
				printDLLsearchPaths();

			if (vm.count("print-dll-loaded"))
				printDLLs();
		}

		void loadDLL(const std::string &filename)
		{
			auto doLoad = [](const std::string &f)
			{
				boost::shared_ptr<DLLhandle> h(new DLLhandle(f));
				handles.push_back(h);
			};
			// Search for the dll
			using namespace boost::filesystem;
			path p(filename);
			if (p.is_absolute())
			{
				if (exists(p)) doLoad(p.string());
				else throw debug::xMissingFile(p.string().c_str());
			}
			else {
				// Load in the default search paths
				if (!searchPaths.size())
					loadSearchPaths();
				// Proceed through the search paths to attempt to find the file
				for (const auto &s : searchPaths)
				{
					path pr = boost::filesystem::absolute(p, s);
					if (exists(pr))
					{
						doLoad(pr.string());
						return;
					}
				}
				// By this point, the path cannot be found
				throw debug::xMissingFile(p.string().c_str());
			}
			
		}

		void printDLLs(std::ostream &out)
		{
			out << "rtmath DLL registry table:\n--------------------\n";
			for (const auto p : preambles)
			{
				out << "Name:\t" << p.name << "\n"
					<< "Description:\t" << p.description << "\n"
					<< "UUID:\t" << p.uuid << "\n";
				if (p.path)
					out << "Path:\t" << p.path << std::endl;
			}
			out << std::endl;
			out << "DLL paths loaded:\n----------------\n";
			for (const auto p : DLLpathsLoaded)
				out << p << "\n";
			out << std::endl;

			/*out << "\nHook Table:\n-----------------\nClass\tTopic\tPointer";
			for (const auto hm : hookRegistry)
			{
				out << hm.first << "\n";
				for (const auto h : hm.second)
				{
					out << "\t" << h.first << "\t" << h.second << "\n";
				}
			}
			*/

			out << std::endl;
		}

		void printDLLsearchPaths(std::ostream &out)
		{
			out << "rtmath DLL registry search paths:\n--------------------\n";
			for (const auto p : searchPaths)
			{
				out << p.string() << "\n";
			}
			out << std::endl;
		}

		/*
		void queryClass(const char* classname, classHookMapType& result)
		{
			result.clear();
			std::string shc(classname);
			if (hookRegistry.count(shc))
			{
				result = hookRegistry.at(shc);
			}
		}
		*/
	}
}

extern "C"
{
	bool rtmath_registry_register_dll(const rtmath::registry::DLLpreamble &p)
	{
		rtmath::registry::DLLpreamble b = p;
		// \todo Add in dll path to preamble!
		preambles.push_back(b);
		return true;
	}

	//bool rtmath_registry_register_hook(const char* hookedClass, const char* topic) { return false; }
	/*
	bool rtmath_registry_register_hook(const char* hookedClass, const char* topic, void* func)
	{
		// \todo Replace void* with an appropriate function definition

		std::string shc(hookedClass);
		if (!hookRegistry.count(shc))
		{
			rtmath::registry::classHookMapType newHookMap;
			hookRegistry.insert(std::pair < std::string, rtmath::registry::classHookMapType >
				(shc, newHookMap));
		}

		rtmath::registry::classHookMapType &map = hookRegistry.at(shc);

		map.insert(std::pair<std::string, void*>(topic, func));
	}
	*/
	
}

