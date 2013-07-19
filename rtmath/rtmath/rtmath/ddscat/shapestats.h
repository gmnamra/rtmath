#pragma once
#include <vector>
#include <map>
#include <set>
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Dense>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/version.hpp>
//#include <boost/program_options.hpp>

#include "shapefile.h"
#include "shapestatsRotated.h"

// Forward declarations
namespace rtmath {
	namespace ddscat {
		class shapeFileStatsBase;
		class shapeFileStats;
	}
}

namespace boost { namespace program_options { 
	class options_description; class variables_map; } }

namespace rtmath {
	namespace ddscat {
		struct rotComp
		{
			bool operator()(const boost::shared_ptr<const shapeFileStatsRotated> &lhs,
				const boost::shared_ptr<const shapeFileStatsRotated> &rhs) const;
		};

		/*
		 * Version history:
		 * 2 - qhull disabled, to remove pcl dependence. Hence, all qhull-derived terms are zero
		 * 1 - added qhull enabled / disabled flag when recording calculations
		 * 0 - 
		 */
		class shapeFileStatsBase
		{
		public:
			virtual ~shapeFileStatsBase();
			// Function that, if the shapefile referenced is not loaded, reloads the shapefile
			// Required for hulling or stats adding operations
			bool load();

			// Set rotation matrix, with each value in degrees
			//void setRot(double beta, double theta, double phi);
			void calcStatsBase();
			// calcStatsRot calculates the stats RELATIVE to the shapefile default rot.
			boost::shared_ptr<const shapeFileStatsRotated> calcStatsRot(double beta, double theta, double phi);

			// rot is the effective rotation designated by the choice of a1 and a2
			Eigen::Matrix3f rot, invrot;
			double beta, theta, phi;

			// The constant multipliers! d is unknown!
			float V_cell_const, V_dipoles_const;
			float aeff_dipoles_const;

			// These require convex hull calculations
			float max_distance;
			float a_circum_sphere;
			float V_circum_sphere;
			float SA_circum_sphere;
			
			float V_convex_hull;
			float aeff_V_convex_hull;
			float SA_convex_hull;
			float aeff_SA_convex_hull;

			// Special stats calculated only in default orientation
			float V_ellipsoid_max;
			float aeff_ellipsoid_max;
			float V_ellipsoid_rms;
			float aeff_ellipsoid_rms;

			// Extend to get volume fractions
			float f_circum_sphere;
			float f_convex_hull;
			float f_ellipsoid_max;
			float f_ellipsoid_rms;

			static const unsigned int _maxVersion;
			unsigned int _currVersion;
			bool qhull_enabled;

			// Before normalization and rotation
			Eigen::Vector3f b_min, b_max, b_mean;

			std::set<boost::shared_ptr<const shapeFileStatsRotated>, rotComp > rotations;

			// The object
			boost::shared_ptr<shapefile> _shp;
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		protected:
			shapeFileStatsBase();
			bool _valid;

			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
			
		};

		class shapeFileStats : public shapeFileStatsBase
		{
		public:
			shapeFileStats();
			shapeFileStats(const shapefile &shp);
			shapeFileStats(const boost::shared_ptr<const shapefile> &shp);
			/// Should the stats file be recalculated in the newest version?
			bool needsUpgrade() const;
		private:
			/// Recalculate all stats, using the newest version of the code
			void upgrade();
			/// Gets some initial path information from rtmath.conf
			static void initPaths();
		public:
			/// \brief Generate shapefile stats for the given shape. Optionally write them to statsfile.
			///
			/// \note Reads and writes to hash database for precomputed stats
			static boost::shared_ptr<shapeFileStats> genStats(
				const std::string &shpfile, const std::string &statsfile = "");
			static void doQhull(bool);
			static bool doQhull();

			/**
			 * \brief Adds passes shapestats options to a program
			 *
			 * \item cmdline provides options only allowed on the command line
			 * \item config provides options available on the command line and in a config file
			 * \item hidden provides options allowed anywhere, but are not displayed to the user
			 **/
			static void add_options(
				boost::program_options::options_description &cmdline,
				boost::program_options::options_description &config,
				boost::program_options::options_description &hidden);
			/// Processes static options defined in add_options
			static void process_static_options(
				boost::program_options::variables_map &vm);
			/// 

		private:
			friend class ::boost::serialization::access;
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
		};

	}
}

#define SHAPESTATS_VERSION 2
BOOST_CLASS_VERSION(rtmath::ddscat::shapeFileStatsBase, SHAPESTATS_VERSION);

