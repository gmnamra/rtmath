#include <memory>
#include <iostream>
#include <boost/filesystem.hpp>
#include "../../rtmath/rtmath/rtmath.h"

int main(int argc, char **argv)
{
	using namespace std;
	using namespace rtmath;
	using namespace rtmath::debug;
	using namespace rtmath::ddscat;
	using namespace rtmath::units;
	using namespace boost::filesystem;
	try {
		cerr << "Batch genparams test program" << endl;
		rtmath::debug::appEntry(argc, argv);
		rtmath::config::parseParams p(argc, argv);

		// Will create a test set of ddscat runs
		// these will vary frequencies, sizes and shapes for a set of test cases
		// The purpose is to ascertain, develop and debug the corresponding library routines.
		// Also, the routines will be used in qt and ncurses programs

		// Use the default generator
		ddscat::ddParGenerator gen;

		// Set name
		gen.name = "Test run";

		// Set description
		gen.description = "genparams-test test to ensure proper file generation";

		// Set frequencies
		gen.freqs.insert(hasUnits(94.0, "GHz"));
		gen.freqs.insert(hasUnits(110.0, "GHz"));

		// Set temperature
		gen.temps.insert(hasUnits(-10.0, "C"));
		gen.temps.insert(hasUnits(267.0, "K"));

		// Set the shape rotations
		rotations ra; // use default values
		rotations rb(0,0,1,0,90,18,0,0,1);
		gen.rots.insert(ra);
		gen.rots.insert(rb);

		std::shared_ptr<shapeSphere> bshape(new shapeSphere());
		gen.setShapeBase(bshape);

		// The important constraints are now set. The output-writing routine
		// will write out the description file, and the generating routine will
		// generate the full directory structure.

		// Generate
		gen.generate("testout");

		// Write the output
		gen.write("test.txt");
	}
	catch (rtmath::debug::xError &err)
	{
		err.Display();
		return 1;
	}
	return 0;
}
