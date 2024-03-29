#include <string>
#include <iostream>
#include <memory>
#include <exception>
#define BOOST_TEST_DYN_LINK
#include "../rtmath/atmos.h"
#include "../rtmath/atmoslayer.h"
#include "../rtmath/absorb.h"
#include "../rtmath/error/error.h"

#include "globals.h"
#include <boost/test/unit_test.hpp>
//BOOST_GLOBAL_FIXTURE( globals );

BOOST_AUTO_TEST_SUITE(test_atmos);

// These tests are designed to test the functionality of atmos and its associated classes.
// It will lead a sample atmosphere and perform basic optical depth calculations.
// To accomplish this, however, the functionality of the absorber derived classes and of 
// the atmoslayer class must also be verified.

using namespace rtmath;
using namespace rtmath::atmos;

// Construct atmoslayer
BOOST_AUTO_TEST_CASE(atmoslayer_initialization) {
	// atmoslayer comes first, as absorbers depend on it
	atmoslayer sample;
	sample.p(100); // Junk values for testing
	sample.T(200);
	sample.dz(110);

	BOOST_CHECK(sample.p() == 100);
	BOOST_CHECK(sample.T() == 200);
	BOOST_CHECK(sample.dz() == 110);
}

// Perform basic absorption calculations
BOOST_AUTO_TEST_CASE(absorber_calculations) {
	// First, note that absorbers must reference an atmoslayer class that provides 
	// p, T and dz.
	try {
		atmoslayer sample(100,200,10,11,10);
		BOOST_CHECK(sample.p() == 100);
		BOOST_CHECK(sample.T() == 200);
		BOOST_CHECK(sample.dz() == 11);
		// Construct an absorber
		abs_N2 ao; // Can't test H2O, as no wv concentration given.
		ao.setLayer(sample);

		double ret = ao.deltaTau(94);
		BOOST_CHECK(ret > 0); // Just make sure it's valid for now
		
		// Now try and place it in sample and run
		std::shared_ptr<absorber> cln(ao.clone());
		sample.absorbers.insert(cln);
		double retb = cln->deltaTau(94.0);
		BOOST_CHECK(retb == ret);

		// Have sample actually do a tau calculation. With one element, verify consistency.
	}
	catch (rtmath::debug::xError &err)
	{
		err.Display();
		BOOST_FAIL("Test threw an error.");
	}
}

// Verify absorption calculations



// Attempt to clone atmoslayer


// Create atmos and have it load a file
BOOST_AUTO_TEST_CASE(atmos_load_ryan) {
	using namespace std;
	using namespace atmos;
	using namespace boost::filesystem;
	try {
		path profilepath = globals::instance()->pProfiles / "trp.txt";
		//string profilepath = "../profiles/trp.txt";
		rtmath::atmos::atmos trp;
		trp.loadProfileRyanB(profilepath.string());
		double res = 0;
		res = trp.tau(94.0);
		BOOST_CHECK(res > 0);
		//cout << res << endl;
	}
	catch (rtmath::debug::xError &err)
	{
		err.Display();
		BOOST_FAIL("Test threw an error.");
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
		BOOST_FAIL("Test threw an error.");
	}
}


BOOST_AUTO_TEST_SUITE_END();

