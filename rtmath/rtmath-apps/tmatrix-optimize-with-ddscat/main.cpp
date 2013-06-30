/* This program is designed to find optimum scaling factors for tmatrix runs 
 * for various particles. The optimum values are determined using ddscat run 
 * results as a base, and Google's Ceres Solver is used to minimize the 
 * defined cost function.
 *
 * This is designed to perform runs with medium-sized numbers of particles, as 
 * repeated t-matrix runs may take some time. All results are written out 
 * to csv files. The overall cost function is minimized in 3d, and gridded 
 * values may also be calculated and exported.
 *
 * Also designed to export results for individual flakes.
 */

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include "../../rtmath/rtmath/ddscat/ddOutputSingle.h"
#include "../../rtmath/rtmath/common_templates.h"
#include "../../rtmath/rtmath/splitSet.h"
#include "../../rtmath/rtmath/ddscat/tmData.h"
#include <Ryan_Serialization/serialization.h>
#include "../../rtmath/rtmath/Serialization/tmData_serialization.h"
#include "../../rtmath/rtmath/units.h"
#include "../../rtmath/rtmath/ddscat/shapestats.h"
#include "../../rtmath/rtmath/ddscat/shapestatsRotated.h"
//#include "../../rtmath/rtmath/ddscat/shapestatsviews.h" /// \todo Fix shapestatsviews?
#include "../../rtmath/rtmath/error/error.h"

/// Class that matches shapefiles to run results.
/// \todo Retool this to match shape files, shape stats and ddscat results
class dataset
{
public:
	dataset() : id("") {}
	dataset(const std::string &prefix) : id(prefix) {}
	// For holly stuff, the shape id is before the first underscore.
	// Other stuff may be appended at the end (lev2), but she nicely splits the folders!
	std::string id;
	// avg file provides freq, dielectric info, qsca, qbk, qext, aeff, d
	std::vector<boost::filesystem::path> ddres; // all avg results (.avg)
	boost::filesystem::path shapestats; // shape stats (.xml)
	boost::filesystem::path shapefile; // the shape file (.shp, _shape.txt)

	// holly shape files are like 1mm14shape.txt or 1mm1_..._shape.txt
	// liu shape files are like avg_5000.shp
	// stat results end in .xml (+.bz2,+...)
	// holly avg files follow 1mm14_t_f.avg
	// liu avg files follow avg_3000[.*]
	static std::string getPrefix(const boost::filesystem::path &filename)
	{
		// Prefix is the first part of the filename.
		using namespace std;
		using namespace boost::filesystem;
		path pleaf = filename.filename();
		path proot = filename.parent_path();
		string sleaf = pleaf.string(); // the filename

		size_t pund = sleaf.find('_'); // May be liu or holly
		size_t pshape = sleaf.find("shape"); // Only Holly
		size_t pshp = sleaf.find(".shp"); // Only Liu
		size_t pavg = sleaf.find("avg_"); // Only Liu

		bool isLiu = false, isHolly = false;
		if (pshp != string::npos) isLiu = true;
		if (pavg != string::npos) isLiu = true;
		if ((pshape != string::npos) && !isLiu) isHolly = true;
		if ((pund != string::npos) && !isLiu) isHolly = true;

		// Based on detection criteria, perform appropriate truncation
		if (isLiu)
		{
			// Liu avg files need no manipulation.
			// Remove .shp - pshp for raw shapes and tmatrix results
			if (pshp != string::npos)
				sleaf = sleaf.substr(0,pshp);
			// Raw liu shapes, tmatrix results re now truncated
		}
		if (isHolly)
		{
			size_t pos = 0;
			if ((pund != string::npos) && (pavg != string::npos)) pund = string::npos;
			if (pund == string::npos && pshape != string::npos) pos = pshape;
			else if (pshape == string::npos && pund != string::npos) pos = pund;
			else if (pshape == pund == string::npos) throw;
			else pos = (pund < pshape) ? pund : pshape;
			sleaf = sleaf.substr(0,pos);
		}

		return sleaf;
	}
	static bool isValid(const boost::filesystem::path &filename)
	{
		using namespace boost::filesystem;
		using namespace std;
		if (is_directory(filename)) return false;
		if (isAvg(filename)) return true;
		if (isShape(filename)) return true;
		if (isShapeStats(filename)) return true;
		return false;
	}
	static bool isAvg(const boost::filesystem::path &filename)
	{
		using namespace boost::filesystem;
		using namespace std;
		path pleaf = filename.filename();

		//string meth; // replaced with std::string()
		if (Ryan_Serialization::detect_compression(pleaf.string(), std::string()))
			pleaf.replace_extension();

		path ext = pleaf.extension();

		if (ext.string() == ".avg") return true;
		return false;
	}
	static bool isShape(const boost::filesystem::path &filename)
	{
		using namespace boost::filesystem;
		using namespace std;
		path pleaf = filename.filename();

		//string meth; // replaced with std::string()
		if (Ryan_Serialization::detect_compression(pleaf.string(), std::string()))
			pleaf.replace_extension();

		path ext = pleaf.extension();
		if (ext.string() == ".shp") return true;
		if (pleaf.string().find_last_of("shape.txt") != string::npos) return true; // Lazy check
		return false;
	}
	static bool isShapeStats(const boost::filesystem::path &filename)
	{
		using namespace boost::filesystem;
		using namespace std;
		path pleaf = filename.filename();

		//string meth; // replaced with std::string()
		if (Ryan_Serialization::detect_compression(pleaf.string(), std::string()))
			pleaf.replace_extension();

		path ext = pleaf.extension();
		if (ext.string() == ".xml") return true;
		return false;
	}
};


