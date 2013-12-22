/* shape-extract
 * Reads a shape file and writes the desired visualization format
 */

#include <functional>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <Ryan_Debug/debug.h>

#include "../../rtmath/rtmath/common_templates.h"
#include "../../rtmath/rtmath/ddscat/shapefile.h"
#include "../../rtmath/rtmath/error/debug.h"

int main(int argc, char** argv)
{
	using namespace std;
	using namespace rtmath;
	using namespace boost::filesystem;

	try {
		cerr << "rtmath-shape-extract\n\n";

		namespace po = boost::program_options;

		po::positional_options_description p;
		p.add("input", -1);

		po::options_description desc("Allowed options"), cmdline("Command-line options"), 
			config("Config options"), hidden("Hidden options"), oall("all options");

		cmdline.add_options()
			("help,h", "produce help message")
			("input,i", po::value< string >(), "input shape file")
			("output,o", po::value< string >(), "output shape file")
			("bov,b", po::value<string>(), "output bov file prefix")
			("decimate", po::value<vector<size_t> >()->multitoken(), "Perform decimation with the given kernel sizing")
			("enhance", po::value<vector<size_t> >()->multitoken(), "Perform enhancement with the given kernel sizing")
			("decimate-threshold", po::value<size_t>(), "Use threshold decimation method")
			;

		desc.add(cmdline).add(config);
		oall.add(cmdline).add(config).add(hidden);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(oall).positional(p).run(), vm);
		po::notify(vm);

		auto doHelp = [&](const std::string &message)
		{
			cerr << desc << "\n";
			if (message.size()) cerr << message << endl;
			exit(1);
		};

		using rtmath::ddscat::shapefile::shapefile;

		if (vm.count("help") || argc == 1) doHelp("");
		if (!vm.count("input")) doHelp("Need to specify an input file.");
		string input = vm["input"].as<string>();
		cerr << "Reading input shape file " << input << endl;
		boost::shared_ptr<shapefile> shp(new shapefile);
		shp->read(input);

		if (vm.count("decimate"))
		{
			vector<size_t> kernel = vm["decimate"].as<vector<size_t> >();
			if (kernel.size() < 3) kernel.assign(3, kernel.at(0));
			shapefile::decimationFunction df = shapefile::decimateDielCount;
			if (vm.count("decimate-threshold"))
			{
				size_t threshold = vm["decimate-threshold"].as<size_t>();
				using namespace std::placeholders;
				//rtmath::ddscat::convolutionCellInfo ci;
				df = std::bind(shapefile::decimateThreshold,_1,threshold);
			}
			boost::shared_ptr<shapefile> dec = shp->decimate
				(kernel[0], kernel[1], kernel[2], df);
			shp = dec;
		}

		if (vm.count("enhance"))
		{
			vector<size_t> kernel = vm["enhance"].as<vector<size_t> >();
			if (kernel.size() < 3) kernel.assign(3, kernel.at(0));
			boost::shared_ptr<shapefile> dec = shp->enhance
				(kernel[0], kernel[1], kernel[2]);
			shp = dec;
		}

		if (vm.count("output"))
		{
			string sOutput = vm["output"].as<string>();
			cerr << "Writing shape file as " << sOutput << endl;
			shp->write(sOutput);
		}

		if (vm.count("bov"))
		{
			string bPrefix = vm["bov"].as<string>();
			cerr << "Writing BOV files with prefix " << bPrefix << endl;
			//shp.write(string(bPrefix).append("-orig.dat"));
			shp->writeBOV(bPrefix);
		}

	}
	catch (rtmath::debug::xError &err)
	{
		err.Display();
		cerr << endl;
		return 1;
	} catch (std::exception &e)
	{
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}

