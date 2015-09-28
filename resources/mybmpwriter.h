/*
 * mybmpwriter.h
 *
 *  Created on: Sep 18, 2015
 *      Author: fpaetow
 */

#ifndef MYBMPWRITER_H_
#define MYBMPWRITER_H_

#include <stdio.h>
#include <stdlib.h>

long calculate_filesize(const long width, const long height);
void calcute_bmpfileheader(unsigned char * bmpfileheader, const long filesize);
void calculate_bmpinfoheader(unsigned char * bmpinfoheader, const long width,
		const long height);
void safe_image_to_bmp(const long x_mon, const long y_mon,
		unsigned char * image, char * name);

#endif /* MYBMPWRITER_H_ */
