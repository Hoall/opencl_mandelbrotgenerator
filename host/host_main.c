/*
 * host_main.c
 *
 *      Author: Felix Paetow
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

#include "../resources/error_code.h"

#define LENGTH 1000

// Kernel

const char *KernelSource =
		"\n"
				"__kernel void multi(                                                 \n"
				"   __global float* a,                                                  \n"
				"   const unsigned int count)                                           \n"
				"{                                                                      \n"
				"   int i = get_global_id(0);                                           \n"
				"   if(i < count)                                                       \n"
				"       a[i] = a[i] * 2;                                                \n"
				"}                                                                      \n"
				"\n";

int main(void) {
	int err;               // error code returned from OpenCL calls

	float* h_a = (float*) calloc(LENGTH, sizeof(float));       // a vector

	size_t global;                  // global domain size

	cl_device_id device_id;     // compute device id
	cl_context context;       // compute context
	cl_command_queue commands;      // compute command queue
	cl_program program;       // compute program
	cl_kernel ko_multi;       // compute kernel

	cl_mem d_a;                    // device memory used for the input  a vector

	int count = LENGTH;
	int i;

	//set up array
	for (i = 0; i < count; ++i) {
		h_a[i] = (float) i;
	}

	// Set up platform and GPU device

	cl_uint numPlatforms;

	// Find number of platforms
	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkError(err, "Finding platforms");
	if (numPlatforms == 0) {
		printf("Found 0 platforms!\n");
		return EXIT_FAILURE;
	}

	// Get all platforms
	cl_platform_id Platform[numPlatforms];
	err = clGetPlatformIDs(numPlatforms, Platform, NULL);
	checkError(err, "Getting platforms");

	// Secure a GPU
	for (i = 0; i < numPlatforms; i++) {
		err = clGetDeviceIDs(Platform[i], DEVICE, 1, &device_id, NULL);
		if (err == CL_SUCCESS) {
			break;
		}
	}

	if (device_id == NULL)
		checkError(err, "Finding a device");

	err = output_device_info(device_id);
	checkError(err, "Printing device output");

	// Create a compute context
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	checkError(err, "Creating context");

	// Create a command queue
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	checkError(err, "Creating command queue");

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    checkError(err, "Creating program");

	// Build the program
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		size_t len;
		char buffer[2048];

		printf("Error: Failed to build program executable!\n%s\n",
				err_code(err));
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
				sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		return EXIT_FAILURE;
	}

	// Create the compute kernel from the program
	ko_multi = clCreateKernel(program, "multi", &err);
	checkError(err, "Creating kernel");

	d_a = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * count, NULL,
			&err);
	checkError(err, "Creating buffer d_a");

	// Write a vector into compute device memory
	err = clEnqueueWriteBuffer(commands, d_a, CL_TRUE, 0, sizeof(float) * count,
			h_a, 0, NULL, NULL);
	checkError(err, "Copying h_a to device at d_a");

	// Set the arguments to our compute kernel
	err = clSetKernelArg(ko_multi, 0, sizeof(cl_mem), &d_a);
	err |= clSetKernelArg(ko_multi, 1, sizeof(unsigned int), &count);
	checkError(err, "Setting kernel arguments");

	// Execute the kernel over the entire range of our 1d input data set
	// letting the OpenCL runtime choose the work-group size
	global = count;
	err = clEnqueueNDRangeKernel(commands, ko_multi, 1, NULL, &global, NULL, 0,
	NULL, NULL);
	checkError(err, "Enqueueing kernel");

	// Wait for the commands to complete before stopping the timer
	err = clFinish(commands);
	checkError(err, "Waiting for kernel to finish");

	// Read back the results from the compute device
	err = clEnqueueReadBuffer(commands, d_a, CL_TRUE, 0, sizeof(float) * count,
			h_a, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error: Failed to read output array!\n%s\n", err_code(err));
		exit(1);
	}

    for(i = 0; i < count; i++)
    {
    	printf("%d %f\n", i, h_a[i]);
    }

    // cleanup then shutdown
    clReleaseMemObject(d_a);
    clReleaseProgram(program);
    clReleaseKernel(ko_multi);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    free(h_a);

	return 0;
}
