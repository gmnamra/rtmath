/* This program ingests Beth's data and generates hdf5 files */
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Ryan_Debug/debug.h>

#include "observation.h"
#include "parser.h"
#include "export.h"

int main(int argc, char** argv)
{
	using namespace std;
	using namespace boost::filesystem;

	try {
		cerr << "obsSkinTemp-convert\n\n";

		namespace po = boost::program_options;

		po::positional_options_description p;
		p.add("input", -1);

		po::options_description desc("Allowed options");

		desc.add_options()
			("help,h", "produce help message")
			("input,i", po::value< vector<string> >()->multitoken(), "input text files")
			("recursive,r", "Select recursive directory input")
			("output,o", po::value< string >(), "output file")
			;

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
		po::notify(vm);  

		auto doHelp = [&](const std::string &message)
		{
			cerr << desc << "\n";
			if (message.size())
				cerr << message << "\n";
			exit(1);
		};

		if (vm.count("help") || argc == 1) doHelp("");
		if (!vm.count("input")) doHelp("Need to specify input file(s).\n");
		vector<string > inputs = vm["input"].as< vector<string > >();
		string output;
		if (vm.count("output")) output= vm["output"].as< string >();
		else doHelp("Need to specify an output file.\n");

		bool recurse = false;
		if (vm.count("recursive")) recurse = true;
		auto expandFolders = [&](const vector<string> &src, vector<path> &dest)
		{
			dest.clear();
			for (auto s : src)
			{
				using namespace boost::filesystem;
				path p(s);
				if (is_directory(p))
				{
					if (!recurse)
						copy(directory_iterator(p), 
						directory_iterator(), back_inserter(dest));
					else
						copy(recursive_directory_iterator(p,symlink_option::recurse), 
						recursive_directory_iterator(), back_inserter(dest));
				}
				else dest.push_back(p);
			}
		};

		vector<boost::filesystem::path> vinputs;
		expandFolders(inputs, vinputs);

		std::vector<observation> obs;
		obs.reserve(12000000);

		for (const auto &si : vinputs)
		{
			// Validate input file
			path pi(si);
			if (!exists(pi)) throw std::string("Missing file ").append(pi.string());
			if (is_symlink(pi)) pi = boost::filesystem::read_symlink(pi);
			if (!exists(pi)) throw std::string("Missing symbolic link location ").append(pi.string());
			if (is_directory(pi)) continue;
			if (pi.filename().string().at(0) == '.') continue;
			//if (pi.extension().string() != ".dat") continue;
			if (pi.string().find("landObsSkintempLR") == string::npos) continue;

			cerr << "Input: " << si << endl;
			//cerr << "Input: " << pi.filename() << endl;

			parse_file(pi.string(), obs);
		}

		if (!obs.size()) throw std::string("No data was successfully read!");

		std::sort(obs.begin(), obs.end(), [](const observation &a, const observation &b) -> bool
		{
			if (a.temp != b.temp) return a.temp < b.temp;
			if (a.skinTemp != b.skinTemp) return a.skinTemp < b.skinTemp;
			if (a.wbTemp != b.wbTemp) return a.wbTemp < b.wbTemp;
			if (a.pres != b.pres) return a.pres < b.pres;
			return false;
		});

		// Add in some padding to allow chunking to work.
		// There are 22407693 entries in the sample ingest.
		// Do a chunk for every 10000 entries.
		size_t rawSize = obs.size();
		const size_t chunkSize = 10000;
		size_t numChunks = (obs.size() / chunkSize);
		if (obs.size() % chunkSize) numChunks++;
		size_t targetSize = numChunks*chunkSize;
		obs.resize(targetSize);

		exportToHDF(output, obs, rawSize, chunkSize);
		
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
		return 1;
	}
	catch (std::string &e)
	{
		cerr << e << endl;
		return 1;
	}
	return 0;
}

