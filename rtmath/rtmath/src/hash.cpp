#include "../rtmath/Stdafx.h"
#include "../rtmath/defs.h"
#include "../rtmath/hash.h"
#include "../rtmath/Public_Domain/MurmurHash3.h"
#include "../rtmath/Serialization/serialization_macros.h"

#include <Ryan_Serialization/serialization.h>
#include <boost/serialization/export.hpp>
#include <boost/serialization/version.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <boost/lexical_cast.hpp>
#include "../rtmath/error/error.h"

namespace rtmath {

	boost::filesystem::path findHash(const boost::filesystem::path &base, const HASH_t &hash)
	{
		using namespace boost::filesystem;
		using boost::lexical_cast;
		using Ryan_Serialization::detect_compressed;
		using std::string;

		string sHashName = lexical_cast<string>(hash.lower);
		string sHashStart = sHashName.substr(0,2);
		path pHashName(sHashName);
		path pHashStart(sHashStart);

		// First, check the base directory
		path pBaseCand = base / pHashName;
		if( detect_compressed(pBaseCand.string()) ) return pBaseCand;

		// Then, check in subfolders corresponding to the first two digits of the hash
		path pSubCand = base / pHashStart / pHashName;
		if (detect_compressed(pSubCand.string() )) return pSubCand;

		return path("");
	}

	boost::filesystem::path storeHash(const boost::filesystem::path &base, const HASH_t &hash)
	{
		using namespace boost::filesystem;
		using boost::lexical_cast;
		using Ryan_Serialization::detect_compressed;
		using std::string;

		string sHashName = lexical_cast<string>(hash.lower);
		string sHashStart = sHashName.substr(0,2);
		path pHashName(sHashName);
		path pHashStart(sHashStart);

		if (!exists(base))
			throw rtmath::debug::xMissingFile(base.string().c_str());

		if (!exists(base / pHashStart))
			create_directory(base / pHashStart);

		return base/pHashStart/pHashName;
	}

	HASH_t HASH(const void *key, int len)
	{
		HASH_t res;
		MurmurHash3_x64_128(key, len, HASHSEED, &res);
		return res;
	}

	template<class Archive>
	void UINT128::serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("upper", upper);
		ar & boost::serialization::make_nvp("lower", lower);
	}

	EXPORTINTERNAL(rtmath::UINT128::serialize);
}


BOOST_CLASS_EXPORT_IMPLEMENT(rtmath::UINT128);
