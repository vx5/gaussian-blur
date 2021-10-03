#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
INPUT: String filename
OUTPUT: File extension (e.g., "png") if present, else empty string
*/
char* filenameExt(char *filename) {
    char* ext = strrchr(filename, '.');
    if (!ext) return "";
    return ext;
}

/*
void makeGaussKernel(float** kernel, int kernelSize) {
	int kernelRad = kernelSize / 2;
	float sumKernelVals = 0;
	int sigma = (kernelSize - 1) / 6;
	for (int row = 0; row < kernelSize; row++) {
		for (int col = 0; col < kernelSize; col++) {
			int relRow = row - kernelRad;
			int relCol = col - kernelRad;
			float gaussVal = gaussFx(relRow, relCol, sigma);
			kernel[row][col] = gaussVal;
			sumKernelVals += gaussVal;
		}
	}
	for (int row = 0; row < kernelSize; row++) {
		for (int col = 0; col < kernelSize; col++) {
			kernel[row][col] /= sumKernelVals;
		}
	}
}
*/

void makeBlurredImage(const unsigned char* origImage, unsigned char* blurredImage, float** kernel) {
	// 


	// 
}

// Main takes in command line arguments
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
	int height, width, numChannels;
	uint8_t* origImage = stbi_load(argv[1], &width, &height, &numChannels, 0);
	// Catch error or invalid loading of iamge
	if (origImage == NULL || !(strncmp(filenameExt(filename), "jpg", 3))) {
		exitWithError("File could not be loaded -- please name a .jpg file "
		"in the current directory");
	}
	// Catch unusually large kernel size
	if (kernelSize > height || kernelSize > width) exitWithError("Kernel size must be less than min of height, width");
	// Create copy where blurred pixels will be placed
	size_t imageSize = height * width * numChannels;
	uint8_t* blurred = malloc(imageSize);
	// META: Blurring of image 
	uint8_t* fromPtr = origImage;
	uint8_t* toPtr = blurred;
	while (fromPtr < origImage + imageSize) {
		*toPtr = (uint8_t) ((*fromPtr + *(fromPtr + 1) + *(fromPtr + 2)) / 3.0);
		fromPtr += numChannels;
		toPtr += 1;
	}
	// Populate blurred image
	// META: Writing of image and exit
	char* newFilename = malloc(19 + sizeof(filename) / sizeof(char));
	stbi_write_jpg(strcat(strcat(newFilename, "blur_"), filename), width, height, 1, blurred, 100);
	stbi_image_free(origImage);
	free(blurred);
	free(newFilename);
	exit(EXIT_SUCCESS);
}