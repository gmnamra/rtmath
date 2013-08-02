#pragma once
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <map>
#include <complex>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/version.hpp>
//#include "ddOutputSingle.h"
//#include "ddScattMatrix.h"
//#include "ddweights.h"
#include "../hash.h"

namespace rtmath {

	namespace ddscat {
		class ddOutputSingle;
		class ddOutput;
		class ddOutputGenerator;
		class shapefile;
		class shapeFileStats;


		/**
		 * \brief The ensemble subclasses reweight the data to represent various ensemble schemes
		 **/
		class ddOutputGenerator : public boost::enable_shared_from_this<ddOutputGenerator>
		{
			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
			ddOutputGenerator();
		public:
			virtual ~ddOutputGenerator() {}
		protected:
			ddOutputGenerator(boost::shared_ptr<ddOutput> source);
			boost::shared_ptr<ddOutput> src, res;
		};

		/// The trivial ensemble class that duplicates DDSCAT results
		class ddOutputGeneratorSimple : public ddOutputGenerator
		{
		protected:
			ddOutputGeneratorSimple(boost::shared_ptr<ddOutput> source);
		public:
			virtual ~ddOutputGeneratorSimple() {}
			//virtual boost::shared_ptr<ddOutput> generate() const override;
		};

		/*
		// Not a child of ddOutputSingle, but a provider for ddOutput
		// This class instead CREATES a ddOutputSingle that
		// reflects the weighting distribution as-is (not updated if original
		// ddOutput gains more ddOutputSingles).
		class ddOutputEnsemble //: public ddOutputSingle
		{
		public:
			ddOutputEnsemble();
			virtual ~ddOutputEnsemble();
			// Long function definition here, even though the invocation is short...
			virtual void genEnsemble(const std::unordered_map<coords::cyclic<double>, 
				boost::shared_ptr<const ddscat::ddOutputSingle>, 
				boost::hash<coords::cyclic<double> > 
				> &_mapOutputSingleRaw, 
				ddOutputSingle &res) const = 0;
		};

		class ddOutputEnsembleGaussian : public ddOutputEnsemble
		{
		public:
			ddOutputEnsembleGaussian(double sigma, size_t coord_varying);
			virtual ~ddOutputEnsembleGaussian();
			virtual void genEnsemble(const std::unordered_map<coords::cyclic<double>, 
				boost::shared_ptr<const ddscat::ddOutputSingle>, 
				boost::hash<coords::cyclic<double> > 
				> &_mapOutputSingleRaw, 
				ddOutputSingle &res) const;
		private:
			double _sigma;
			size_t _coord_varying;
		};

		class ddOutputEnsembleIsotropic : public ddOutputEnsemble
		{
		public:
			ddOutputEnsembleIsotropic(size_t coord_varying);
			virtual ~ddOutputEnsembleIsotropic();
			virtual void genEnsemble(const std::unordered_map<coords::cyclic<double>, 
				boost::shared_ptr<const ddscat::ddOutputSingle>, 
				boost::hash<coords::cyclic<double> > 
				> &_mapOutputSingleRaw, 
				ddOutputSingle &res) const;
		private:
			size_t _coord_varying;
		};

		class ddOutputEnsembleAligned : public ddOutputEnsemble
		{
		public:
			ddOutputEnsembleAligned();
			virtual ~ddOutputEnsembleAligned();
			virtual void genEnsemble(const std::unordered_map<coords::cyclic<double>, 
				boost::shared_ptr<const ddscat::ddOutputSingle>, 
				boost::hash<coords::cyclic<double> > 
				> &_mapOutputSingleRaw, 
				ddOutputSingle &res) const;
		private:
			size_t _coord_varying;
		};
		*/
	}
}


BOOST_CLASS_EXPORT_KEY(rtmath::ddscat::ddOutputGenerator);
BOOST_SERIALIZATION_ASSUME_ABSTRACT(rtmath::ddscat::ddOutputGenerator);
