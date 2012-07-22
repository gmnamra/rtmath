/* shape
 * A program designed for analyzing ddscat shape.par files and the associated ___ files output when 
 * ddscat runs. The shape file is loaded, and various properties of the shape are analyzed. 
 * In particular, the moments of inertia are calculated for the shape, and its most likely 
 * axis, given lamellar-like flow conditions, is calculated. This is important because it 
 * indicates if the shapes that are generated contain the appropriate angle calculations!
 * 
 * Also, ancillary useful quantities are calculated. Crystal mass is not, as these shapes may be scaled.
 * However, the number of dipoles is presented, along with crystal volume relations for a given 
 * radius. 
 */
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma warning( disable : 4800 )
#include <cmath>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TNamed.h>
#include <TGraph2D.h>
#include <TStyle.h>
#include <TH2.h>
#pragma warning( pop ) 
#include "../../rtmath/rtmath/rtmath.h"


void doHelp();

int main(int argc, char** argv)
{
	using namespace std;
	using namespace rtmath;
	using namespace boost::filesystem;

	try {
		cerr << "rtmath-shape\n\n";
		rtmath::debug::appEntry(argc, argv);
		//if (argc == 1) doHelp(); // Undesirable.
		config::parseParams p(argc,argv);


		// Begin by parsing the parameters. For example, it is useful to determine where the shape file 
		// is located. By default, assume the current directory and read in a path given the option.
		// It the option path is a directory, figure out the likely file.
		if (p.readParam("-h")) doHelp();
		//bool flag = false;
		string shapepath = "./";
		p.readParam<string>("-s",shapepath);
		path pshapepath(shapepath);
		if (!exists(pshapepath)) throw rtmath::debug::xMissingFile(shapepath.c_str());
		// Is this a directory or a file
		if (is_directory(pshapepath))
		{
			// Figure out where the shape file is located.
			path ptarget = pshapepath / "target.out";
			path pshapedat = pshapepath / "shape.dat";
			if (exists(ptarget))
			{ pshapepath = ptarget;
			} else if (exists(pshapedat))
			{ pshapepath = pshapedat;
			} else {
				throw rtmath::debug::xMissingFile("shape.dat or target.out");
			}
			shapepath = pshapepath.string();
		}

		// File found and it exists!

		// Specify output location of analysis...
		string outpath = shapepath;
		outpath.append(".stats.xml");
		//string outpath = "./shapeanalysis.out";
		p.readParam<string>("-o",outpath);

		// Specify beta, theta, phi rotations
		string sbeta, stheta, sphi;
		if (!p.readParam<string>("-b",sbeta)) doHelp();
		if (!p.readParam<string>("-t",stheta)) doHelp();
		if (!p.readParam<string>("-p",sphi)) doHelp();

		paramSet<double> betas(sbeta);
		paramSet<double> thetas(stheta);
		paramSet<double> phis(sphi);

		// Should plots be generated?
		//bool doPlots = p.readParam("-P");



		// Start the run from here
		//ofstream out(outpath.c_str());

		// Load the shape file
		cerr << "Processing " << shapepath << endl;
		rtmath::ddscat::shapefile shp(shapepath);

		cerr << "\tCalculating baseline statistics" << endl;
		rtmath::ddscat::shapeFileStats sstats(shp);
		for (auto beta = betas.begin(); beta != betas.end(); beta++)
		{
			for (auto theta = thetas.begin(); theta != thetas.end(); theta++)
			{
				for (auto phi = phis.begin(); phi != phis.end(); phi++)
				{
					cerr << "\tCalculating rotation (beta,theta,phi): ("
						<< *beta << ", " << *theta << ", " << *phi << ")" << endl;
					sstats.calcStatsRot(*beta,*theta,*phi);
				}
			}
		}

		cerr << "Done calculating. Writing results." << endl;
		rtmath::serialization::write<rtmath::ddscat::shapeFileStats>(sstats,outpath);
		//shp.print(out);
	}
	catch (rtmath::debug::xError &err)
	{
		err.Display();
		cerr << endl;
		return 1;
	}
	return 0;
}

void doHelp()
{
	using namespace std;
	cerr << "Options:\n";
	cerr << "-h\n";
	cerr << "\tDisplay this help message.\n";
	cerr << "-s (file or directory)\n";
	cerr << "\tSpecify the shape file or its containing directory.\n";
	cerr << "\tIf a directory, try autodetection (default).\n";
	cerr << "-o (output filename)\n";
	cerr << "\tOutput file of analysis. Default is ./shapeanalysis.out\n";
	cerr << "-b, -t, -p\n";
	cerr << "\tSpecify ranges for rotations beta, theta, phi\n";
	cerr << endl << endl;
	exit(1);
}
