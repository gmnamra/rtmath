#include "../rtmath/Stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <cmath>
#include "../rtmath/ddscat/ddpar.h"
#include "../rtmath/ddscat/ddVersions.h"
#include "../rtmath/config.h"
#include "../rtmath/ddscat/rotations.h"
#include "../rtmath/error/debug.h"
#include "../rtmath/error/error.h"

namespace rtmath {
	namespace ddscat {

		namespace ddParParsers
		{

			bool ddParLine::operator==(ddParLine &rhs)
			{
				if (_id != rhs._id) return false;
				if (_endWriteWithEndl != rhs._endWriteWithEndl) return false;
				std::ostringstream oT, oR;
				write(oT);
				rhs.write(oR);
				std::string sT = oT.str(), sR = oR.str();
				if (sT != sR) return false;

				return true;
			}

			bool ddParLine::operator!=(ddParLine &rhs)
			{
				return !(operator==(rhs));
			}

			bool ddParLine::operator<(ddParLine &rhs)
			{
				if (_id != rhs._id) return (_id < rhs._id);
				std::ostringstream oT, oR;
				write(oT);
				rhs.write(oR);
				std::string sT = oT.str(), sR = oR.str();
				return (sT < sR);
			}

		}

		// ddPar overrides
		// These are template functions (for int, double, size_t) that implement the 
		// easy-use interface
		template<class T>
		T ddPar::__getSimple(ddParParsers::ParId key) const
		{
			boost::shared_ptr<const ddParParsers::ddParLineSimple<T> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			T v;
			getKey(key,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimple<T> >(linein);
			line->get(v);
			return v;
		}

		template size_t ddPar::__getSimple<size_t>(ddParParsers::ParId) const;
		template bool ddPar::__getSimple<bool>(ddParParsers::ParId) const;
		template double ddPar::__getSimple<double>(ddParParsers::ParId) const;
		template int ddPar::__getSimple<int>(ddParParsers::ParId) const;

		template<class T>
		void ddPar::__setSimple(ddParParsers::ParId key, T val)
		{
			boost::shared_ptr< ddParParsers::ddParLineSimple<T> >
				line(new ddParParsers::ddParLineSimple<T> (key));
			line->set(val);
			insertKey(key,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		template void ddPar::__setSimple<size_t>(ddParParsers::ParId, size_t);
		template void ddPar::__setSimple<bool>(ddParParsers::ParId, bool);
		template void ddPar::__setSimple<double>(ddParParsers::ParId, double);
		template void ddPar::__setSimple<int>(ddParParsers::ParId, int);

		bool ddPar::__getSimpleBool(ddParParsers::ParId key) const
		{
			boost::shared_ptr<const ddParParsers::ddParLineSimple<size_t> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			size_t v;
			getKey(key,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimple<size_t> >(linein);
			line->get(v);
			return (v) ? true : false;
		}

		void ddPar::__setSimpleBool(ddParParsers::ParId key, bool val)
		{
			boost::shared_ptr< ddParParsers::ddParLineSimple<size_t> >
				line(new ddParParsers::ddParLineSimple<size_t> (key));
			size_t vi = (val) ? 1 : 0;
			line->set(vi);
			insertKey(key,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		template<class valtype>
		valtype ddPar::__getSimplePlural(ddParParsers::ParId id, size_t index) const
		{ 
			boost::shared_ptr<const ddParParsers::ddParLineSimplePlural<valtype> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			valtype v;
			getKey(id,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimplePlural<valtype> >(linein);
			line->get(index,v);
			return v; 
		}

		template size_t ddPar::__getSimplePlural<size_t>(ddParParsers::ParId, size_t) const;
		template bool ddPar::__getSimplePlural<bool>(ddParParsers::ParId, size_t) const;
		template double ddPar::__getSimplePlural<double>(ddParParsers::ParId, size_t) const;
		template int ddPar::__getSimplePlural<int>(ddParParsers::ParId, size_t) const;

		template<class valtype>
		void ddPar::__setSimplePlural(ddParParsers::ParId id, size_t index, size_t maxSize, const valtype &v)
		{ 
			boost::shared_ptr< ddParParsers::ddParLineSimplePlural<valtype> > line;
			boost::shared_ptr< ddParParsers::ddParLine > linein;
			// does key exist? if so, load it and prepare for update
			if (exists(id))
			{
				getKey(id, linein);
				line = boost::static_pointer_cast< ddParParsers::ddParLineSimplePlural<valtype> >(linein);
			} else {
				line = boost::shared_ptr<ddParParsers::ddParLineSimplePlural<valtype> > 
					(new ddParParsers::ddParLineSimplePlural<valtype> (id));
				line->resize(maxSize);
			}
			line->set(index,v);
			insertKey(id,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		template void ddPar::__setSimplePlural<size_t>(ddParParsers::ParId, size_t, size_t, const size_t&);
		template void ddPar::__setSimplePlural<bool>(ddParParsers::ParId, size_t, size_t, const bool&);
		template void ddPar::__setSimplePlural<double>(ddParParsers::ParId, size_t, size_t, const double&);
		template void ddPar::__setSimplePlural<int>(ddParParsers::ParId, size_t, size_t, const int&);

		void ddPar::__getString(ddParParsers::ParId id, std::string &val) const
		{ 
			boost::shared_ptr<const ddParParsers::ddParLineSimple<std::string> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			std::string v;
			getKey(id,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimple<std::string> >(linein);
			line->get(v);
			val = v; 
		}

		void ddPar::__setString(ddParParsers::ParId id, const std::string &val)
		{ 
			boost::shared_ptr< ddParParsers::ddParLineSimple<std::string> >
				line(new ddParParsers::ddParLineSimple<std::string> (id));
			line->set(val);
			insertKey(id,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		bool ddPar::__getStringBool(ddParParsers::ParId id, const std::string &bfalse, const std::string &btrue) const
		{
			boost::shared_ptr<const ddParParsers::ddParLineSimple<std::string> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			std::string v;
			getKey(id,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimple<std::string> >(linein);
			line->get(v);
			if (v == btrue)
				return true;
			return false;
		}

		void ddPar::__setStringBool(ddParParsers::ParId id, bool v, const std::string &bfalse, const std::string &btrue)
		{ 
			boost::shared_ptr<ddParParsers::ddParLineSimple<std::string> >
				line(new ddParParsers::ddParLineSimple<std::string> (id));
			std::string vs = (v) ? btrue : bfalse ;
			line->set(vs);
			insertKey(id,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		ddPar::ddPar()
		{
			_init();
			// Fill in the blanks from the default file. Needed here to avoid
			// crashed associated with accessing missing keys.
			//_populateDefaults(false);
		}
		/*
		ddPar::ddPar()
		{
			_init();
			_populateDefaults(false);
		}
		*/
		ddPar::ddPar(const std::string &filename, bool popDefaults)
		{
			_init();
			readFile(filename);
			if (popDefaults)
				populateDefaults(false);
		}

		ddPar::~ddPar()
		{
		}

		bool ddPar::operator==(const ddPar &rhs) const
		{
			std::string sThis, sRhs;
			std::ostringstream oThis, oRhs;
			write(oThis);
			rhs.write(oRhs);
			sThis = oThis.str();
			sRhs = oRhs.str();
			return (sThis == sRhs);
		}

		bool ddPar::operator!=(const ddPar &rhs) const
		{
			return !(operator==(rhs));
		}

		ddPar & ddPar::operator=(const ddPar &rhs)
		{
			if (this != &rhs)
			{
				_version = rhs._version;
				std::ostringstream out;
				rhs.write(out);
				std::string data = out.str();
				std::istringstream in(data);
				read(in);
			}
			return *this;
		}

		ddPar::ddPar(const ddPar &src)
		{
			// Expensive copy constructor. Implements cloning to avoid screwups.
			_version = src._version;
			std::ostringstream out;
			src.write(out);
			std::string data = out.str();
			std::istringstream in(data);
			read(in);
		}

		ddPar* ddPar::clone() const
		{
			ddPar *lhs = new ddPar;

			lhs->_version = _version;
			
			std::ostringstream out;
			write(out);
			std::string data = out.str();
			std::istringstream in(data);

			lhs->read(in);

			return lhs;
		}

		void ddPar::readFile(const std::string &filename, bool overlay)
		{
			// Check file existence
			using namespace std;
			using namespace boost::filesystem;
			path p(filename);
			if (!boost::filesystem::exists(p)) throw debug::xMissingFile(filename.c_str());
			ifstream in(filename.c_str());
			read(in, overlay);
		}

		void ddPar::writeFile(const std::string &filename) const
		{
			populateDefaults();
			std::ofstream out(filename.c_str());
			write(out);
		}

		void ddPar::write(std::ostream &out) const
		{
			// Writing is much easier than reading!
			using namespace std;

			// Ensute that all necessary keys exist. If not, create them!!!
			//populateDefaults(); // User's responsibility

			// Write file version
			string ver;
			ver = rtmath::ddscat::ddVersions::getVerString(_version);
			out << "' ========= Parameter file for v" << ver << " =================== '" << endl;

			// Loop through and write parameters and comments
			for (auto it = _parsedData.begin(); it != _parsedData.end(); it++)
			{
				// If key is valid for this output version, write it
				if (it->second->versionValid(_version))
					it->second->write(out);

				// Check here for dielectric write. Always goes after NCOMP.
				if (it->first == ddParParsers::NCOMP)
				{
					int i = 1;
					for (auto ot = _diels.begin(); ot != _diels.end(); ++ot, ++i)
					{
						ostringstream o;
						// "...file with refractive index" + " #"
						o << " " << i;
						string plid = o.str();
						(*ot)->write(out, plid);
					}
				}
			}
			for (auto ot = _scaPlanes.begin(); ot != _scaPlanes.end(); ++ot)
			{
				// If key is valid for this output version, write it
				if (ot->second->versionValid(_version))
				{
					ostringstream o;
					// "...for plane" + " #"
					o << " " << boost::lexical_cast<std::string>(ot->first);
					string plid = o.str();
					ot->second->write(out, plid);
				}
			}
		}

		bool ddPar::exists(ddParParsers::ParId key) const
		{
			if (_parsedData.count(key)) return true;
			return false;
		}

		void ddPar::getKey(ddParParsers::ParId key, boost::shared_ptr<ddParParsers::ddParLine> &res)
		{
			res ;//= nullptr;
			if (_parsedData.count(key))
				res = _parsedData[key];
		}

		void ddPar::getKey(ddParParsers::ParId key, boost::shared_ptr<const ddParParsers::ddParLine> &res) const
		{
			res ;//= nullptr;
			if (_parsedData.count(key))
				res = _parsedData[key];
		}

		void ddPar::delKey(ddParParsers::ParId key)
		{
			if (_parsedData.count(key))
				_parsedData.erase(key);
		}

		void ddPar::insertKey(ddParParsers::ParId key, boost::shared_ptr<ddParParsers::ddParLine> ptr)
		{
			if (_parsedData.count(key))
				_parsedData.erase(key);
			_parsedData[key] = ptr;
		}
		
		void ddPar::getSIJ(std::set<size_t> &sij) const
		{
			sij.clear();

			boost::shared_ptr< const ddParParsers::ddParLineSimplePlural<size_t> > line;
			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			std::vector<size_t> val;
			// NSMELTS, INDICESIJ
			getKey(ddParParsers::INDICESIJ,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineSimplePlural<size_t> >(linein);
			line->get(val);
			for (auto it = val.begin(); it != val.end(); it++)
			{
				if (!sij.count(*it))
					sij.insert(*it);
			}
		}

		void ddPar::setSIJ(const std::set<size_t> &sij)
		{
			// Set array size NSMELTS, with max of ?
			boost::shared_ptr<ddParParsers::ddParLineSimple<std::size_t> > line_size
					( new ddParParsers::ddParLineSimple<std::size_t>(ddParParsers::NSMELTS) );
			line_size->set(sij.size());

			// Set array
			boost::shared_ptr< ddParParsers::ddParLineSimplePlural<size_t> > line
				(new ddParParsers::ddParLineSimplePlural<size_t>(ddParParsers::INDICESIJ));
			std::vector<size_t> val;
			for (auto it = sij.begin(); it != sij.end(); it++)
				val.push_back(*it);
			line->set(val);

			insertKey(ddParParsers::NSMELTS,boost::static_pointer_cast< ddParParsers::ddParLine >(line_size));
			insertKey(ddParParsers::INDICESIJ,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		void ddPar::getAeff(double &min, double &max, size_t &n, std::string &spacing) const
		{
			boost::shared_ptr< const ddParParsers::ddParLineMixed<double, std::string> > line;

			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			getKey(ddParParsers::AEFF,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineMixed<double, std::string> >(linein);
			line->setSep(3, 4);
			line->get<double>(0,min);
			line->get<double>(1,max);
			double dN;
			line->get<double>(2,dN);
			n = (size_t) dN;

			line->get<std::string>(3,spacing);
		}

		void ddPar::getWavelengths(double &min, double &max, size_t &n, std::string &spacing) const
		{
			boost::shared_ptr< const ddParParsers::ddParLineMixed<double, std::string> > line;

			boost::shared_ptr< const ddParParsers::ddParLine > linein;
			getKey(ddParParsers::WAVELENGTHS,linein);
			line = boost::static_pointer_cast< const ddParParsers::ddParLineMixed<double, std::string> >(linein);
			line->setSep(3, 4);
			line->get<double>(0,min);
			line->get<double>(1,max);
			double dN;
			line->get<double>(2,dN);
			n = (size_t) dN;

			line->get<std::string>(3,spacing);
		}
		
		void ddPar::setAeff(double min, double max, size_t n, const std::string &spacing)
		{
			boost::shared_ptr< ddParParsers::ddParLineMixed<double, std::string> > line
				(new ddParParsers::ddParLineMixed<double, std::string>(3,4, ddParParsers::AEFF));
			line->set<double>(0,min);
			line->set<double>(1,max);
			line->set<double>(2,(double) n);
			line->set<std::string>(3,spacing);
			insertKey(ddParParsers::AEFF,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		void ddPar::setWavelengths(double min, double max, size_t n, const std::string &spacing)
		{
			boost::shared_ptr< ddParParsers::ddParLineMixed<double, std::string> > line
				(new ddParParsers::ddParLineMixed<double, std::string>(3, 4, ddParParsers::WAVELENGTHS));
			line->set<double>(0,min);
			line->set<double>(1,max);
			line->set<double>(2,(double) n);
			line->set<std::string>(3,spacing);
			insertKey(ddParParsers::WAVELENGTHS,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		void ddPar::getRots(rotations &rots) const
		{
			// The rotations constructor already contains the necessary stuff. Just alias it.
			rots = rotations(*this);
		}

		void ddPar::setRots(const rotations &rots)
		{
			// use rotations code for duplication prevention
			rots.out(*this);
			/*
			boost::shared_ptr< ddParParsers::ddParLineMixed<double, size_t> > bline
				(new ddParParsers::ddParLineMixed<double, size_t>(2, ddParParsers::NBETA));
			bline->set<double>(0, rots.bMin());
			bline->set<double>(1, rots.bMax());
			bline->set<double>(2, rots.bN());


			boost::shared_ptr< ddParParsers::ddParLineMixed<double, size_t> > tline
				(new ddParParsers::ddParLineMixed<double, size_t>(2, ddParParsers::NTHETA));
			tline->set<double>(0, rots.tMin());
			tline->set<double>(1, rots.tMax());
			tline->set<double>(2, rots.tN());

			boost::shared_ptr< ddParParsers::ddParLineMixed<double, size_t> > pline
				(new ddParParsers::ddParLineMixed<double, size_t>(2, ddParParsers::NPHI));
			pline->set<double>(0, rots.pMin());
			pline->set<double>(1, rots.pMax());
			pline->set<double>(2, rots.pN());
			*/
		}

		void ddPar::getPlane(size_t key, boost::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > &res)
		{
			res ;//= nullptr;
			if (_scaPlanes.count(key))
				res = _scaPlanes[key];
		}

		void ddPar::getPlane(size_t key, boost::shared_ptr<const ddParParsers::ddParLineSimplePlural<double> > &res) const
		{
			res ;//= nullptr;
			if (_scaPlanes.count(key))
				res = _scaPlanes[key];
		}

		void ddPar::getPlane(size_t n, double &phi, double &thetan_min, double &thetan_max, double &dtheta) const
		{
			boost::shared_ptr<const ddParParsers::ddParLineSimplePlural<double> > res;
			getPlane(n, res);
			res->get(0, phi);
			res->get(1, thetan_min);
			res->get(2, thetan_max);
			res->get(3, dtheta);
		}

		void ddPar::delPlane(size_t key)
		{
			if (_scaPlanes.count(key))
				_scaPlanes.erase(key);
		}

		void ddPar::insertPlane(size_t key, boost::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > &res)
		{
			if (_scaPlanes.count(key))
				_scaPlanes.erase(key);
			_scaPlanes[key] = res;
		}

		void ddPar::setPlane(size_t n, double phi, double thetan_min, double thetan_max, double dtheta)
		{
			boost::shared_ptr<ddParParsers::ddParLineSimplePlural<double> > res
				(new ddParParsers::ddParLineSimplePlural<double>(ddParParsers::PLANE1));
			res->resize(4);
			res->set(0,phi);
			res->set(1,thetan_min);
			res->set(2,thetan_max);
			res->set(3,dtheta);

			insertPlane(n, res);
		}

		void ddPar::getDiels(std::vector<std::string>& res) const
		{
			//boost::shared_ptr<const ddParParsers::ddParLineSimple<std::string> > res;
			for (auto &diel : _diels)
			{
				std::string val;
				diel->get(val);
				res.push_back(val);
			}
		}

		void ddPar::setDiels(const std::vector<std::string>& src)
		{
			_diels.clear();
			// Set individual dielectrics
			for (auto &file : src)
			{
				boost::shared_ptr<ddParParsers::ddParLineSimple<std::string> > res
					(new ddParParsers::ddParLineSimple<std::string>(ddParParsers::IREFR));
				res->set(file);
				_diels.push_back(res);
			}

			// And update the count
			boost::shared_ptr< ddParParsers::ddParLineSimple<int> > line
				(new ddParParsers::ddParLineSimple<int>(ddParParsers::NCOMP));
			line->set((int) _diels.size());
			insertKey(ddParParsers::NCOMP,boost::static_pointer_cast< ddParParsers::ddParLine >(line));
		}

		void ddPar::populateDefaults(bool overwrite, const std::string &src) const
		{
			// Populates missing items for this version with default
			// entries. Used when converting between ddscat file versions.
			// Also used when file information is incomplete.
			using namespace rtmath::ddscat::ddParParsers;
			using namespace boost::filesystem;
			using namespace std;
			// Let's take the default entries from a default scattering file.
			// I don't want to hardcode these values, plus, by varying the path, 
			// this improves scriptability...

			boost::shared_ptr<ddPar> basep; // basep is a copy with auto-deletion
			if (src == "")
			{
				basep = boost::shared_ptr<ddPar>(new ddPar(*(defaultInstance())));
			}
			else
			{
				if (boost::filesystem::exists(path(src)))
				{
					basep = boost::shared_ptr<ddPar>(new ddPar(src));
				} else {
					throw rtmath::debug::xMissingFile(src.c_str());
				}
			}
			ddPar &base = *basep;

			for (auto it = base._parsedData.begin(); it != base._parsedData.end(); it++)
			{
				// If overwrite, then overwrite any existing key
				// If not, and key exists, skip to next one
				// If key does not exist, add it
				if (this->_parsedData.count(it->first))
				{
					if (overwrite)
					{
						this->_parsedData.erase(it->first);
					} else {
						continue;
					}
				}
				this->_parsedData[it->first] = it->second;
			}
			for (auto it = base._scaPlanes.begin(); it != base._scaPlanes.end(); it++)
			{
				// If overwrite, then overwrite any existing key
				// If not, and key exists, skip to next one
				// If key does not exist, add it
				if (this->_scaPlanes.count(it->first))
				{
					if (overwrite)
					{
						this->_scaPlanes.erase(it->first);
					} else {
						continue;
					}
				}
				this->_scaPlanes[it->first] = it->second;
			}
		}

		void ddPar::_init()
		{
			_version = rtmath::ddscat::ddVersions::getDefaultVer();
		}

		void ddPar::read(std::istream &stream, bool overlay)
		{
			// Parse until end of stream, line-by-line
			// Split string based on equal sign, and do parsing
			// based on keys
			using namespace std;
			using namespace rtmath::config;
			// _keys are mostly useless. Just used for loading.
			std::map<std::string, std::string> _keys;
			if (!overlay)
			{
				_parsedData.clear();
				_scaPlanes.clear();
				_diels.clear();
			}

			size_t line = 1;
			{
				std::string junk;
				std::getline(stream,junk); // Disregard first line in file
			}
			size_t nScaPlane = 0;
			string comment;
			while (stream.good())
			{
				string lin;
				std::getline(stream,lin);
				line++;
				// Check if this is a comment line (ends with ')
				// Need extra logic if line ends in whitespace
				{
					bool skip = false;
					for (auto it = lin.rbegin(); it != lin.rend(); ++it)
					{
						if (*it == ' ') continue;
						if (*it == '\'') 
						{
							// End line parsing
							skip = true; 
							break; 
						}
						// If we make it here, the line is valid for 
						// key-value pair parsing
						skip = false;
						break;
					} // Awkward. TODO: redo.
					if (skip) continue;
				}

				// Split lin based on '='
				// Prepare tokenizer
				typedef boost::tokenizer<boost::char_separator<char> >
					tokenizer;
				boost::char_separator<char> sep("=");
				tokenizer tcom(lin,sep);
				vector<string> vals;
				for (auto it=tcom.begin(); it != tcom.end(); ++it)
					vals.push_back(*it);
				if (vals.size() < 2) 
				{
					continue;
					//ostringstream errmsg;
					//errmsg << "This is not a valid ddscat.par file (error on file line " << line << ").";
					//throw rtmath::debug::xUnknownFileFormat(errmsg.str().c_str());
				}

				// Populate map
				//_keys[vals[1]] = vals[0];
				using namespace rtmath::ddscat::ddParParsers;
				{
					boost::shared_ptr<ddParLine> ptr = mapKeys(vals[1]);
					// Strip trailing whitespace at the end of vals[0].
					// It confuses some of the parsing functions, 
					// like ddParSimple<string>
					std::string vz = boost::algorithm::trim_right_copy(vals[0]);
					ptr->read(vz);
					// Individual dielectric files fo insta a separate structure
					if (ptr->id() == ddParParsers::IREFR)
					{
						_diels.push_back(
							boost::dynamic_pointer_cast<ddParParsers::ddParLineSimple<std::string> >(ptr));
					}
					// Everything but diels and scattering plane go here
					else if (ptr->id() < ddParParsers::PLANE1)
					{
						if (_parsedData.count(ptr->id()))
						{
							if (overlay)
							{
								_parsedData.erase(ptr->id());
							} else {
								ostringstream ostr;
								ostr << "Duplicate ddscat.par key: ";
								ostr << vals[1];
								throw rtmath::debug::xBadInput(ostr.str().c_str());
							}
						}
						_parsedData[ptr->id()] = ptr;
					} else if (ptr->id() == ddParParsers::PLANE1)
					{
						// Scattering plane info
						nScaPlane++;
						_scaPlanes[nScaPlane] = 
							boost::dynamic_pointer_cast<ddParParsers::ddParLineSimplePlural<double> >(ptr);
					} else {
						// Unknown key
						ostringstream ostr;
						ostr << "Unknown ddscat.par key: ";
						ostr << vals[1];
						throw rtmath::debug::xBadInput(ostr.str().c_str());
					}
				}

			}
		}

		ddPar*& ddPar::defaultInstance()
		{
			using namespace std;
			using namespace boost::filesystem;
			static ddPar* s_inst = nullptr;
			static bool loaded = false;
			if (!loaded)
			{
				std::shared_ptr<rtmath::config::configsegment> cRoot = config::loadRtconfRoot();
				string sBasePar, scwd;
				cRoot->getVal<string>("ddscat/DefaultFile", sBasePar);
				cRoot->getCWD(scwd);

				path pscwd(scwd), psBasePar(sBasePar);
				pscwd.remove_filename();

				if (psBasePar.is_relative()) psBasePar = pscwd / psBasePar;

				if (psBasePar.string().size() && boost::filesystem::exists(path(psBasePar)))
				{
					s_inst = new ddPar(psBasePar.string(), false);
				} else {
					// Cannot get default instance.....
					if (psBasePar.string().size())
					{
						throw rtmath::debug::xMissingFile(psBasePar.string().c_str());
					} else {
						throw rtmath::debug::xOtherError();
					}
				}

				//rtmath::debug::instances::registerInstance( "ddPar::defaultInstance", reinterpret_cast<void*>(s_inst));
				loaded = true;
			}
			return s_inst;
		}

		namespace ddParParsers
		{
			boost::shared_ptr<ddParLine> mapKeys(const std::string &key)
			{
				using namespace std;
				using namespace rtmath::ddscat::ddParParsers;
				boost::shared_ptr<ddParLine> ptr;

				if (key.find("CMTORQ") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> > 
					( new ddParLineSimple<std::string>(CMTORQ) );
				else if (key.find("CMDSOL") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CMDSOL) );
				else if (key.find("CMDFFT") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CMDFFT) );
				else if (key.find("CALPHA") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CALPHA) );
				else if (key.find("CBINFLAG") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CBINFLAG) );
				else if (key.find("dimension") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<size_t> >
					( new ddParLineSimplePlural<size_t>(DIMENSION) );
				else if (key.find("CSHAPE") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CSHAPE) );
				else if (key.find("shape parameters") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<double> > 
					( new ddParLineSimplePlural<double>(SHAPEPARAMS) );
				else if (key.find("NCOMP") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(NCOMP) );
				else if (key.find("refractive index") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(IREFR) );
				else if (key.find("NRFLD") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(NRFLD) );
				// version 7.2 NRFLD
				else if (key.find("fract. extens.") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<double> >
					( new ddParLineSimplePlural<double>(FRACT_EXTENS) );
				// version 7.2 FRACT_EXTENS
				else if (key.find("TOL") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<double> > 
					( new ddParLineSimple<double>(TOL) );
				else if (key.find("MXITER") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(MXITER) );
				// version 7.2 MXITER
				else if (key.find("GAMMA") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<double> > 
					( new ddParLineSimple<double>(GAMMA) );
				else if (key.find("ETASCA") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<double> > 
					( new ddParLineSimple<double>(ETASCA) );

				// TODO: fix wavelengths and aeff to read two doubles, a size_t and a string
				else if (key.find("wavelengths") != string::npos)
					ptr = boost::shared_ptr<ddParLineMixed<double, std::string> >
					( new ddParLineMixed<double, std::string>(3, 4, WAVELENGTHS));
				else if (key.find("NAMBIENT") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<double> > 
					( new ddParLineSimple<double>(NAMBIENT) );
				// version 7.0 NAMBIENT
				else if (key.find("aeff") != string::npos)
					ptr = boost::shared_ptr<ddParLineMixed<double, std::string> >
					( new ddParLineMixed<double, std::string>(3, 4, AEFF));

				else if (key.find("Polarization state") != string::npos)
					ptr = boost::shared_ptr<ddParTuples<double> >
					( new ddParTuples<double>(2, POLSTATE));
				else if (key.find("IORTH") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(IORTH) );
				else if (key.find("IWRKSC") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(IWRKSC) );
				else if (key.find("IWRPOL") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(IWRPOL) );
				// IWRPOL is version 7.0

				else if (key.find("NBETA") != string::npos)
					ptr = boost::shared_ptr<ddParLineMixed<double, size_t> >
					( new ddParLineMixed<double, size_t>(2, 3, NBETA));
				else if (key.find("NTHETA") != string::npos)
					ptr = boost::shared_ptr<ddParLineMixed<double, size_t> >
					( new ddParLineMixed<double, size_t>(2, 3, NTHETA));
				else if (key.find("NPHI") != string::npos)
					ptr = boost::shared_ptr<ddParLineMixed<double, size_t> >
					( new ddParLineMixed<double, size_t>(2, 3, NPHI));
				else if (key.find("IWAV") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<double> >
					( new ddParLineSimplePlural<double>(IWAV) );
				else if (key.find("NSMELTS") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(NSMELTS) );
				else if (key.find("indices ij of") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<std::size_t> >
					( new ddParLineSimplePlural<std::size_t>(INDICESIJ) );
				else if (key.find("CMDFRM") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::string> >
					( new ddParLineSimple<std::string>(CMDFRM) );
				else if (key.find("NPLANES") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimple<std::size_t> > 
					( new ddParLineSimple<std::size_t>(NPLANES) );
				else if (key.find("for plane") != string::npos)
					ptr = boost::shared_ptr<ddParLineSimplePlural<double> >
					( new ddParLineSimplePlural<double>(PLANE1));
				else
				{
					cerr << "Unmatched key: " << key << endl;
					throw rtmath::debug::xBadInput("ddscat.par");
				}

				return ptr;
			}

			bool ddParLine::versionValid(size_t ver) const
			{
				if (rtmath::ddscat::ddVersions::isVerWithin(ver,72,0))
				{
					switch (_id)
					{
					case IWRPOL:
						return false;
					default:
						return true;
					}
				}
				if (rtmath::ddscat::ddVersions::isVerWithin(ver,0,72)) // up through 7.1 (inclusive)
				{
					switch (_id)
					{
					case NRFLD:
					case FRACT_EXTENS:
					case MXITER:
					case NAMBIENT:
						return false;
					default:
						return true;
					}
				}
				return true;
			}

			bool commentString(ParId id, std::string &key)
			{
				switch (id)
				{
				case CMTORQ:
					key = "'**** Preliminaries ****'";
					break;
				case DIMENSION:
					key = "'**** Initial Memory Allocation ****'";
					break;
				case CSHAPE:
					key = "'**** Target Geometry and Composition ****'";
					break;
				case NRFLD:
					key = "'**** Additional Nearfield calculation? ****'";
					break;
				case TOL:
					key = "'**** Error Tolerance ****'";
					break;
				case MXITER:
					key = "'**** maximum number of iterations allowed ****'";
					break;
				case GAMMA:
					key = "'**** Interaction cutoff parameter for PBC calculations ****'";
					break;
				case ETASCA:
					key = "'**** Angular resolution for calculation of <cos>, etc. ****'";
					break;
				case WAVELENGTHS:
					key = "'**** Wavelengths (micron) ****'";
					break;
				case NAMBIENT:
					key = "'**** Refractive index of ambient medium'";
					break;
				case AEFF:
					key = "'**** Effective Radii (micron) **** '";
					break;
				case POLSTATE:
					key = "'**** Define Incident Polarizations ****'";
					break;
				case IWRKSC:
					key = "'**** Specify which output files to write ****'";
					break;
				case NBETA:
					key = "'**** Prescribe Target Rotations ****'";
					break;
				case IWAV:
					key = "'**** Specify first IWAV, IRAD, IORI (normally 0 0 0) ****'";
					break;
				case NSMELTS:
					key = "'**** Select Elements of S_ij Matrix to Print ****'";
					break;
				case CMDFRM:
					key = "'**** Specify Scattered Directions ****'";
					break;
				default:
					return false;
				}
				return true;
			}

			void pString(const std::string &in, std::string &out)
			{
				out = in;
				// Remove all single quotes from the string
				boost::algorithm::erase_all(out, "\'");
			}

			void idString(ParId id, std::string &key)
			{
				switch (id)
				{
				case CMTORQ:
					key = "CMTORQ*6 (NOTORQ, DOTORQ) -- either do or skip torque calculations";
					break;
				case CMDSOL:
					key = "CMDSOL*6 (PBCGS2, PBCGST, PETRKP) -- select solution method";
					break;
				case CMDFFT:
					key = "CMDFFT*6 (GPFAFT, FFTMKL)";
					break;
				case CALPHA:
					key = "CALPHA*6 (GKDLDR, LATTDR)";
					break;
				case CBINFLAG:
					key = "CBINFLAG (NOTBIN, ORIBIN, ALLBIN)";
					break;
				case DIMENSION:
					key = "dimension";
					break;
				case CSHAPE:
					key = "CSHAPE*9 shape directive";
					break;
				case SHAPEPARAMS:
					key = "shape parameters 1-3";
					break;
				case NCOMP:
					key = "NCOMP = number of dielectric materials";
					break;
				case IREFR:
					key = "file with refractive index";
					break;
				case NRFLD:
					key = "NRFLD (=0 to skip nearfield calc., =1 to calculate nearfield E)";
					break;
				case FRACT_EXTENS:
					key = "(fract. extens. of calc. vol. in -x,+x,-y,+y,-z,+z)";
					break;
				case TOL:
					key = "TOL = MAX ALLOWED (NORM OF |G>=AC|E>-ACA|X>)/(NORM OF AC|E>)";
					break;
				case MXITER:
					key = "MXITER";
					break;
				case GAMMA:
					key = "GAMMA (1e-2 is normal, 3e-3 for greater accuracy)";
					break;
				case ETASCA:
					key = "ETASCA (number of angles is proportional to [(3+x)/ETASCA]^2 )";
					break;
				case WAVELENGTHS:
					key = "wavelengths";
					break;
				case NAMBIENT:
					key = "NAMBIENT";
					break;
				case AEFF:
					key = "aeff";
					break;
				case POLSTATE:
					key = "Polarization state e01 (k along x axis)";
					break;
				case IORTH:
					key = "IORTH  (=1 to do only pol. state e01; =2 to also do orth. pol. state)";
					break;
				case IWRKSC:
					key = "IWRKSC (=0 to suppress, =1 to write \".sca\" file for each target orient.";
					break;
				case IWRPOL:
					key = "IWRPOL (=0 to suppress, =1 to write \".pol\" file for each (BETA,THETA)";
					break;
				case NBETA:
					key = "BETAMI, BETAMX, NBETA  (beta=rotation around a1)";
					break;
				case NTHETA:
					key = "THETMI, THETMX, NTHETA (theta=angle between a1 and k)";
					break;
				case NPHI:
					key = "PHIMIN, PHIMAX, NPHI (phi=rotation angle of a1 around k)";
					break;
				case IWAV:
					key = "first IWAV, first IRAD, first IORI (0 0 0 to begin fresh)";
					break;
				case NSMELTS:
					key = "NSMELTS = number of elements of S_ij to print (not more than 9)";
					break;
				case INDICESIJ:
					key = "indices ij of elements to print";
					break;
				case CMDFRM:
					key = "CMDFRM (LFRAME, TFRAME for Lab Frame or Target Frame)";
					break;
				case NPLANES:
					key = "NPLANES = number of scattering planes";
					break;
				case PLANE1:
					key = "phi, thetan_min, thetan_max, dtheta (in deg) for plane";
					break;
				case UNKNOWN:
				default:
					throw rtmath::debug::xBadInput("Unknown parid");
				}
			}
		} // end ddparparsers
	} // end namespace ddscat
} // end rtmath


