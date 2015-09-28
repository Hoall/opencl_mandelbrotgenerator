/*
 * zoom.h
 *
 *      Author: Felix Paetow
 */

#ifndef SRC_ZOOM_H_
#define SRC_ZOOM_H_

#include <math.h>
#include <stdio.h>
#include "my_complex.h"

my_complex_t find_dot_to_zoom(const float x_min, const float x_max,
		const float y_min, const float y_max, long * image, const long heigth,
		const long width, const long itr);
float calculate_distance_abs(const float a, const float b);
void reduce_section_focus_dot(float * const min, float * const max,
		const float dot, float reduction_value);
void reduce_plane_section_focus_dot(float * const x_min, float * const x_max,
		float * const y_min, float * const y_max,
		float reduction_in_percentage, my_complex_t dot);

#endif /* SRC_ZOOM_H_ */
