#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
// This code leverages the stb library for reading from and writing to images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_resources/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_resources/stb_image_write.h"
// POSIX threads used for multithreading
#include <pthread.h>
// TODO gauss.h
// For tagging error text with red "ERROR: " declaration and newline
#define ERROR_TAG "\x1b[31mERROR:\x1b[0m "
// For number of supported channels
#define TARGET_CHANNELS 3

// For offsets in uint8_t* image
const int channelOffsets[TARGET_CHANNELS] = {0, 1, 2};

// META: Helper functions

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
void exitWithError(char* errorMsg) {
	fprintf(stderr, ERROR_TAG);
	fprintf(stderr, "%s", errorMsg);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
char* filenameExt(char *filename) {
    char* ext = strrchr(filename, '.');
    if (!ext) return "";
    return ext;
}

/*
To be passed to buildKernel to generate box filter
OUTPUT: 1, regardless of input, so that the kernel
is made up of uniform values that sum to 1
*/
float boxKernel(int row, int col) {
	return 1;
}

/*
INPUT: Size of kernel to be made, function to be applied to elements relative to center of kernel
to populate kernel values
OUTPUT: None
ACTION: Kernel values stored in kernel parameter
NOTE: Memory used to store kernel must be freed
*/
void buildKernel(int kernelSize, float kernel[kernelSize][kernelSize], float (*fx)(int, int)) {
	int kernelRad = kernelSize / 2;
	// Will be used to divide kernel values to that they sum to 1
	float sumKernelVals = 0;
	for (int row = 0; row < kernelSize; row++) {
		for (int col = 0; col < kernelSize; col++) {
			// Calculate kernel value using offsets from center of kernel
			float rawKernelVal = (float) fx(row - kernelRad, col - kernelRad);
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
INPUT: Kernel, kernel size, as well as source image and destination image
OUTPUT: None
ACTIONS: Applies provided kernel to source image bytes, stores result in destination image bytes
*/
void applyKernel(int kernelSize, float kernel[kernelSize][kernelSize], unsigned char* src, unsigned char* dst, int height, int width) {
	// Define kernel radius
	int kernelRad = kernelSize / 2;
	// Iterates through each pixel
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			// Stores sum of all kernel values seen (is only <1 when kernel is not fully contained within image)
			float weightsSeen = 0;		
			// Stores sum of pixel values observed for each channel
			float channelSums[TARGET_CHANNELS];
			// Reset all channel sums to 0
			for (int channel = 0; channel < TARGET_CHANNELS; channel++) channelSums[channel] = 0;
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
						uint8_t* checkPixel = src + ((realRow * width + realCol) * TARGET_CHANNELS);
						// Update channel sums
						for (int channel = 0; channel < TARGET_CHANNELS; channel++) {
							channelSums[channel] += kernelVal * *(checkPixel + channelOffsets[channel]);
						}
					}
				}
			}
			// Access and update relevant pixel information in destination image bytes
			uint8_t* dstPixel = dst + ((row * width + col) * TARGET_CHANNELS);
			for (int channel = 0; channel < TARGET_CHANNELS; channel++) {
				*(dstPixel + channelOffsets[channel]) = (uint8_t) (channelSums[channel] / weightsSeen);
			}
		}
	}
}

int main(int argc, char** argv) {
	// META: Reading of input and initialization
	// Catch invalid number of arguments
	if (argc != 3) {
		exitWithError("2 arguments required "
		"e.g., \"./gauss_blur example.png 5\", to blur example.png with kernel size 5");
	}
	// Read number of threads
	char* filename = argv[1];
	int kernelSize = atoi(argv[2]);
	// Catch invalid kernel size
	if (kernelSize % 2 == 0) exitWithError("Kernel size must be an odd positive integer");
	// Load file using stb, last argument 0 ensures that all channels are loaded
	printf("Image loading...\n");
	int height, width, numChannels;
	uint8_t* origImage = stbi_load(argv[1], &width, &height, &numChannels, 0);
	// Catch error or invalid loading of image, including wrong number of channels
	if (origImage == NULL || numChannels != TARGET_CHANNELS || !(strncmp(filenameExt(filename), "jpg", 3))) {
		exitWithError("File could not be loaded -- please name a .jpg file "
		"with 3 channels (RGB) in the current directory");
	}
	// Catch unusually large kernel size
	if (kernelSize > height || kernelSize > width) exitWithError("Kernel size must be less than min of height, width");
	// Create copy where blurred pixels will be placed
	size_t imageSize = height * width * numChannels;
	uint8_t* blurred = malloc(imageSize);
	// META: Blurring of image 
	// Initialize kernel
	printf("Building kernel...\n");
	float kernel[kernelSize][kernelSize];
	// Build kernel with given function
	buildKernel(kernelSize, kernel, boxKernel);
	// Applying kernel to image
	printf("Applying kernel...\n");
	time_t timex = time(NULL);
	applyKernel(kernelSize, kernel, origImage, blurred, height, width);
	// META: Writing of image with new filename and exit
	char blurTag[5] = "BLUR_";
	char* newFilename = malloc(sizeof(blurTag) + sizeof(filename) / sizeof(char));
	printf("Writing blurred image...\n");
	stbi_write_jpg(strcat(strcat(newFilename, blurTag), filename), width, height, numChannels, blurred, 100);
	// Free heap memory allocated during execution
	stbi_image_free(origImage);
	free(blurred);
	free(newFilename);
	printf("All done after %d seconds\n", (int) (time(NULL) - timex));
	exit(EXIT_SUCCESS);
}