#include <iostream>
#include "../rtmath/rtmath.h"
#include "../rtmath/rtmath-base.h"
#include "../rtmath/mie.h"
#include "../rtmath/rayleigh.h"
#include <complex>
#include <time.h>
//#include <netcdf.h>
//#include <netcdfcpp.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <omp.h>
#include <memory>
#include <string>
#include <boost/filesystem.hpp> // used for location of output of netcdf

#include "../rtmath/debug_mem.h"
#include "../rtmath/damatrix_quad.h"
#include "../rtmath/rayleigh.h"
#include "../rtmath/ddscat2.h"

void test()
{
	// Begin by loading files

	// I now have several files. Select the subset whete Theta ranges
	// from 0 to 90 degrees

	// Perform averaging of phase functions to generate isotropic solution
	// will have 91 or 181 possible theta (lc)

	// Write out results
}

int main(int argc, char* argv[])
{
	using namespace std;
	using namespace rtmath;
	using namespace rtmath::ddscat;
	using namespace boost::filesystem;

	try {
		// Take ddscat name or path from argv, and attempt to load the files
		if (argc == 1) 
		{
			cout << "Error: no files specified.\n";
			std::getchar();
			return 1;
		}
		string file(argv[1]);
		boost::filesystem::path p(file.c_str()), dir, outfile, 
			outiso, out5s, out10s, out15s, out30s;
		dir = p.remove_filename();
		cout << "Loading from: " << dir << endl << endl;

		//ddOutputSingle a(file);
		ddOutput a(file);
		std::map<ddCoords3, ddOutputSingle, ddCoordsComp>::const_iterator it;
		it = a._data.begin();
		//cout << it->first.beta << " " << it->first.theta << " " << it->first.phi << endl;

		// Write output in the same folder as the input
		outfile = dir / "out.nc";
		outiso = dir / "isotropic.nc";
		out5s = dir / "sigma5.nc";
		out10s = dir / "sigma10.nc";
		out15s = dir / "sigma15.nc";
		out30s = dir / "sigma30.nc";

		//it->second.print();
		it->second.writeCDF(outfile.string());
		cout << "Output written to " << outfile << endl;
		//cout << "Loaded " << a._data.size() << " files" << endl;

		// Now, try and generate ensemble results
		cout << "Generating ensembles" << endl;
		ddOutputEnsembleIso ensiso;
		ensiso._ensemble = a._data;
		ensiso.generate();
		//ens.res.print();
		ensiso.res.writeCDF(outiso.string());
		cout << "Isotropic output written to " << outiso << endl;
		cout << endl << endl;

		ddOutputEnsembleGaussianTheta s5(0,5);
		s5._ensemble = a._data;
		s5.generate();
		s5.res.writeCDF(out5s.string());

		ddOutputEnsembleGaussianTheta s10(0,10);
		s10._ensemble = a._data;
		s10.generate();
		s10.res.writeCDF(out10s.string());

		ddOutputEnsembleGaussianTheta s15(0,15);
		s15._ensemble = a._data;
		s15.generate();
		s15.res.writeCDF(out15s.string());

		ddOutputEnsembleGaussianTheta s30(0,30);
		s30._ensemble = a._data;
		s30.generate();
		s30.res.writeCDF(out30s.string());

		cout << "Sigma 5, 10, 15 and 30 written to files." << endl << endl;

		std::set<double> THETAS;
		// Do iteration to see bounds on THETA
		for (it=a._data.begin(); it != a._data.end(); it++)
		{
			double th = it->second._Theta;
			if (THETAS.count(th) == 0)
				THETAS.insert(th);
			//it->second._Theta
		}

		cout << "THETAS processed:\n";
		std::set<double>::iterator tit;
		for(tit=THETAS.begin(); tit != THETAS.end(); tit++)
		{
			cout << *tit << "\t";
		}
		cout << endl << endl;

		std::set<double> PHIS;
		// Do iteration to see bounds on PHI
		for (it=a._data.begin(); it != a._data.end(); it++)
		{
			double th = it->second._Phi;
			if (PHIS.count(th) == 0)
				PHIS.insert(th);
			//it->second._Theta
		}

		cout << "PHIS processed:\n";
		for(tit=PHIS.begin(); tit != PHIS.end(); tit++)
		{
			cout << *tit << "\t";
		}
		cout << endl << endl;

	}
	catch (...)
	{
		cerr << "Error thrown" << endl;

	}

	std::getchar();
	return 0;
}
