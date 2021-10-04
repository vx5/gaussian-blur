#include <math.h>
// Header file
#include "gauss_fx.h"

/*
INPUT: row and col positions relative to center of kernel,
kernel size
OUTPUT: Gaussian value given row and col offsets 
*/
float gaussKernelVal(int row, int col, int kernelSize) {
	/* 
	Sigma is assumed to follow the below formula, see link for rationale:
	https://dsp.stackexchange.com/questions/10057/gaussian-blur-standard-deviation-radius-and-kernel-size
	*/
	float sigma = (kernelSize - 1) / 4.0;
	// Gaussian probability distribution function
	float mCand = 1.0 / (2.0 * M_PI * pow(sigma, 2));
	float exp = -1 * (pow(row, 2) + pow(col, 2)) / (2 * pow(sigma, 2));
	return mCand * pow(M_E, exp);
}