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
// For tagging error text with red "ERROR: " declaration
#define ERROR_FY(string) "\x1b[31mERROR:\x1b[0m " string


char* filenameExt(char *filename) {
    char* ext = strrchr(filename, '.');
    if (!ext) return "";
    printf("%s", ext);
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

void makeBlurredImage(unsigned char* origImage, unsigned char* blurredImage, float** kernel) {
	// 


	// 
}

// Main takes in command line arguments
int main(int argc, char** argv) {
	// Catch invalid number of arguments
	if (argc != 3) {
		char invalidNumArgs[] = ERROR_FY("2 arguments required "
		"e.g., \"./gauss_blur example.png 5\", to blur example.png with kernel size 5\n");
		fprintf(stderr, invalidNumArgs);
		exit(EXIT_FAILURE);
	}
	// Read number of threads
	int kernelSize = atoi(argv[2]);
	// Catch invalid kernel size
	if (kernelSize % 2 == 0) {
		char invalidNumArgs[] = ERROR_FY("Kernel size must be an odd positive integer\n");
		fprintf(stderr, invalidNumArgs);
		exit(EXIT_FAILURE);
	}
	// Load file using stb, last argument 0 ensures that all channels are loaded
	int height, width, numChannels;
	const unsigned char* origImage = stbi_load(argv[1], &width, &height, &numChannels, 0);
	// Catch error or invalid loading of iamge
	if (origImage == NULL || filenameExt(argv[1]) != "jpg") {
		char invalidNumArgs[] = ERROR_FY("File could not be loaded -- please name a .jpg file "
		"in the current directory\n");
		fprintf(stderr, invalidNumArgs);
		exit(EXIT_FAILURE);
	}
	// Catch unusually large kernel size
	if (kernelSize > height || kernelSize > width) {
		char invalidNumArgs[] = ERROR_FY("Kernel size must be less than min of height, width\n");
		fprintf(stderr, invalidNumArgs);
		exit(EXIT_FAILURE);
	}
	// Create copy where blurred pixels will be placed
	size_t imageSize = height * width * numChannels;
	unsigned char* blurred = malloc(imageSize);
	// 
	stbi_image_free(origImage);
	free(blurred);
	exit(EXIT_SUCCESS);
}