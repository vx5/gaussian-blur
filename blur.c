#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
// This code leverages the stb library for reading from and writing to images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_resources/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_resources/stb_image_write.h"
// POSIX threads used for multithreading
#include <pthread.h>
// Defines maximum number of threads
#define PTHREAD_THREADS_MAX 512
// Headers required
#include "created_resources/gen_helper.h"
#include "created_resources/gen_blur.h"
#include "created_resources/gauss_fx.h"
// For number of supported channels
#define TARGET_CHANNELS 3

// META: For use in main call below

// For offsets in uint8_t* image bytes
int channelOffsets[TARGET_CHANNELS] = {0, 1, 2};

// Struct to hold arguments common to all threads
// See gen_blur for info on parameters
struct threadCommonArgs {
	int kernelSize;
	float** kernel; 
	unsigned char* src;
	unsigned char* dst;
	int height;
	int width;
	int numChannels;
	int* channelOffsets;
};

// Struct to hold arguments unique to some threads
// See gen_blur for info on parameters
struct threadArgs {
	struct threadCommonArgs commonArgs;
	int startPixelNum;
	int endPixelNum;
};

/*
INPUT: Struct of type threadArgs (above)
OUTPUT: None
ACTION: Creates thread that 
*/
void* threadApplyKernel(void* args) {
	struct threadArgs* allArgs = (struct threadArgs*) args;
	struct threadCommonArgs commonArgs = allArgs -> commonArgs;
	applyKernel(commonArgs.kernelSize, commonArgs.kernel, 
		commonArgs.src, commonArgs.dst, commonArgs.height, 
		commonArgs.width, commonArgs.numChannels, 
		commonArgs.channelOffsets, allArgs -> startPixelNum, 
		allArgs -> endPixelNum);
	return NULL;
}

int main(int argc, char** argv) {
	time_t timer = time(NULL);
	// META: Reading of input and initialization
	// Catch invalid number of arguments
	if (argc != 5) {
		exitWithError("3-4 arguments required "
		"e.g., \"./gauss_blur example.png 5 4 80\", to blur example.png with kernel size 5 using 4 threads\n"
		"where blurred image will be written with 80% quality");
	}
	// Read input
	char* filename = argv[1];
	int kernelSize = atoi(argv[2]);
	int numThreads = atoi(argv[3]);
	int writeQuality = atoi(argv[4]);
	// Catch invalid kernel size and invalid thread nums
	if (kernelSize % 2 == 0 || kernelSize < 3) exitWithError("Kernel size must be odd integer 3 or greater");
	if (numThreads <= 0) exitWithError("Number of threads must be positive integer");
	if (writeQuality <= 0 || writeQuality > 100) exitWithError("Write quality must be positive integer between 1 and 100");
	// Ensure num threads does not exceed max
	if (numThreads > PTHREAD_THREADS_MAX) numThreads = PTHREAD_THREADS_MAX; 
	// Load file using stb, last argument 0 ensures that all channels are loaded
	printf("Image loading...\n");
	int height, width, numChannels;
	uint8_t* origImage = stbi_load(argv[1], &width, &height, &numChannels, 0);
	// Catch error or invalid loading of image, including wrong number of channels
	if (origImage == NULL || numChannels != TARGET_CHANNELS || !(strncmp(filenameExt(filename), "jpg", 3))) {
		// NOTE: text should be adjusted if number of target channels changes
		exitWithError("File could not be loaded -- please name a .jpg file "
		"with 3 channels (RGB) in the current directory");
	}
	// Catch unusually large kernel size
	if (kernelSize > height || kernelSize > width) exitWithError("Kernel size must be less than min of height, width");
	// Create copy where blurred pixels will be placed
	int imageSize = height * width * numChannels;
	uint8_t* blurred = malloc(imageSize);
	// META: Blurring of image 
	// Initialize kernel
	printf("Building kernel...\n");
	float kernel[kernelSize][kernelSize];
	// Build kernel with given function
	buildKernel(kernelSize, kernel, gaussKernelVal);
	// Convert kernel to useKernel of type float**, needed for use in structs, etc.
	float* useKernel[kernelSize];
	for (int kernelIdx = 0; kernelIdx < kernelSize; kernelIdx++) {
		useKernel[kernelIdx] = kernel[kernelIdx];
	} 
	// Applying kernel to image
	printf("Applying kernel...\n");
	// Multithreads kernel application
	// Initialize struct with key info used by all threads
	struct threadCommonArgs commonArgs;
	commonArgs.kernelSize = kernelSize; 
	commonArgs.kernel = useKernel; 
	commonArgs.src = origImage;
	commonArgs.dst = blurred;
	commonArgs.height = height;
	commonArgs.width = width;
	commonArgs.numChannels = numChannels;
	commonArgs.channelOffsets = channelOffsets;
	// Initialize array of structs to be taken in by threads
	int pixelsPerThread = ceil((height * width) / numThreads);
	int startPixel = 0;
	int endPixel = startPixel + pixelsPerThread;
	struct threadArgs args[numThreads];
	for (int thread = 0; thread < numThreads; thread++) {
		struct threadArgs thisThreadArgs = {commonArgs, startPixel, endPixel};
		args[thread] = thisThreadArgs;
		// Calculates starting and end pixels for each thread
		startPixel += pixelsPerThread;
		endPixel += pixelsPerThread;
		// Ensures last thread's end pixel is in bounds
		if (endPixel > height * width) endPixel = height * width;
	}
	// To retain threads to be joined
	pthread_t threads[numThreads];
	// Create and join thread loops
	for (int thread = 0; thread < numThreads; thread++) {
		if (pthread_create(threads + thread, NULL, threadApplyKernel, &args[thread]) != 0) {
			exitWithError("Error in applying kernel - thread creation");
		}
	}
	for (int thread = 0; thread < numThreads; thread++) {
		if (pthread_join(threads[thread], NULL) != 0) {
			exitWithError("Error in applying kernel - thread joining");
		}
	}
	// META: Writing of image with new filename and exit
	char blurTag[5] = "BLUR_";
	char* newFilename = malloc(sizeof(blurTag) + sizeof(filename) / sizeof(char));
	printf("Writing blurred image...\n");
	stbi_write_jpg(strcat(strcat(newFilename, blurTag), filename), width, height, numChannels, blurred, writeQuality);
	// Free heap memory allocated during execution
	stbi_image_free(origImage);
	free(blurred);
	free(newFilename);
	printf("All done after %d seconds\n", (int) (time(NULL) - timer));
	exit(EXIT_SUCCESS);
}