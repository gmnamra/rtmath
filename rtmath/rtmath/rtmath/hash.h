#pragma once
#error "Removed header. Switch to Ryan_Debug code."
#include "defs.h"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#if USE_RYAN_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#endif

#include "macros.h"
#include "registry.h"
#include <boost/filesystem.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <Ryan_Debug/logging_base.h>

//namespace boost {
	//namespace filesystem { class path; } 
	//namespace program_options { class variables_map; } 
//}

namespace rtmath {
	namespace hash {
		BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(
			m_hash,
			boost::log::sources::severity_channel_logger_mt< >,
			(boost::log::keywords::severity = rtmath::debug::error)(boost::log::keywords::channel = "hash"));
	}
	class hashStore;
	typedef std::shared_ptr<const hashStore> pHashStore;
	class hash_provider_registry{};
	/// Designed to be a singleton
	class DLEXPORT_rtmath_core Hash_registry_provider
	{
	public:
		Hash_registry_provider();
		~Hash_registry_provider();
		//typedef std::function<boost::shared_ptr<VoronoiDiagram>
		//	(const Eigen::Array3f &, const Eigen::Array3f &,
		//	const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>&)> voronoiStdGeneratorType;
		//voronoiStdGeneratorType generator;
		const char* name;
	};
	namespace registry {
		extern template class usesDLLregistry<
			::rtmath::hash_provider_registry,
			::rtmath::Hash_registry_provider >;
	}

	/// Used for hashing
	class DLEXPORT_rtmath_core UINT128 {
	public:
		UINT128() : lower(0), upper(0) {}
		UINT128(uint64_t lower, uint64_t upper) : lower(lower), upper(upper) {}
		uint64_t lower;
		uint64_t upper;
		inline bool operator<(const UINT128 &rhs) const
		{
			if (upper != rhs.upper) return upper < rhs.upper;
			if (lower != rhs.lower) return lower < rhs.lower;
			return false;
		}
		inline bool operator==(const UINT128 &rhs) const
		{
			if (upper != rhs.upper) return false;
			if (lower != rhs.lower) return false;
			return true;
		}
		inline bool operator!=(const UINT128 &rhs) const
		{
			return !operator==(rhs);
		}
		inline UINT128 operator^(const UINT128 &rhs)
		{
			UINT128 res = *this;
			res.lower = res.lower ^ rhs.lower;
			res.upper = res.upper ^ rhs.upper;
			return res;
		}

		std::string string() const
		{
			std::ostringstream o;
			o << lower;
			std::string res = o.str();
			return res;
		}
#if USE_RYAN_SERIALIZATION
	private:
		friend class ::boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version);
#endif
	};

	typedef UINT128 HASH_t;
#define HASHSEED 2387213

	/// Wrapper function that calculates the hash of an object (key) with length (len).
	HASH_t DLEXPORT_rtmath_core HASH(const void *key, int len);

	/// Wrapper function to read and hash a file (handles compression, too!)
	HASH_t DLEXPORT_rtmath_core HASHfile(const std::string& filename);


	
	class DLEXPORT_rtmath_core hashStore :
		virtual public ::rtmath::registry::usesDLLregistry<
		hash_provider_registry, Hash_registry_provider >
	{
	public:
		static bool findHashObj(const std::string &hash, const std::string &key,
			std::shared_ptr<registry::IOhandler> &sh, std::shared_ptr<registry::IO_options> &opts);
		static bool storeHash(const std::string&, const std::string &key,
			std::shared_ptr<registry::IOhandler> &sh, std::shared_ptr<registry::IO_options> &opts);
		static void addHashStore(pHashStore, size_t priority);
	public:
		virtual bool storeHashInStore(const std::string& hash, const std::string &key,
			std::shared_ptr<registry::IOhandler> &sh, std::shared_ptr<registry::IO_options> &opts
			) const;
		virtual bool findHashInStore(const std::string &hash, const std::string &key,
			std::shared_ptr<registry::IOhandler> &sh, std::shared_ptr<registry::IO_options> &opts) const;

	public:
		hashStore();
		/// Base location of the hash database
		boost::filesystem::path base;
		/// Indicates whether this store is read-only
		bool writable;
		/// The type of the hash store (dir, hdf5, ...)
		std::string type;
	public:
		virtual ~hashStore();
	
	};

}

#if USE_RYAN_SERIALIZATION
BOOST_CLASS_EXPORT_KEY(rtmath::UINT128);
#endif
std::ostream & operator<<(std::ostream &stream, const rtmath::UINT128 &ob);

