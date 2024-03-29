#pragma once
#include "../defs.h"
#include <set>
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>

#include "ddpar.h"
#include "shapes.h"
#include "rotations.h"
#include "../common_templates.h"
#include "../units.h"

namespace rtmath {
	namespace ddscat
	{
		class constrainable;
		class ddParGeneratorBase;
		class ddParIteration;
		class ddParIterator;
		class ddPar;
		class ddParGenerator;
	}
}

namespace rtmath {
	namespace ddscat {

		class DLEXPORT_rtmath_ddscat ddParGeneratorBase 
			: public constrainable
		{
		public:
			ddParGeneratorBase();
			virtual ~ddParGeneratorBase();
			// freqs and temps are now part of the shape constraints
			std::set<boost::shared_ptr<rotations> > rots;
			// The other constraints
			//shapeConstraintContainer &shapeConstraintsGlobal;
			// The shapes to generate
			std::set<boost::shared_ptr<shapeModifiable> > shapes;

			ddPar base;

			// These don't go into a ddscat.par file
			std::string name, description, outLocation;
			size_t ddscatVer;
			std::string strPreCmds;
			std::string strPostCdms;
			bool compressResults, genIndivScripts, genMassScript;
			bool shapeStats, registerDatabase, doExport;
			std::string exportLoc;

			friend class ddParIteration;
			friend class ddParIterator;

			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		class ddParGenerator;

		class DLEXPORT_rtmath_ddscat ddParIterator
		{
		public:
			// Constructor takes the pointer so we don't have to copy every conceivable property
			ddParIterator(const ddParGenerator &gen, boost::shared_ptr<shapeModifiable> shp);

			/* // These are subsumed by the specific shape
			void exportShape(const std::string &filename) const;
			void exportDiel(const std::string &filename) const;
			void exportDDPAR(ddPar &out) const;
			void exportDDPAR(const std::string &filename) const;
			*/

			// The shape is a special clone of one of the ddParGenerator shapes, with no iteration over
			// values - only one element in each shapeConstraint entry.
			// contains freq, temp, reff, and every other needed quantity
			boost::shared_ptr<shapeModifiable> shape;
			boost::shared_ptr<rotations> rots;

			friend class ddParIteration;
			friend class ddParGenerator;
			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		private:
			//const ddParGenerator &_gen;
			const ddParGenerator *_genp;
		public:
			ddParIterator(); // TODO: set serialization to not need this. Also, making it private is hard.
		};

		/// This whole class exists just to encapsulate all of the conversion and iteration into a separate step
		class DLEXPORT_rtmath_ddscat ddParIteration
		{
		public:
			ddParIteration(const ddParGenerator &gen);
			// Iterators go here
			typedef std::set< boost::shared_ptr<ddParIterator> > data_t;
			typedef data_t::const_iterator const_iterator;
			inline const_iterator begin() const { return _elements.begin(); }
			inline const_iterator end() const { return _elements.end(); }
		private:
			ddParIteration();
			data_t _elements;
			//const ddParGenerator &_gen;
			const ddParGenerator *_genp;
			void _populate();
			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		/** 
		* The generator acts to construct a set of ddscat.par files and the associated 
		* mtabs based on a template and a set of varied parameters.
		* The parameter variation generates separate files, primarily because ddscat
		* doesn't have the necessary sophistication to deal with our common usage scenarios.
		**/
		class DLEXPORT_rtmath_ddscat ddParGenerator : public ddParGeneratorBase
		{
		public:
			ddParGenerator();
			ddParGenerator(const ddPar &base);
			virtual ~ddParGenerator();
			void import(const std::string &ddparfilename);
			void generate(const std::string &basedir) const;
		public: // Static stuff here
			void setDefaultBase(const std::string &ddbasefilename);
			void setDefaultBase(const ddPar &base);
			friend class ddParIteration;
			friend class ddParIterator;
			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

	}
}

BOOST_CLASS_EXPORT_KEY(rtmath::ddscat::ddParGeneratorBase)
BOOST_CLASS_EXPORT_KEY(rtmath::ddscat::ddParGenerator)