int main(int argc, char** argv)
{
	using namespace std;
	try {
		namespace po = boost::program_options;

		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("inputs,i", po::value<vector<string> >(), 
			"Specify input files (.avg, .shp files). "
			"If a folder is speified, recurse and select all matching files.")
			("write-indiv-flakes", "Write output for individual flakes")
			("grid-output", "Perform gridded calculations in addition to pure minimization")
			("cost-function,c", po::value<string>()->default_value("both"),
			"Select the cost function that will be used for minimization. Can be: qsca, qbksc or both.")
			("use-volfrac", po::value<bool>()->default_value(true),
			"Use volume fraction in minimization")
			("use-aeff", po::value<bool>()->default_value(true),
			"Use effective radius in minimization")
			("use-ar", po::value<bool>()->default_value(true),
			"Use aspect ratio in minimization")
			("stats-dir", po::value<string>(),
			"If only a shape file is specified, write stats to this directory. Used for speedups in future runs.")
			("verbose,v", "Verbose output")
			;

		po::positional_options_description p;
		p.add("inputs",-1);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
		po::notify(vm);

		vector<string> rawinputs; // May be expanded by os.

		auto doHelp = [&](const std::string &message)
		{
			cerr << message << endl;
			cerr << desc << endl;
			exit(1);
		};
		if (vm.count("help") || argc == 1) doHelp("");

		bool verbose = false;
		if (vm.count("verbose")) verbose = true;

		if (!vm.count("inputs")) doHelp("Need to specify input files.");
		rawinputs = vm["inputs"].as<vector<string> >();

		bool writeStats = false;
		string statsDir;
		if (vm.count("stats-dir"))
		{
			writeStats = true;
			statsDir = vm["stats-dir"].as<string>();
		}

		using namespace boost::filesystem;

		// sca and fml files have no reference to the originating ddscat runs. However, 
		/* they can be matched by:
		1 - filename
		2 - using the TARGET field - it's the shape.dat title
		3 - header properties (# of dipoles, freq, d, aeff)
		*/

		map<string,dataset> data;

		auto insertMapping = [&](const boost::filesystem::path &p)
		{
			if (dataset::isValid(p) == false) return;
			std::string prefix = dataset::getPrefix(p);
			if (!data.count(prefix))
				data[prefix] = std::move(dataset(prefix));
			if (dataset::isAvg(p))
				data[prefix].ddres.push_back(p);
			if (dataset::isShape(p))
				data[prefix].shapefile = p;
			if (dataset::isShapeStats(p))
				data[prefix].shapestats = p;
		};

		auto expandSymlinks = [](const boost::filesystem::path &p) -> boost::filesystem::path
		{
			using namespace boost::filesystem;
			if (is_symlink(p))
			{
				path pf = boost::filesystem::absolute(read_symlink(p), p.parent_path());
				if (is_directory(pf))
					return pf;
				else
					return p;
			} else {
				return p;
			}
		};

		// Expand directories and validate input files
		// If a directory is specified, recurse through the 
		// structure and pick up all valid shape files.
		for (auto it = rawinputs.begin(); it != rawinputs.end(); ++it)
		{
			path pi(*it);
			pi = expandSymlinks(pi);
			if (!exists(pi)) throw rtmath::debug::xMissingFile(it->c_str());
			if (is_directory(pi) )
			{
				vector<path> cands;
				copy(recursive_directory_iterator(pi,symlink_option::recurse), 
					recursive_directory_iterator(), back_inserter(cands));
				for (auto f = cands.begin(); f != cands.end(); ++f)
				{
					path pf = *f;
					pf = expandSymlinks(pf);
					insertMapping(pf); // already does dir checking
				}
			} else {
				insertMapping(*it);
			}
		}

		// Now that the mappings are established, we can load the 
		// avg and tmm files and perform the computations and 
		// merging operations.

		//ofstream gout(vm["output"].as<string>().c_str());
		// Write csv header
		//writeCSVheader(gout);

		for (auto it = data.begin(); it != data.end(); ++it)
		{
			cerr << "Processing " << it->first << endl;
			// Skip if there are no ddscat results to use
			if (it->second.ddres.size() == 0) continue;
			// Skip if no shape or shape stats file
			if (it->second.shapefile.empty() && it->second.shapestats.empty()) continue;
			
			// If no shape stats file, generate one
			// And place generated stats file in specified directory
			boost::shared_ptr<rtmath::ddscat::shapeFileStats> stats;
			if (!it->second.shapestats.empty())
				Ryan_Serialization::read(stats,it->second.shapestats.string());
			/// \todo Add Ryan_Serialization::read--- T = read<..>(file) alias.
			else
			{
				// Generate the stats. 
				string statfilename;
				if (writeStats)
				{
					it->second.shapestats = path(statsDir);
					it->second.shapestats /= it->second.shapefile.filename();
					it->second.shapestats += "-stats.xml";
				}
				stats = rtmath::ddscat::shapeFileStats::genStats(
					it->second.shapefile.string(), // File to load
					it->second.shapestats.string() // Stats file to save
					);
			}

			// Read the ddscat results


			// A cache is created for each object to prevent multiple loads.
			// It is dumped at the end of reading each data set.
			map<std::string, std::vector<rtmath::tmatrix::tmData> > tmcache;
			map<std::string, rtmath::ddscat::ddOutputSingle> ddcache;

			for (auto st = it->second.ddres.begin(); 
				st != it->second.ddres.end(); ++st)
			{
				using namespace rtmath::ddscat;
				cerr << "\t" << *st << std::endl;
				// Load the dd avg file
				// Read files only once. Cache the results.
				ddOutputSingle ddfile;
				if (ddcache.count(st->string()))
				{
					ddfile = ddcache.at(st->string());
				} else {
					ddfile.readFile(st->string(), ".avg");
					ddcache[st->string()] = ddfile;
				}
				double dSpacing = ddfile.dipoleSpacing();
				double aeff = ddfile.aeff();
				double freq = rtmath::units::conv_spec("um","GHz").convert(ddfile.wave());

				double qbk = ddfile.getStatEntry(QBKM);
				double qsca = ddfile.getStatEntry(QSCAM);
				double qabs = ddfile.getStatEntry(QABSM);
				double qext = ddfile.getStatEntry(QEXTM);
				double g1 = ddfile.getStatEntry(G1M);
				double g2 = ddfile.getStatEntry(G2M);

				// Iterate over all tmatrix and mie runs
				for (auto ot = it->second.tmres.begin(); 
					ot != it->second.tmres.end(); ++ot)
				{
					if (verbose)
						cerr << "\t\tcand\t" << *ot << std::endl;
					// Read files only once. Cache the results.
					std::vector<rtmath::tmatrix::tmData> td;
					//rtmath::tmatrix::tmData td;
					if (tmcache.count(ot->string()))
					{
						td = tmcache.at(ot->string());
					} else {
						::Ryan_Serialization::read<std::vector<rtmath::tmatrix::tmData> >
							(td, ot->string());
						tmcache[ot->string()] = td;
					}

					if (verbose)
						cerr << "There are " << td.size() << " candidates." << endl;
					// Iterate through the tmatrix combinations
					for (auto pt = td.begin(); pt != td.end(); ++pt)
					{
						if (verbose)
							cerr << pt->freq << "\t" << freq << std::endl;
						if (abs(pt->freq - freq) > tol ) continue; // 0.0001

						cerr << "\t\t" << *ot << std::endl;

						// For each tmatrix / mie file, the results are already 
						// combined according to isotropic orientation!
						double tQscam = pt->tstats->stats.at("Qsca");
						double tQextm = pt->tstats->stats.at("Qext");
						double tQbkm = pt->tstats->stats.at("Qbk");


						// Write out the resultant values to a csv file.
						/*
						out << "source,isoflag,oribeta,oritheta,oriphi,frequency,wavelength,sizep,"
						<< "dipole_spacing,T,nu,reff_real,reff_imag,volmeth,dielmeth,shapemeth,"
						<< "aeff_dd,aeff_tm,aspect_ratio,dd_qext,tm_qext,"
						<< "dd_qsca,tm_qsca,dd_qbk,tm_qbk,dd_walb,tm_walb"
						<< std::endl;
						*/
						if (pt->data.size())
						{
							auto qt = pt->data.begin();
							gout << it->first << ",iso,iso,iso,iso,";
							gout << freq << "," << ddfile.wave() << "," << pt->sizep << ","
								<< dSpacing << "," << pt->T << "," << pt->nu << "," 
								<< pt->reff.real() << "," << pt->reff.imag() << ","
								<< pt->volMeth << "," << pt->dielMeth << "," << pt->shapeMeth << ","
								<< aeff << "," << qt->get()->tm->base->axi << ","
								<< qt->get()->tm->base->eps << ","
								<< qext << "," << tQextm << ","
								<< qsca << "," << tQscam << "," 
								<< qbk << "," << tQbkm << ","
								<< qsca/qext << "," << tQscam/tQextm;
						
							gout << std::endl;
						}
						if (pt->miedata.size())
						{
							auto qt = pt->miedata.begin();
							gout << it->first << ",iso,iso,iso,iso,";
							gout << freq << "," << ddfile.wave() << "," << pt->sizep << ","
								<< dSpacing << "," << pt->T << "," << pt->nu << "," 
								<< pt->reff.real() << "," << pt->reff.imag() << ","
								<< pt->volMeth << "," << pt->dielMeth << "," << pt->shapeMeth << ","
								<< aeff << "," << qt->get()->mc->base->axi << ","
								<< "1.0000" << ","
								<< qext << "," << tQextm << ","
								<< qsca << "," << tQscam << "," 
								<< qbk << "," << tQbkm << ","
								<< qsca/qext << "," << tQscam/tQextm;
						
							gout << std::endl;
						}
					}
				}
			}
		}

	}
	catch (std::exception &e)
	{
		cerr << "Exception caught\n";
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}

