/// \brief Provides silo file IO
#define _SCL_SECURE_NO_WARNINGS

#include <string>
#include <boost/math/constants/constants.hpp>

#include "../../rtmath/rtmath/defs.h"
#include "../../rtmath/rtmath/refract.h"
#include "../../rtmath/rtmath/phaseFunc.h"
//#include "../../rtmath/rtmath/ddscat/shapefile.h"
//#include "../../rtmath/rtmath/ddscat/shapestats.h"
//#include "../../rtmath/rtmath/ddscat/ddOutput.h"
#include "../../rtmath/rtmath/conversions/convertLength.h"
#include "../../rtmath/rtmath/plugin.h"
#include "../../rtmath/rtmath/units.h"
#include <Ryan_Debug/debug.h>
#include <Ryan_Debug/error.h>

#include "plugin-nondda-misc.h"

namespace rtmath
{
	namespace plugins
	{
		namespace nondda_misc
		{

			namespace RGi {
				void doCrossSection(
					const rtmath::phaseFuncs::pf_class_registry::setup &s,
					const rtmath::phaseFuncs::pf_class_registry::inputParamsPartial& i,
					rtmath::phaseFuncs::pf_class_registry::cross_sections& c)
				{
					using namespace ::rtmath::phaseFuncs;
					const double pi = boost::math::constants::pi<double>();

					// Force the dielectric constant of solid ice
					std::complex<double> mRes = i.m;
					//std::complex<double> mAir(1.0, 0);
					//i.rmeth(i.m, mAir, i.vFrac, mRes);

					// Expressed in terms of the max diameter
					// ar gives oblate/prolate indicator.
					auto cl = rtmath::units::converter::getConverter(i.lengthUnits, "m");
					auto cs = rtmath::units::converter::getConverter(s.lengthUnits, "m");

					// Perform the calculation
					double ar = 1.; //i.eps;
					bool prolate = false;
					if (ar > 1.0) {
						prolate = true;
						ar = 1./ar;
					}
					//double Dlong = i.maxDiamFull;
					//double Dshort = Dlong * ar;
					using namespace rtmath::units;
					using namespace rtmath::units::keywords;
					double Dshort = convertLength(
						_in_length_value = cl->convert(i.maxDiamFull),
						_in_length_type = "Max_Diameter",
						_out_length_type = "Mean_Diameter",
						_ar = ar);

					const double k = 2. * pi / cs->convert(s.wavelength);
					float qext = -1, qsca = -1, qback = -1, gsca = -1;
					const double f = i.vFrac;
					// V is volume of solid ice in particle
					const double V = 4. / 3. * pi * std::pow(cl->convert(i.aeff), 3.); //i.aeff, scaledAeff

					try {
						std::complex<double> K = ((i.m*i.m) - 1.) / ((i.m*i.m) + 2.);
						std::complex<double> Km = ((mRes*mRes) - 1.) / ((mRes*mRes) + 2.);
						double K2 = (Km * conj(Km)).real();
						qext = -1;
						qsca = -1;
						qback = (float) K2  * 81./4. * V * V
							/ (pi * k * k * pow(Dshort,6.));
						qback *= pow(sin(k*Dshort) - (k*Dshort*cos(k*Dshort)), 2.);
						//qback /= pi * pow(i.aeff,2.);
						gsca = -1;

						c.Csca = -1;
						c.Cbk = qback;
						c.g = -1;
						c.Cext = -1;
						c.Cabs = -1;
					}
					catch (...) {
						//std::cerr << "\t" << t.what() << std::endl;
						RDthrow(Ryan_Debug::error::xOtherError())
							<< Ryan_Debug::error::otherErrorText("A rayleigh-gans error has occurred");
					}
				}
			}
		}
	}
}


