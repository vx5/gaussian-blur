#ifndef GAUSS_FX_H
#define GAUSS_FX_H

/*
INPUT: row and col positions relative to center of kernel,
kernel size
OUTPUT: Gaussian value given row and col offsets 
*/
float gaussKernelVal(int row, int col, int kernelSize);

#endif