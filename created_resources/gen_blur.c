// Standard required headers
#include <stdio.h>
#include <stdlib.h>
// Header file
#include "gen_blur.h"

/*
To be passed to buildKernel to generate box filter
OUTPUT: 1, regardless of input, so that the kernel
is made up of uniform values that sum to 1
*/
float boxKernel(int row, int col, int kernelSize) {
	return 1;
}

/*
INPUT: Size of kernel to be made, function to be applied to elements relative to center of kernel
to populate kernel values
OUTPUT: None
ACTION: Kernel values stored in kernel parameter
NOTE: Memory used to store kernel must be freed
*/
void buildKernel(int kernelSize, float kernel[kernelSize][kernelSize], float (*fx)(int, int, int)) {
	int kernelRad = kernelSize / 2;
	// Will be used to divide kernel values to that they sum to 1
	float sumKernelVals = 0;
	for (int row = 0; row < kernelSize; row++) {
		for (int col = 0; col < kernelSize; col++) {
			// Calculate kernel value using offsets from center of kernel
			float rawKernelVal = fx(row - kernelRad, col - kernelRad, kernelSize);
			kernel[row][col] = rawKernelVal;
			// Sum all raw kernel values
			sumKernelVals += rawKernelVal;
		}
	}
	// Divide kernel values so that they sum to 1
	for (int row = 0; row < kernelSize; row++) {
		for (int col = 0; col < kernelSize; col++) {
			kernel[row][col] /= sumKernelVals;
		}
	}
}

/*
INPUT: Kernel, kernel size, as well as source image and destination image, and number of channels, as
well as offsets from a given byte location for a pixel, which are used to access different channels
(e.g., for uint8_t* from stb_image, 0 for the red channel, 1 for the green channel, 2 for the blue channel).
Also takes in pixel number (when counting row by row from the left) to start and end (not included)
OUTPUT: None
ACTIONS: Applies provided kernel to source image bytes, stores result in destination image bytes
*/
void applyKernel(int kernelSize, float** kernel, unsigned char* src, unsigned char* dst, 
	int height, int width, int numChannels, int channelOffsets[numChannels], int startPixelNum, int endPixelNum) {
	// Define kernel radius
	int kernelRad = kernelSize / 2;
	// Iterates through each pixel
	for (int pixelNum = startPixelNum; pixelNum < endPixelNum; pixelNum++) {
		int row = pixelNum / width;
		int col = pixelNum % width;
		// Stores sum of all kernel values seen (is only <1 when kernel is not fully contained within image)
		float weightsSeen = 0;		
		// Stores sum of pixel values observed for each channel
		float channelSums[numChannels];
		// Reset all channel sums to 0
		for (int channel = 0; channel < numChannels; channel++) channelSums[channel] = 0;
		// Iterates through each offset from current pixel allowed by the kernel
		for (int rowOff = -1 * kernelRad; rowOff <= kernelRad; rowOff++) {
			for (int colOff = -1 * kernelRad; colOff <= kernelRad; colOff++) {
				// Checks whether offsets are still in image bounds
				int realRow = row + rowOff;
				int realCol = col + colOff;
				if (realRow >= 0 && realRow < height && realCol >= 0 && realCol < width) {
					// Access kernel value
					float kernelVal = kernel[rowOff + kernelRad][colOff + kernelRad];
					weightsSeen += kernelVal;
					// Access relevant offset pixel
					uint8_t* checkPixel = src + ((realRow * width + realCol) * numChannels);
					// Update channel sums
					for (int channel = 0; channel < numChannels; channel++) {
						channelSums[channel] += kernelVal * *(checkPixel + channelOffsets[channel]);
					}
				}
			}
		}
		// Access and update relevant pixel information in destination image bytes
		uint8_t* dstPixel = dst + ((row * width + col) * numChannels);
		for (int channel = 0; channel < numChannels; channel++) {
			*(dstPixel + channelOffsets[channel]) = (uint8_t) (channelSums[channel] / weightsSeen);
		}	
	}
}