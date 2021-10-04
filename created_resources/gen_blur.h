#ifndef GEN_BLUR_H
#define GEN_BLUR_H

/*
To be passed to buildKernel to generate box filter
OUTPUT: 1, regardless of input, so that the kernel
is made up of uniform values that sum to 1
*/
float boxKernel(int row, int col, int kernelSize);

/*
INPUT: Size of kernel to be made, function to be applied to elements relative to center of kernel
to populate kernel values (with inputs as row offset, col offset, kernelSize)
OUTPUT: None
ACTION: Kernel values stored in kernel parameter
NOTE: Memory used to store kernel must be freed
*/
void buildKernel(int kernelSize, float kernel[kernelSize][kernelSize], float (*fx)(int, int, int));

/*
INPUT: Kernel, kernel size, as well as source image and destination image, and number of channels, as
well as offsets from a given byte location for a pixel, which are used to access different channels
(e.g., for uint8_t* from stb_image, 0 for the red channel, 1 for the green channel, 2 for the blue channel)
OUTPUT: None
ACTIONS: Applies provided kernel to source image bytes, stores result in destination image bytes
*/
void applyKernel(int kernelSize, float** kernel, unsigned char* src, unsigned char* dst, 
	int height, int width, int numChannels, int channelOffsets[numChannels], int startPixelNum, int endPixel);

#endif