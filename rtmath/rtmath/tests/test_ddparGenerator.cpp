#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#include "globals.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

#include "../rtmath/rtmath.h"


BOOST_AUTO_TEST_SUITE(test_ddparGenerator);

using namespace rtmath;
using namespace std;
using namespace boost;
using namespace rtmath::debug;
using namespace rtmath::ddscat;
using namespace rtmath::units;

// Test to construct ddparGenerator
BOOST_AUTO_TEST_CASE(pargenerator_construct)
{
	// Trivial construction
	ddParGenerator p;
	// Known base ddPar
	ddParGenerator q(*rtmath::ddscat::ddPar::defaultInstance());
}

// Test to prepare a sample ddparGenerator file
BOOST_AUTO_TEST_CASE(pargenerator_sample)
{
	ddParGenerator p;
	p.name = "test run";
	p.description = "desc goes here";
	p.outLocation = "./testrun";

	p.ddscatVer = 72;
	p.compressResults = true;

	// Set temp, aeff
	p.addConstraint(shapeConstraint::create("temp", -10, "C"));
	p.addConstraint(shapeConstraint::create("aeff", "50:100:350", "um"));

	// Set shape
	boost::shared_ptr< rtmath::ddscat::shapes::from_file > shp(new rtmath::ddscat::shapes::from_file());
	shp->addConstraint(shapeConstraint::create("aeff", 200, "um"));
	shp->addConstraint(shapeConstraint::create("source_filename", 0, "testdiel1.tab"));
	// Note: comparison at end may fail if multiple source_filenames present, as there is 
	//	no particular ordering involved. Files are the same otherwise
	//shp->addConstraint(shapeConstraint::create("source_filename", 0, "testdiel2.tab"));
	p.shapes.insert(shp);


	// Set rotations
	p.rots.insert( rotations::create(p.base) );

	// Now, attempt to save and load the file
	string sObj, sObjb;
	ostringstream out, outb;
	rtmath::serialization::write<ddParGenerator>(p,out);
	rtmath::serialization::write<ddParGenerator>(p,"test.parGenerator");

	ddParGenerator q;
	sObj = out.str();
	istringstream in(sObj);
	rtmath::serialization::read<ddParGenerator>(q,in);
	rtmath::serialization::write<ddParGenerator>(q,outb);
	sObjb = outb.str();
	
	BOOST_CHECK(sObj == sObjb);
}

// Read from ddPar file




// Read a known ddparGenerator file

// Attempt to generate a batch of runs

//

BOOST_AUTO_TEST_SUITE_END();

/*
		std::map<std::string, std::string> freqaliases;
		freqaliases["GPM_Radiometer"] = "10.7, 18.7, 23.8, 89.0, 165.5, 183.3";
		freqaliases["GPM_Dual_Radar"] = "13.6, 35.5";
		gen.freqs.insert(std::make_pair<paramSet<double>, std::string>
			( paramSet<double>("35.5,GPM_Radiometer,GPM_Dual_Radar", &freqaliases), "GHz" )   );


		shp->shapeConstraints.insert(std::make_shared<shapeConstraint>("shpar1","1"));
*/