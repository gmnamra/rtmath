#pragma once
#include "../defs.h"
#include <string>
#include <set>
#include <map>
#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/tuple/tuple.hpp>

// Forward declarations
namespace rtmath {
	namespace ddscat {
		class rotationsBase;
		class rotations;
	}
}


namespace rtmath {
	namespace ddscat {
		class ddPar;

		class DLEXPORT_rtmath_ddscat_base rotationsBase
		{
		public:
			rotationsBase();
			virtual ~rotationsBase();
		protected:
			double _bMin, _bMax;
			double _tMin, _tMax;
			double _pMin, _pMax;
			size_t _bN, _tN, _pN;

			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		class DLEXPORT_rtmath_ddscat_base rotations : public rotationsBase
		{
		public:
			rotations();
			rotations(const ddPar &src);
			rotations(double bMin, double bMax, size_t bN,
				double tMin, double tMax, size_t tN,
				double pMin, double pMax, size_t pN);
			static boost::shared_ptr<rotations> create(
				double bMin, double bMax, size_t bN,
				double tMin, double tMax, size_t tN,
				double pMin, double pMax, size_t pN);
			static boost::shared_ptr<rotations> create();
			static boost::shared_ptr<rotations> create(const ddPar &src);
			virtual ~rotations();
			double bMin() const { return _bMin; }
			double bMax() const { return _bMax; }
			size_t bN() const { return _bN; }
			double tMin() const { return _tMin; }
			double tMax() const { return _tMax; }
			size_t tN() const { return _tN; }
			double pMin() const { return _pMin; }
			double pMax() const { return _pMax; }
			size_t pN() const { return _pN; }
			// ddPar output function
			void out(ddPar &dest) const;
			void betas(std::string &dest) const;
			void thetas(std::string &dest) const;
			void phis(std::string &dest) const;
			void betas(std::set<double> &b) const;
			void thetas(std::set<double> &t) const;
			void phis(std::set<double> &p) const;

			bool operator==(const rotations &rhs) const;
			bool operator!=(const rotations &rhs) const;
			bool operator<(const rotations &rhs) const;

			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);

			/** \brief Convenience function to generate rotations based on 
			 * betas, thetas and phis. Handles degeneracy.
			 *
			 * \param out is the map between a given rotation and its level of degeneracy.
			 **/
			static void populateRotations(const std::set<double> &betas,
				const std::set<double> &thetas, const std::set<double> &phis,
				std::map<boost::tuple<double,double,double>, size_t> &out);
		};

		// Function to calculate Gimbal matrices and effective rotation matrix.
		// Using ddscat conventions.
		template<class T>
		void rotationMatrix(T thetad, T phid, T betad,
			Eigen::Matrix<T, 3, 3, 0, 3, 3> &Reff);

		// TODO: Should some extern template magic go here?
	}
}


BOOST_CLASS_EXPORT_KEY(rtmath::ddscat::rotationsBase);
BOOST_CLASS_EXPORT_KEY(rtmath::ddscat::rotations);
