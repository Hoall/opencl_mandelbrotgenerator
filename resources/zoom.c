/*
 * zoom.c
 *
 *      Author: Felix Paetow
 */

#include "zoom.h"

/**
 * Finds the point the zoom shall focus on.
 *
 * The function takes the midlle of the image and searches for the first point
 * in the Mandelbrot set. That's the zoom point.
 *
 * @param x_min Smallest X-value of the plane section.
 * @param x_max Greatest X-value of the plane section.
 * @param y_min Smallest Y-value of the plane section.
 * @param y_max Greatest Y-value of the plane section.
 * @param imagevalues The image as a set of iteration values.
 * @param width The width.
 * @param height The height.
 * @param itr The number of required iterations.
 * @return The point to zoom to.
 */
my_complex_t find_dot_to_zoom(const float x_min, const float x_max,
		const float y_min, const float y_max, long * image, const long heigth,
		const long width, const long itr) {

	//find zoom point based on iteration values
	long middle = heigth / 2;
	long * middle_of_image = image + middle * width;

	short found_dot = 1;
	long i = width;
	while (found_dot != 0) {
		if (*(middle_of_image + i) == itr) {
			found_dot = 0;
		} else {
			--i;
		}
	}

	//calculates the value of the zoom point
	float delta_x = delta(x_min, x_max, width);
	float delta_y = delta(y_min, y_max, heigth);

	my_complex_t c;
	c.real = x_min + i * delta_x;
	c.imaginary = y_max - (float) middle * delta_y;

	return c;
}

/**
 * Calculates the absolute value between two points on a line.
 *
 * @param a First value.
 * @param b Second value.
 * @return The absolute distance.
 */
float calculate_distance_abs(const float a, const float b) {
	float result = -1;

	if (a < b) {
		result = b - a;
		result = fabs(result);
	} else {
		result = a - b;
		result = fabs(result);
	}

	return result;
}

/**
 * Reduce the line about the reduction value in a way, that the dot is moving
 * to the middle of the line.
 *
 *
 * @param min Smallest value of the line.
 * @param max Greatest value of the line.
 * @param dot The dot on the line.
 * @param reduction_value The value by which the line is to be reduced.
 */
void reduce_section_focus_dot(float * const min, float * const max,
		const float dot, float reduction_value) {
	//If the value is on the line
	if (dot > *min && dot < *max) {
		float dot_distance_to_min = calculate_distance_abs(*min, dot);
		float dot_distance_to_max = calculate_distance_abs(*max, dot);
		float difference_dot_distances = calculate_distance_abs(
				dot_distance_to_min, dot_distance_to_max);

		//If the line is greater then the reduction value, otherwise wrong
		//values
		if (reduction_value < (dot_distance_to_min + dot_distance_to_max)) {
			/* If the reduction value is less equal to the difference of the
			 * distances then the total reduction value can be deducted.
			 */
			if (reduction_value <= difference_dot_distances) {
				if (dot_distance_to_min < dot_distance_to_max) {
					*max -= reduction_value;

				} else if (dot_distance_to_max < dot_distance_to_min) {
					*min += reduction_value;
				}
				/* if the reduction is greater than the difference of the
				 * distances then the side of the greater distance and the
				 * reduction value will be reduced by the difference and the
				 * distance again by half of the remaining reduction value.
				 * The other side is then reduced by the other half of the
				 * remaining reduction value.
				 */
			} else {
				if (dot_distance_to_min == dot_distance_to_max) {
					*max -= (reduction_value / 2);
					*min += (reduction_value / 2);
				} else if (dot_distance_to_min < dot_distance_to_max) {
					reduction_value -= difference_dot_distances;
					*max -= ((reduction_value / 2) + difference_dot_distances);
					*min += (reduction_value / 2);
				} else if (dot_distance_to_max < dot_distance_to_min) {
					reduction_value -= difference_dot_distances;
					*min += ((reduction_value / 2) + difference_dot_distances);
					*max -= (reduction_value / 2);
				}
			}
		}

	}
}

void reduce_plane_section_focus_dot(float * const x_min, float * const x_max,
		float * const y_min, float * const y_max,
		float reduction_in_percentage, my_complex_t dot) {

	float reduction_value_x = fabs(
			(*x_max - *x_min) * reduction_in_percentage / 100);
	float reduction_value_y = fabs(
			(*y_max - *y_min) * reduction_in_percentage / 100);

	reduce_section_focus_dot(x_min, x_max, dot.real, reduction_value_x);
	reduce_section_focus_dot(y_min, y_max, dot.imaginary, reduction_value_y);
}
