#pragma once
#include "defs.h"

#include <complex>
#include <functional>

#include <Ryan_Debug/error.h>

namespace rtmath {

	namespace zeros {
		// Zero-finding implementation - Brent's method
		double DLEXPORT_rtmath_core findzero(double a, double b, const std::function<double(double) > & evaltarget);

		// f is an arbitrary class with operator(). So, a functional / lambda function does work.
		template<class T, class U>
		U secantMethod(const T &f, 
			U guess_a, U guess_b,
			double eps = 0.000001, size_t maxIter = 50)
		{
			// Secant method is defined by recurrance
			// xn = x_(n-1) - f(x_(n-1)) * (x_(n-1) - x_(n-2)) / (f(x_(n-1)) - f(x_(n-2)))
			using namespace std;
			U zero;
			U xn = guess_a, xn1 = guess_b, xn2;
			U fxn1, fxn2;
			size_t i=0;
			do {
				xn2 = xn1;
				xn1 = xn;

				fxn1 = f(xn1);
				fxn2 = f(xn2);

				xn = xn1 - fxn1 * (xn1 - xn2) / (fxn1 - fxn2);
			} while ( (abs(xn-xn1) > eps) && (i++ < maxIter));

			if (i >= maxIter) RDthrow(Ryan_Debug::error::xModelOutOfRange());
			zero = xn;
			return zero;
		}

	}

}

