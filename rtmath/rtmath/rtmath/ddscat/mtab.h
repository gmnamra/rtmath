#pragma once

/* The purpose of this header is to generate the mtab files - files which contain 
 * information on the index of refraction for a scatterer. This index of refraction 
 * is frequency and temperature-dependent, and it also depends on the material and 
 * phase being considered. So, the mtab file class will link into providers that 
 * provide this information. The default provider will be for ice.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <complex>
#include "../matrixop.h"
#include "../refract.h"

namespace rtmath {
	namespace ddscat {

		// Use a function pointer to specify the provider.
		// The provider is a function of the form void (double f, double t, complex<double> &m)
		class mtab
		{
		public:
			mtab(void (*provider)(double, double, std::complex<double>&) = &rtmath::refract::mice);
			virtual ~mtab();

			void f(double newF) { _f = newF; }
			double f() const { return _f; }
			void T(double newT) { _T = newT; }
			double T() const { return _T; }

			virtual void eval(std::complex<double> &res) const;
			void write(std::ostream &out) const;
			void write(const std::string &filename) const;

		protected:
			//mtab();
			double _f;
			double _T;
			void (*_provider)(double, double, std::complex<double>&);
		};
	}
}

std::ostream & operator<<(std::ostream &stream, const rtmath::ddscat::mtab &ob);

