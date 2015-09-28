/*
 * host_main.c
 *
 *      Author: Felix Paetow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "../resources/my_complex.h"

int main(void) {
	//###############################################
	//
	// Declare variables for OpenCL
	//
	//###############################################
	int err;               // error code returned from OpenCL calls

	size_t global;                  // global domain size

	cl_device_id device_id;     // compute device id
	cl_context context;       // compute context
	cl_command_queue commands;      // compute command queue
	cl_program program;       // compute program
	cl_kernel ko_calculate_imagerowdots_iterations;       // compute kernel

	cl_mem d_a;                    // device memory used for the input  a vector

	int i;

	//###############################################
	//
	// Set values for mandelbrot
	//
	//###############################################

	//plane section values
	float x_ebene_min = -1;
	float y_ebene_min = -1;
	float x_ebene_max = 2;
	float y_ebene_max = 1;

	//monitor resolution values
	const long x_mon = 640;
	const long y_mon = 480;

	//Iterations
	long itr = 100;

	//abort condition
	float abort_value = 2;

	//Get memory for image
	long* h_image = (long*) calloc(x_mon * y_mon, sizeof(long));

	//###############################################
	//
	// Set up platform and GPU device
	//
	//###############################################

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

	//###############################################
	//
	// Create context, command queue and kernel
	//
	//###############################################

	// Create a compute context
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	checkError(err, "Creating context");

	// Create a command queue
	commands = clCreateCommandQueue(context, device_id, 0, &err);
	checkError(err, "Creating command queue");

	//Read Kernel source
	FILE *fp;
	char *source_str;
	size_t source_size, program_size;

	fp = fopen("./kernel/calculate_iterations.cl", "r");
	if (!fp) {
		printf("Failed to load kernel\n");
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	program_size = ftell(fp);
	rewind(fp);
	source_str = (char*) malloc(program_size + 1);
	source_str[program_size] = '\0';
	fread(source_str, sizeof(char), program_size, fp);
	fclose(fp);

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, (const char **) &source_str,
	NULL, &err);

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

		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL,
				&log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
				log_size, log, NULL);

		// Print the log
		printf("%s\n", log);

		return EXIT_FAILURE;
	}

	// Create the compute kernel from the program
	ko_calculate_imagerowdots_iterations = clCreateKernel(program,
			"calculate_imagerowdots_iterations", &err);
	checkError(err, "Creating kernel");


	//###############################################
	//###############################################
	//
	// Loop to calculate image dot iterations
	//
	//###############################################
	//###############################################

	float y_value = y_ebene_max;
	float delta_y = delta(y_ebene_min, y_ebene_max, y_mon);

	for (int row = 0; row < y_mon; ++row) {
		//###############################################
		//
		// Create and write buffer
		//
		//###############################################

		//Get memory for row
		long* h_image_row = (long*) calloc(x_mon, sizeof(long));     // a vector

		d_a = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(long) * x_mon,
		NULL, &err);
		checkError(err, "Creating buffer d_a");

		// Write a vector into compute device memory
		err = clEnqueueWriteBuffer(commands, d_a, CL_TRUE, 0,
				sizeof(long) * x_mon, h_image_row, 0, NULL, NULL);
		checkError(err, "Copying h_a to device at d_a");

		//###############################################
		//
		// Set the arguments to our compute kernel
		//
		//###############################################

		err = clSetKernelArg(ko_calculate_imagerowdots_iterations, 0,
				sizeof(float), &x_ebene_min);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 1,
				sizeof(float), &x_ebene_max);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 2,
				sizeof(float), &y_value);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 3,
				sizeof(long), &x_mon);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 4,
				sizeof(float), &abort_value);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 5,
				sizeof(long), &itr);
		err |= clSetKernelArg(ko_calculate_imagerowdots_iterations, 6,
				sizeof(cl_mem), &d_a);
		checkError(err, "Setting kernel arguments");

		/*__kernel void calculate_imagerowdots_iterations(const float x_min, const float x_max,
		 const float y_value, const long x_mon, const float abort_value, const long itr,
		 __global long * imagerow)*/

		// Execute the kernel over the entire range of our 1d input data set
		// letting the OpenCL runtime choose the work-group size
		global = x_mon;
		err = clEnqueueNDRangeKernel(commands,
				ko_calculate_imagerowdots_iterations, 1, NULL, &global, NULL, 0,
				NULL, NULL);
		checkError(err, "Enqueueing kernel");

		// Wait for the commands to complete
		err = clFinish(commands);
		checkError(err, "Waiting for kernel to finish");

		// Read back the results from the compute device
		err = clEnqueueReadBuffer(commands, d_a, CL_TRUE, 0,
				sizeof(long) * x_mon, h_image_row, 0, NULL, NULL);
		if (err != CL_SUCCESS) {
			printf("Error: Failed to read output array!\n%s\n", err_code(err));
			exit(1);
		}

		//reduce y
		y_value -= delta_y;

		//cope row to image
		memcpy(h_image + row * x_mon, h_image_row, sizeof(long) * x_mon);

		free(h_image_row);
	}

	for (i = 0; i < x_mon * y_mon; ++i) {
		printf("%ld ", h_image[i]);
	}
	fflush(stdout);

	//###############################################
	//
	// cleanup then shutdown
	//
	//###############################################

	clReleaseMemObject(d_a);
	clReleaseProgram(program);
	clReleaseKernel(ko_calculate_imagerowdots_iterations);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);

	free(h_image);

	return 0;
}
