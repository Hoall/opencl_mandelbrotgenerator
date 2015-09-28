/*
 * mybmpwriter.c
 *
 *  Created on: Sep 17, 2015
 *      Author: fpaetow
 */

#include "mybmpwriter.h"

/**
 * Calculates the size of the bmp file.
 *
 * @param width The width.
 * @param height The height.
 * @return The file size in bytes.
 */
long calculate_filesize(const long width, const long height) {
	long value = 54 + 3 * width * height; //54 is the header of the file

	return value;
}

/**
 * Set a default header and writes the file size in it. The given array must
 * have a size of 14.
 *
 * @param bmpfileheader A 14-byte array.
 * @param filesize The final size of the bmp file.
 */
void calcute_bmpfileheader(unsigned char * mbmpfileheader, const long filesize) {
	unsigned char standardbmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0,
			0, 54, 0, 0, 0 };

	for (int i = 0; i < 14; ++i) {
		mbmpfileheader[i] = standardbmpfileheader[i];
	}

	mbmpfileheader[2] = (unsigned char) (filesize);
	mbmpfileheader[3] = (unsigned char) (filesize >> 8);
	mbmpfileheader[4] = (unsigned char) (filesize >> 16);
	mbmpfileheader[5] = (unsigned char) (filesize >> 24);
}

/**
 * Set default info header and writes the width and height of the image in it.
 * The given array must have the size of 40 bytes.
 *
 * @param bmpinfoheader A 40-byte array.
 * @param width The width.
 * @param height The height.
 */
void calculate_bmpinfoheader(unsigned char * mbmpinfoheader, const long width,
		const long height) {
	unsigned char standardbmpinfoheader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 0, 24, 0 };

	for (int i = 0; i < 40; ++i) {
		mbmpinfoheader[i] = standardbmpinfoheader[i];
	}

	mbmpinfoheader[4] = (unsigned char) (width);
	mbmpinfoheader[5] = (unsigned char) (width >> 8);
	mbmpinfoheader[6] = (unsigned char) (width >> 16);
	mbmpinfoheader[7] = (unsigned char) (width >> 24);
	mbmpinfoheader[8] = (unsigned char) (height);
	mbmpinfoheader[9] = (unsigned char) (height >> 8);
	mbmpinfoheader[10] = (unsigned char) (height >> 16);
	mbmpinfoheader[11] = (unsigned char) (height >> 24);
}

/**
 * Save a image in a bmp file.
 *
 * @param x_mon Resolution of the monitor on the horizontal axis.
 * @param y_mon Resolution of the monitor on the vertical axis.
 * @param image The image values.
 * @param name The name for the bmp file.
 */
void safe_image_to_bmp(const long x_mon, const long y_mon,
		unsigned char * image, char * name) {
	FILE *f;
	unsigned char bmpfileheader[14];
	unsigned char bmpinfoheader[40];
	unsigned char bmppad[3] = { 0, 0, 0 };
	long filesize;

	filesize = calculate_filesize(y_mon, x_mon);

	//set header
	calcute_bmpfileheader(bmpfileheader, filesize);
	calculate_bmpinfoheader(bmpinfoheader, x_mon, y_mon);

	f = fopen(name, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	//Passing through the lines
	for (long i = 0; i < y_mon; ++i) {
		fwrite(image + ((i * x_mon * 3)), 3, x_mon, f);
		fwrite(bmppad, 1, (4 - (x_mon * 3) % 4) % 4, f);
	}

	fclose(f);
}
