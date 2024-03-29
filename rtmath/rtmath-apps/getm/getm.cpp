// this is a drop-in replacement for Liu's getm and genmtab
// It will add extra functionality, such as a better selection of refractive index
// calculation functions. It will also be able to handle both ice and water.

#include <memory>
#include <complex>
#include <cmath>
#include <cstring>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <Ryan_Debug/debug.h>
#include <Ryan_Debug/splitSet.h>
#include <Ryan_Debug/io.h>
#include "../../rtmath/rtmath/error/debug.h"
#include "../../rtmath/rtmath/refract.h"
#include "../../rtmath/rtmath/units.h"
#include "../../rtmath/rtmath/ddscat/shapefile.h"
#include "../../rtmath/rtmath/ddscat/dielTabFile.h"
#include "../../rtmath/rtmath/command.h"

enum PHASE {
	ICE,
	WATER,
	AIR
};

int main(int argc, char** argv)
{
	using namespace std;
	try {
		//rtmath::debug::appEntry(argc, argv);
		bool mtab = false;

		// detect if rtmath-mtab is being called

		namespace po = boost::program_options;
		po::options_description desc("Allowed options"), cmdline("Command-line options"), 
			config("Config options"), hidden("Hidden options"), oall("all options");
		rtmath::debug::add_options(cmdline, config, hidden);

		cmdline.add_options()
			("help,h", "produce help message")
			("mtab,m", "Produce mtab-style output")
			("debug", "Produce debug output")
			("frequency,f", po::value<string>(), "List of frequencies (default of GHz). Can also take rtmath.conf-provided frequency key (ex: GPM_freqs).")
			("in-units", po::value<string>()->default_value("GHz"), "Units for all frequencies. Can also be in wavelengths or spectral wavenumbers.")
			("output-units", po::value<string>()->default_value("um"), "Output units (should be a wavelength).")
			("temperature,T", po::value<string>(), "List of temperatures")
			("temperature-units", po::value<string>()->default_value("K"), "Units for temperatures.")
			//("salinity,S", po::value<string>(), "List of salinities (TBI)")
			//("salinity-units", po::value<string>()->default_value("ppm"), "Units for salinity.")
			("volume-fraction-ice,i", po::value<string >(), "Volume fractions [0,1] for mixed phase volumes. If not given, calculate from others.")
			("volume-fraction-water,w", po::value<string>(), "Water volume fractions. If not given, calculation depends on presence of both ice and water fractions. If both are given, w = 1 - a - i. If only one is given, assume that water fraction is zero.")
			("volume-fraction-air,a", po::value<string>(), "Air volume fraction. If not given, calculate from others.")
			("nu", po::value<string>()->default_value("0.85"), "Value of nu for Sihvola (default is 0.85. Range is [0,2].")
			("method", po::value<string>()->default_value("Maxwell-Garnett-Ellipsoids"), "Method used to calculate the resulting dielectric "
			"(Sihvola, Debye, Maxwell-Garnett-Spheres, Maxwell-Garnett-Ellipsoids, Bruggeman). "
			"Only matters if volume fractions are given. Then, default is Sihvola.")
			("ice-provider", po::value<string>()->default_value(""), "Provider for the raw ice dielectric. Function name from rtmath::refract.")
			("water-provider", po::value<string>()->default_value(""), "Provider for the raw water dielectric. Function name from rtmath::refract.")
			("other-provider", po::value<string>()->default_value(""), "Provider for the other dielectric. Function name from rtmath::refract.")

			("input-shape,s", po::value<string>(), "Specify shapefile for inner dielectric calculation. Provides volume fraction.")
			("calck,k", "Calculate K coefficient");
			;

		po::positional_options_description p;
		p.add("frequency",1);
		p.add("temperature",2);

		desc.add(cmdline).add(config);
		oall.add(cmdline).add(config).add(hidden);

		po::variables_map vm;
		po::store(po::command_line_parser(argc,argv).
			options(oall).positional(p).run(),
			vm );
		po::notify (vm);

		rtmath::debug::process_static_options(vm);

		rtmath::ddscat::dielTab dfile;
		string method, sTemps, sFreqs, sNus, ofile, unitsFreq, unitsWvlen, unitsTemp;
		string sshape, waterProvider, iceProvider, otherProvider;
		set<double> temps, freqs, nus;
		map<PHASE,string> vsVols;
		map<PHASE,set<double> > vVols;
		bool debug = false, calck = false;

		unitsFreq = vm["in-units"].as<string>();
		unitsWvlen = vm["output-units"].as<string>();
		unitsTemp = vm["temperature-units"].as<string>();

		//("ice-provider", po::value<string>()->default_value("mIce"), "Provider for the raw ice dielectric.")
		//("water-provider", po::value<string>()->default_value("mWater"), "Provider for the raw water dielectric.")
		waterProvider = vm["water-provider"].as<string>();
		iceProvider = vm["ice-provider"].as<string>();
		otherProvider = vm["other-provider"].as<string>();

		if (vm.count("debug"))
			debug = true;
		if (vm.count("calck")) calck = true;

		if (vm.count("input-shape")) sshape = vm["input-shape"].as<string>();
		if (vm.count("method"))
			method = vm["method"].as<string>();
		if (vm.count("temperature"))
			sTemps = vm["temperature"].as<string>();
		if (vm.count("frequency"))
			sFreqs = vm["frequency"].as<string>();
		if (vm.count("nu"))
			sNus = vm["nu"].as<string>();
		if (vm.count("volume-fraction-air"))
			vsVols[AIR] = vm["volume-fraction-air"].as<string>();
		if (vm.count("volume-fraction-water"))
			vsVols[WATER] = vm["volume-fraction-water"].as<string>();
		if (vm.count("volume-fraction-ice"))
			vsVols[ICE] = vm["volume-fraction-ice"].as<string>();

		auto doHelp = [&](const std::string &s)
		{
			cerr << desc << endl;
			cerr << s << endl;
			exit(1);
		};

		if (vm.count("help") || vm.size() == 0 || argc == 1 ||
			!vm.count("temperature") || !vm.count("frequency"))
			doHelp("");

		Ryan_Debug::splitSet::splitSet<double>(sTemps,temps);
		Ryan_Debug::splitSet::splitSet<double>(sFreqs,freqs);
		Ryan_Debug::splitSet::splitSet<double>(sNus,nus);
		for (auto it = vsVols.begin(); it != vsVols.end(); ++it)
		{
			set<double> s;
			Ryan_Debug::splitSet::splitSet<double>(it->second,s);
			vVols[it->first] = move(s);
		}
		if (sshape.size())
		{
			if (!vVols.count(PHASE::ICE)) vVols[PHASE::ICE] = set<double>();

			std::vector<boost::shared_ptr<rtmath::ddscat::shapefile::shapefile> > shps;
			Ryan_Debug::io::readObjs(shps, sshape);
			if (!shps.size()) doHelp("File must contain shapes");
			auto shp = shps[0];
			double numInner = 0, numOccupied = 0, frac = 0;
			if (shp->tags.count("inner-perturbation-numInnerLatticeSites"))
				numInner = boost::lexical_cast<double>(shp->tags.at("inner-perturbation-numInnerLatticeSites"));
			if (shp->tags.count("inner-perturbation-numOccupiedInnerLatticeSites"))
				numOccupied = boost::lexical_cast<double>(shp->tags.at("inner-perturbation-numOccupiedInnerLatticeSites"));
			if (numInner && numOccupied)
			{
				frac = numOccupied / numInner;
				vVols[PHASE::ICE].insert(frac);
			} else doHelp("Need to specify a flake with perturbative structure recorded");
		}

		if (vm.count("mtab")) mtab = true;
		{
			using namespace boost::filesystem;
			path p(argv[0]);
			path fname = p.filename();
			if (fname == path("rtmath-genmtab"))
				mtab = true;
		}

		if (mtab)
		{
			if (temps.size() > 1 || nus.size() > 1 || vVols.size() > 1)
			{
				RDthrow(Ryan_Debug::error::xBadInput())
					<< Ryan_Debug::error::otherErrorText("mtab output can only vary by frequency");
			}
		}

		// Preprocessing done!

		// Figure out tuples of volume fractions
		// Tuple ordering is ice, water, air
		vector<boost::tuple<double, double, double> > fracs;
		{
			bool hasIce = false, hasWater = false, hasAir = false;
			if (vVols.count(ICE)) hasIce = true;
			if (vVols.count(WATER)) hasWater = true;
			if (vVols.count(AIR)) hasAir = true;
		
			// Set up the tuples by varying over possible known parameters and
			// attempting to fill in the unknowns.

			if (!hasIce && !hasWater && !hasAir)
			{
				// Do air / water determination at a later step.
				// This null tuple is detected later in the code
				fracs.push_back(boost::tuple<double,double,double>(0.0,0.0,0.0));
			} else {
				// This is ugly, but is shorter than coding graph theory
				if (!hasWater)
				{
					if (hasIce && hasAir)
					{
						for (auto ice = vVols[ICE].begin(); ice != vVols[ICE].end(); ++ice)
						{
							for (auto air = vVols[AIR].begin(); air != vVols[AIR].end(); ++air)
							{
								double water = 1.0 - *ice - *air;
								fracs.push_back( boost::tuple<double, double, double>(*ice, water, *air));
							}
						}
					} else {
						double water = 0;
						// Missing water and either ice or air
						if (hasAir)
						{
							for (auto air = vVols[AIR].begin(); air != vVols[AIR].end(); ++air)
							{
								double ice = 1.0 - *air;
								fracs.push_back(boost::tuple<double,double,double>(ice,water,*air));
							}
						}

						if (hasIce)
						{
							for (auto ice = vVols[ICE].begin(); ice != vVols[ICE].end(); ++ice)
							{
								double air = 1.0 - *ice;
								fracs.push_back(boost::tuple<double,double,double>(*ice,water,air));
							}
						}
					}
				} else {
					// has Water
					for (auto water = vVols[WATER].begin(); water != vVols[WATER].end(); ++water)
					{
						// Does it have ice, air, both or none?
						if (!hasIce & !hasAir)
						{
							double ice = 0;
							double air = 1.0 - *water;
							fracs.push_back(boost::tuple<double,double,double>(ice,*water,air));
						} else if (hasIce)
						{
							for (auto ice = vVols[ICE].begin(); ice != vVols[ICE].end(); ++ice)
							{
								double air = 1.0 - *water - *ice;
								fracs.push_back(boost::tuple<double,double,double>(*ice,*water,air));
							}
						} else {
							for (auto air = vVols[AIR].begin(); air != vVols[AIR].end(); ++air)
							{
								double ice = 1.0 - *water - *air;
								fracs.push_back(boost::tuple<double,double,double>(ice,*water,*air));
							}
						}
					}
				}
			}
		}

		// Actual calculations
		for (auto rT = temps.begin(); rT != temps.end(); ++rT)
		{
			double T = rtmath::units::converter(unitsTemp, "K").convert(*rT);
			for (auto tempfreq = freqs.begin(); tempfreq != freqs.end(); ++tempfreq)
			{
				// Yes, this is ugly, but I wanted to avoid renaming everything.
				boost::shared_ptr<double> freq(new double); // in GHz
				rtmath::units::conv_spec cnv(unitsFreq, "GHz");
				*freq = cnv.convert(*tempfreq);
				for (auto nu = nus.begin(); nu != nus.end(); ++nu)
				{
					for (auto frac = fracs.begin(); frac != fracs.end(); ++frac)
					{
						// Call appropriate function depending on method
						complex<double> mAir(1.0,0);
						complex<double> mWat;
						complex<double> mIce;
						complex<double> mOther;

						double fIce = frac->get<0>();
						double fWat = frac->get<1>();
						double fAir = frac->get<2>();
						double fOther = 0;

						if (fIce == 0 && fWat == 0 && fAir == 0 && fOther == 0)
						{
							if (T < 273.15)
							{
								fIce = 1.0;
							} else {
								fWat = 1.0;
							}
						}

						using rtmath::refract::_frequency;
						using rtmath::refract::_temperature;
						using rtmath::refract::_m;
						using rtmath::refract::_provider;
						if (fIce)
							rtmath::refract::mIce(_frequency = *freq, _temperature = T, _m = mIce, _provider = iceProvider.c_str());
						if (fWat)
							rtmath::refract::mWater(_frequency = *freq, _temperature = T, _m = mWat, _provider = waterProvider.c_str());
						if (fOther)
							rtmath::refract::mOther(_frequency = *freq, _temperature = T, _m = mOther, _provider = otherProvider.c_str());

						if (debug)
							cerr << fIce << "," << fWat << "," << fAir << "," << fOther << "," << T << "," 
							<< *freq << "," << *nu << "," << mIce << "," << mWat << "," << mAir << "," << mOther;
						if (fIce + fWat + fAir + fOther > 1.0)
						{
							if (debug)
								cerr << " - Invalid\n";
							continue;
						} if (fIce < 0 || fIce > 1.0 || fWat < 0 || fWat > 1.0 || fAir < 0 || fAir > 1.0 || fOther < 0 || fOther > 1.0)
						{
							if (debug)
								cerr << " - Invalid\n";
							continue;
						} else {
							if (debug)
								cerr << endl;
						}

						complex<double> mEff;
						if (fIce && fWat) {
							RDthrow(Ryan_Debug::error::xUnimplementedFunction())
								<< Ryan_Debug::error::otherErrorText(
								"Currently, the code cannot handle mixing rules "
								"with both ice and water!");
						}
						if (fIce < 1 && fWat < 1) {
							// TODO: extend to multiple media
							complex<double> mCur;
							double f = 0;
							if (fIce) { f = fIce; mCur = mIce; }
							if (fWat) { f = fWat; mCur = mWat; }

							if (method == "Sihvola")
							{
								rtmath::refract::sihvola(mCur,mAir,f,*nu,mEff);
							} else if (method == "Debye")
							{
								rtmath::refract::debyeDry(mCur,mAir,f, mEff);
							} else if (method == "Maxwell-Garnett-Spheres")
							{
								rtmath::refract::maxwellGarnettSpheres(mCur,mAir,f,mEff);
							} else if (method == "Maxwell-Garnett-Ellipsoids")
							{
								rtmath::refract::maxwellGarnettEllipsoids(mCur,mAir,f,mEff);
							} else if (method == "Bruggeman")
							{
								rtmath::refract::bruggeman(mCur, mAir, f, mEff);
							} else {
								cerr << "Unknown method: " << method << endl;
								RDthrow(Ryan_Debug::error::xBadInput());
							}
						} else {
							if (fIce == 1) mEff = mIce;
							if (fWat == 1) mEff = mWat;
						}

						// Write effective refractive index
						if (mtab)
						{
							// Convert frequency (GHz) to wavelength (um)
							// units may be overridden by user options
							// freq is already converted to GHz at this point.
							rtmath::units::conv_spec cnv("GHz", unitsWvlen);
							double wvlen = cnv.convert(*freq);
							dfile.freqMMap[wvlen] = mEff;
							//write(mEff,mtab);
						} else {
							cout.setf( ios::scientific, ios::floatfield);
							cout.precision(7);
							cout << " ( " << mEff.real() << " , " << mEff.imag() << " ) " << endl;
						}
						if (calck) {
							complex<double> K = ( mEff*mEff - complex<double>(1,0))
								/ ( mEff*mEff + complex<double>(2,0));
							cout << " K is " << abs(K) << ", and Ksq is " << pow(abs(K),2.) << endl;
						}
					}
				}
			}
		}

		if (mtab) {
			dfile.write(cout);
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
