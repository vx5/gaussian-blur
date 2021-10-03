#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// This code leverages the stb library for reading from and writing to images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_resources/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_resources/stb_image_write.h"
// POSIX threads used for multithreading
#include <pthread.h>
// Headers required
#include "gen_helper.h"
#include "gen_blur.h"
#include "gauss_fx.h"
// For number of supported channels
#define TARGET_CHANNELS 3

// For offsets in uint8_t* image bytes
const int channelOffsets[TARGET_CHANNELS] = {0, 1, 2};
// For image writing quality
const int blurredImageQuality = 100;

// META: Helper functions




int main(int argc, char** argv) {
	time_t timer = time(NULL);
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
	applyKernel(kernelSize, kernel, origImage, blurred, height, width, numChannels, channelOffsets);
	// META: Writing of image with new filename and exit
	char blurTag[5] = "BLUR_";
	char* newFilename = malloc(sizeof(blurTag) + sizeof(filename) / sizeof(char));
	printf("Writing blurred image...\n");
	stbi_write_jpg(strcat(strcat(newFilename, blurTag), filename), width, height, numChannels, blurred, blurredImageQuality);
	// Free heap memory allocated during execution
	stbi_image_free(origImage);
	free(blurred);
	free(newFilename);
	printf("All done after %d seconds\n", (int) (time(NULL) - timer));
	exit(EXIT_SUCCESS);
}