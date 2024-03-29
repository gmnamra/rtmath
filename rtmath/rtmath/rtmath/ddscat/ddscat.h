#pragma once
#error "Unused header ddscat.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <complex>
//#include "../matrixop.h"
//#include "../phaseFunc.h"
//#include "cdf-ddscat.h"
//#include "ddScattMatrix.h"

// Needs extensive use of filesystem
// (for reading whole directories, manipulating paths, ...)
//#include <boost/filesystem.hpp>

/* ddscat.h - Set of classes used for creating ddscat runs, loading ddscat output into model as a daPf,
 * converting ddscat output into the much older format used for comparison with Evas' rt4, and some
 * netcdf stuff.
 * Can write both ddscat.par files and provide a script that iterates through runs and executes them.
 */


namespace rtmath {

	namespace ddscat {
		class ddScattMatrix;
		class ddOutputSingle;
		/*
		class ddScattMatrix{
			// A complex matrix that holds the scattering amplitudes (2x2)
			// It may be converted into other forms
		public:
			// Standard constructor. theta and phi in degrees.
			// wavelength in microns
			ddScattMatrix(double theta, double phi, double wavelength);
			ddScattMatrix();
			std::complex<double> vals[2][2]; // Just for convenience
			mutable std::complex<double> S[4]; // also for convenience
			inline double theta() const {return _theta;}
			inline double phi() const {return _phi;}
			ddScattMatrix& operator=(const ddScattMatrix &rhs);
			ddScattMatrix operator+(const ddScattMatrix &rhs);
			ddScattMatrix& operator+=(const ddScattMatrix &rhs);
			bool operator==(const ddScattMatrix &rhs) const;
			bool operator!=(const ddScattMatrix &rhs) const;
			void print() const;
			void writeCSV(const std::string &filename) const;
			void writeCSV(std::ofstream &out) const;
			void update();
			bool lock;
			double Pnn[4][4];
			double Knn[4][4];
		public: // Conversions start here
			void genS();
			void mueller(double Pnn[4][4]) const;
			void mueller(matrixop &res) const;
			inline matrixop mueller() const { matrixop res(2,4,4); mueller(res); return res; }
			void extinction(double Knn[4][4]) const;
			void extinction(matrixop &res) const;
			inline matrixop extinction() const { matrixop res(2,4,4); extinction(res); return res; }
		private:
			double _theta, _phi, _wavelength;
		};
		*/

		/*
		class ddOutputSingle : public rtmath::phaseFunc
		{
			// Class contains the output of a single ddscat file
			// This usually contains the results for a given Beta, Theta, Phi
			// And has the scattering amplitude matrix elements
			// It is for a given wavenumber, size parameter
			// effective radius and shape (also determining the number of
			// dipoles and a few other quantities).
			// Results are presented with varied theta and phi (note lower case)
		public:
			ddOutputSingle(double beta, double theta, double phi, double wavelength); // rotation angles
			ddOutputSingle(const std::string &filename) { _init(); loadFile(filename); }
			ddOutputSingle() { _init(); }
			void loadFile(const std::string &filename);
			void getF(const ddCoords &coords, ddScattMatrix &f) const;
			void setF(const ddCoords &coords, const ddScattMatrix &f);
			ddOutputSingle& operator=(const ddOutputSingle &rhs);
			void print() const;
			// evaluate phase function at a given scattering angle:
			std::string filename;
			virtual std::shared_ptr<matrixop> eval(double alpha) const;
			void writeCSV(const std::string &filename) const;
			void size(std::set<double> &thetas, std::set<double> &phis) const;

			// Perform interpolation based on the data already generated to 
			// roughly approximate the phase matrices for a given set of coordinates.
			// This interpolation is used to determine the phase matrices at the 
			// quadrature points.
			void interpolate(const ddCoords &coords, ddScattMatrix &res) const;

			// Need function here to generate emission vectors
			void emissionVector(double mu, matrixop &res) const;
		public:
			void _init();
			double _Beta, _Theta, _Phi;
			// wavelength in um, freq in GHz
			double _wavelength, _freq, _numDipoles, _reff;
			double _shape[3]; // for ellipsoids
			mutable std::map<ddCoords, ddScattMatrix, ddCoordsComp> _fs;
		};
		*/
		/*
		class ddOutputEnsemble : public ddOutputSingle
		{
			// Class is a pure virtual class designed to do ensemble weighting
			// of a set of ddOutputSingle
		public:
			ddOutputEnsemble() {}
			virtual ~ddOutputEnsemble() {}
			virtual void generate() = 0; // virtual based on weighting method
			virtual double weight(const ddCoords3 &coords) const = 0;
			ddOutputSingle res;
			std::map<ddCoords3, ddOutputSingle, ddCoordsComp> ensemble;
		};

		class ddOutputEnsembleGaussian : public ddOutputEnsemble
		{
		public:
			ddOutputEnsembleGaussian(double sigma)
			{
				_sigma = sigma;
			}
			virtual ~ddOutputEnsembleGaussian() {}
			virtual void generate() = 0;
			virtual double weight(const ddCoords3 &coords) const = 0;
		protected:
			double _sigma;
			std::map<double,double> _weights;
			void _genWeights(const std::set<double> &points, const
				std::map<double, unsigned int> &recs);
		};

		class ddOutputEnsembleGaussianPhi : public ddOutputEnsembleGaussian
		{
		public:
			ddOutputEnsembleGaussianPhi(double sigma) : ddOutputEnsembleGaussian(sigma)
			{
			}
			virtual ~ddOutputEnsembleGaussianPhi() {}
			virtual void generate();
			virtual double weight(const ddCoords3 &coords) const;
		};
		*/
		class ddCoords3;
		class ddCoordsComp;
		/*
		class ddOutput {
			// Class represents the output of a ddscat run
			// Can be loaded by specifying the path of a ddscat.par file
			// Otherwise, holds an array of all possible rotations that exist
			// and can compute the phase functions for any weighted combination

			// Can write Evans-formatted data
			// TODO: netcdf read/write, ddscat-formatted write
		public:
			ddOutput();
			ddOutput(const std::string &ddscatparFile) { _init(); loadFile(ddscatparFile); }
			void loadFile(const std::string &ddscatparFile);
			// writeEvans interpolates to the quadrature points and writes out the
			// mueller matrix, extinction matrix and emission matrices
			// It assumes that particles are aligned horizontally (but I take care of this)
			//void writeEvans(const std::string scatFile) const;
			void get(const ddCoords3 &coords, ddOutputSingle &f) const;
			void set(const ddCoords3 &coords, const ddOutputSingle &f);

		public:
			void _init();
			mutable std::map<ddCoords3, ddOutputSingle, ddCoordsComp> _data;
		};
		*/
	}; // end namespace ddscat

}; // end namespace rtmath

