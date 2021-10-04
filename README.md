# Gaussian JPG blurring

## Introduction

This projects allows the user to blur a 3-channel (RGB) JPG image using a Gaussian kernel. Image blurring often uses a kernel (a 2D array of values) to color each pixel based on a weighted average of surrounding pixels. A Gaussian kernel populates the 2D array using a Gaussian probability distribution, which is used to weight the surrounding cell's values. It allows the user to select the degree of blurring (by kernel size, rather than the sigma of the Gaussian function, based on online notes about ease-of-use), the number of threads to use (multithreading is implemented), and the quality of the output JPG. An example of blurring is below:

**Before:**
![Pre blur](https://github.com/vx5/gaussian-blur/blob/main/example.jpg?raw=true)

**After:**
![Post blur](https://github.com/vx5/gaussian-blur/blob/main/BLUR_example.jpg?raw=true)

## Key details

#### How to access

Place the "gauss_blur" executable in the same directory (allowing for direct access) as the image you wish to blur. Then run the executable, also supplying the size of blurring kernel you wish to use (an odd integer at least 3 -- the larger the kernel, the greater the blur effect, but the more intensive the blurring process), the number of threads you wish to use (generally enter the number of cores on your machine which can parallelize the blurring task), and the quality of the resulting JPG (a number between 1 and 100). For example:

```
./gauss_blur target.jpg 25 2 90
```

I would now have an image in the directory called BLUR_target.jpg, which is a blurred version of target.jpg. Blurring would have used a kernel of size 25 x 25, 2 threads (my machine has 2 cores which, together, can parallelize 2 tasks), and written the resulting JPG with 90% quality. Note that there may be a substantial speed-up using multithreading when appropriate (e.g., on my 2-core machine, using 2 threads typically halves the time vs. using 1 thread).

#### Project design details

blur.c contains the core code for receiving and responding to user input. Within the created_resources directory, gen_blur contains general functions relevant for blurring (even if not using a Gaussian kernel), while gauss_fx contains a simple implementation of the Gaussian function relevant for this project.

For reading from and writing to JPG files, I used the STB library (header files are in stb_resources).

When the kernel stretches partially extends past the bounds of the image, the resulting weighted average is only taken of pixels within the bounds of the image.

#### Optimizations

I implemented multithreading, using POSIX threads/pthreads, to improve execution speed. Based on the number of threads, the images is divided into several sections -- each thread determines the blurred version of its subsection. The speed improvement may be considerable -- below is an example of trying to blur a large image with 1 vs. 2 threads:

![Speed improvement](https://github.com/vx5/gaussian-blur/blob/main/images/performance.jpg?raw=true)

#### Future considerations 

This project may be work-in-progress, as there may be further opportunities to improve execution speed within a single thread by exploring more efficient code practices, caching, etc.

#### Acknowledgments

I used many external resources to learn about the image blurring process, learn how to use the STB library, learn how to use the pthread library, and debug errors in C. A few include AtoZ Programming Tutorials videos, Stack Exchange posts (I link a page in gauss_fx.c that was helpful), and The Open Group's documentation pages.