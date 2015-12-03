#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <Ryan_Debug/debug.h>
#include <Ryan_Debug/splitSet.h>
#include "../scatdb_ryan/scatdb_ryan.hpp"

int main(int argc, char** argv) {
	try {
		using namespace std;
		namespace po = boost::program_options;
		po::options_description desc("Allowed options"), cmdline("Command-line options"),
			config("Config options"), hidden("Hidden options"), oall("all options");

		Ryan_Debug::add_options(cmdline, config, hidden);

		cmdline.add_options()
			("help,h", "produce help message")
			("stats", "Print stats for selected data")
			("xaxis,x", po::value<string>()->default_value("aeff"), "Specify independent "
			 "axis for interpolation and lowess regression. Default is aeff. Can also use "
			 "'md' for max dimension.")
			("lowess", "Use LOWESS Routine to regress data")
			("interp", "Perform linear interpolation over the x axis.")
			//("integ", po::value<string>(), "Integrate using Sekhon-Srivastava distribution for "
			// "the specified rainfall rates [mm/h]. Suggested 0.1:0.1:3.")
			("output,o", po::value<string>(), "Output file")
			("dbfile,d", po::value<string>(), "Manually specify database location")
			("flaketypes,y", po::value<string>(), "Filter flaketypes by number range")
			("frequencies,f", po::value<string>(), "Filter frequencies (GHz) by range")
			("temp,T", po::value<string>(), "Filter temperatures (K) by range")
			("aeff,a", po::value<string>(), "Range filter by effective radius (um)")
			("max-dimension,m", po::value<string>(), "Range filter by maximum dimension (mm)")
			("cabs", po::value<string>(), "Range filter by absorption cross-section (m)")
			("cbk", po::value<string>(), "Range filter by backscattering cross-section (m)")
			("cext", po::value<string>(), "Range filter by extinction cross-section (m)")
			("csca", po::value<string>(), "Range filter by scattering cross-section (m)")
			("asymmetry,g", po::value<string>(), "Range filter by asymmetry parameter")
			("ar", po::value<string>(), "Range filter by aspect ratio")
			;

		desc.add(cmdline).add(config);
		oall.add(cmdline).add(config).add(hidden);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(oall).run(), vm);
		po::notify(vm);

		Ryan_Debug::process_static_options(vm);

		auto doHelp = [&](const std::string& s)
		{
			cout << s << endl;
			cout << desc << endl;
			exit(3);
		};

		if (vm.count("help") || vm.size() == 0) doHelp("");

		using namespace scatdb_ryan;
		std::string dbfile;
		if (vm.count("dbfile")) dbfile = vm["dbfile"].as<string>();
		db::findDB(dbfile);
		if (!dbfile.size()) doHelp("Unable to detect database file.");
		cerr << "Database file is at: " << dbfile << endl;
		auto sdb = db::loadDB(dbfile.c_str());
		cerr << "Database loaded. Performing filtering." << endl;

		auto f = filter::generate();

		if (vm.count("flaketypes")) f->addFilterInt(db::data_entries::FLAKETYPE, vm["flaketypes"].as<string>());
		if (vm.count("frequencies")) f->addFilterFloat(db::data_entries::FREQUENCY_GHZ, vm["frequencies"].as<string>());
		if (vm.count("temp")) f->addFilterFloat(db::data_entries::TEMPERATURE_K, vm["temp"].as<string>());
		if (vm.count("aeff")) f->addFilterFloat(db::data_entries::AEFF_UM, vm["aeff"].as<string>());
		if (vm.count("max-dimension")) f->addFilterFloat(db::data_entries::MAX_DIMENSION_MM, vm["max-dimension"].as<string>());
		if (vm.count("cabs")) f->addFilterFloat(db::data_entries::CABS_M, vm["cabs"].as<string>());
		if (vm.count("cbk")) f->addFilterFloat(db::data_entries::CBK_M, vm["cbk"].as<string>());
		if (vm.count("cext")) f->addFilterFloat(db::data_entries::CEXT_M, vm["cext"].as<string>());
		if (vm.count("csca")) f->addFilterFloat(db::data_entries::CSCA_M, vm["csca"].as<string>());
		if (vm.count("asymmetry")) f->addFilterFloat(db::data_entries::G, vm["asymmetry"].as<string>());
		if (vm.count("ar")) f->addFilterFloat(db::data_entries::AS_XY, vm["ar"].as<string>());

		auto sdb_filtered = f->apply(sdb);
		string sxaxis = vm["xaxis"].as<string>();
		db::data_entries::data_entries_floats xaxis = db::data_entries::AEFF_UM;
		if (sxaxis == "md") xaxis = db::data_entries::MAX_DIMENSION_MM;

		auto le_filtered = sdb_filtered;
		if (vm.count("lowess")) {
			le_filtered = sdb_filtered->regress(xaxis);
		}
		if (vm.count("stats")) {
			auto stats = le_filtered->getStats();
			cerr << "Stats tables:" << endl;
			stats->print(cerr);
		}
		auto interp_filtered = le_filtered;
		if (vm.count("interp")) {
			interp_filtered = le_filtered->interpolate(xaxis);
		}

		if (vm.count("integ")) {
			std::string sinteg = vm["integ"].as<std::string>();
		}

		if (vm.count("output")) {
			std::string fout = vm["output"].as<std::string>();
			cerr << "Writing output to " << fout << endl;
			std::ofstream out(fout.c_str());
			interp_filtered->print(out);
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
