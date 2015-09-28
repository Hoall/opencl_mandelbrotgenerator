/*
 * my_complex.c
 *
 *      Author: Felix Paetow
 */

#include "my_complex.h"

/**
 * Calculates the distance (delta) for a certain number of uniformly
 * distributed points, spread over a line.
 *
 * @param min Smallest value of the line.
 * @param max Greatest value of the line.
 * @param size Number of Points.
 * @return Distance between the points on the line
 */
float delta(const float min, const float max, const long size) {
	float result = -1;

	result = (max - min) / (float) (size - 1);

	return result;
}

/**
 * Add two complex numbers.
 *
 * @param a First complex number.
 * @param b Second complex number.
 * @return The result of the addition.
 */
my_complex_t add_complex(const my_complex_t a, const my_complex_t b) {
	my_complex_t result;
	result.real = -1;
	result.imaginary = -1;

	result.real = a.real + b.real;
	result.imaginary = a.imaginary + b.imaginary;

	return result;
}

/**
 * Subtract two complex numbers.
 *
 * @param a First complex number.
 * @param b Second complex number.
 * @return The result of the subtraction.
 */
my_complex_t sub_complex(const my_complex_t a, const my_complex_t b) {
	my_complex_t result;
	result.real = -1;
	result.imaginary = -1;

	result.real = a.real - b.real;
	result.imaginary = a.imaginary - b.imaginary;

	return result;
}

/**
 * Multiply two complex numbers.
 *
 * @param a First complex number.
 * @param b Second complex number.
 * @return The result of the multiplication.
 */
my_complex_t mul_complex(const my_complex_t a, const my_complex_t b) {
	my_complex_t result;
	result.real = -1;
	result.imaginary = -1;

	result.real = (a.real * b.real) - (a.imaginary * b.imaginary);
	result.imaginary = (a.real * b.imaginary) + (b.real * a.imaginary);

	return result;
}

/**
 * Calculates the absolute value of a complex number.
 *
 * @param a Number whose amount is to be determined.
 * @return Absolute value of the complex number.
 */
float sum_complex(const my_complex_t a) {
	float result = -1;

	result = sqrt(pow(a.real, 2) + pow(a.imaginary, 2));

	return result;
}
