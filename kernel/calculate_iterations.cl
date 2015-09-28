//###############################################
//
// my_complex functions
//
//###############################################

typedef struct komplex {
	float real;
	float imaginary;
} my_complex_t;

float delta(const float min, const float max, const long size);
my_complex_t add_complex(my_complex_t a, my_complex_t b);
my_complex_t sub_complex(my_complex_t a, my_complex_t b);
my_complex_t mul_complex(my_complex_t a, my_complex_t b);
float sum_complex(my_complex_t a);

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

	result = sqrt(pow((float)a.real, 2) + pow((float)a.imaginary, 2));

	return result;
}

////###############################################
//
// mandelbrotsimplethread functions
//
//###############################################

my_complex_t calculate_dot(const my_complex_t z, const my_complex_t c);
long iterate_dot(const my_complex_t c, const float abort_value,
		const long itr);
__kernel void calculate_imagerowdots_iterations(const float x_min, const float x_max,
		const float y_value, const long x_mon, const float abort_value, const long itr,
		__global long * imagerow);
__kernel void calculate_colorrow(const long width, long itr, __global long * imagerowvalues,
		__global unsigned char * imagerow);

/**
 *  Calculate z(n+1) = z(n)^2 - c
 * @param z The complex number z.
 * @param c The complex number c.
 * @return The complex number z(n+1)
 */
my_complex_t calculate_dot(const my_complex_t z, const my_complex_t c) {
	my_complex_t result, interim_result;
	result.real = -1;
	result.imaginary = -1;

	interim_result = mul_complex(z, z);
	result = sub_complex(interim_result, c);

	return result;
}

/**
 * Calculates and validates whether a point belongs to the Mandelbrot set or
 * not.
 *
 * The formula is as long as calculated until either the absolute value exceeds
 * the abort condition or the number of iterations has been performed.
 *
 * If the abort condition was fullfilled, the point does not belong to the
 * Mandelbrot set. If not, then most likely.
 *
 * @param c The test point.
 * @param abort_value The value of the abort condition. Normally 2.
 * @param itr The number of required iterations.
 * @return The number of iterations for that point.
 */
long iterate_dot(const my_complex_t c, const float abort_value,
		const long itr) {
	float sum = -1;

	//define z
	my_complex_t z;
	z.real = 0;
	z.imaginary = 0;

	long i = 0;
	while (i < itr && sum < abort_value) {
		my_complex_t z_neu;

		z_neu = calculate_dot(z, c);	//calculate z(n+1) = z(n) - c
		sum = sum_complex(z_neu);		//calculate |z|
		z = z_neu;

		if (sum < abort_value) {
			++i;
		}
	}

	return i;
}

/**
 * Calculates a Mandelbrot imagerow without colors.
 *
 * For each point of the imagerow it calculates the points and saves the number of
 * iterations for it. From the number of iterations you can say i a point
 * belongs to a Mandelbrot set or not.
 *
 * Based on the number of iterations the color is choosen later.

 *
 * @param x_min Smallest X-value of the plane section.
 * @param x_max Greatest X-value of the plane section.
 * @param y_min Smallest Y-value of the plane section.
 * @param y_max Greatest Y-value of the plane section.
 * @param x_mon Resolution of the monitor on the horizontal axis.
 * @param y_mon Resolution of the monitor on the vertical axis.
 * @param abort_value The value of the abort condition. Normally 2.
 * @param itr The number of required iterations.
 * @param image The image as a set of iteration values.
 */
__kernel void calculate_imagerowdots_iterations(const float x_min, const float x_max,
		const float y_value, const long x_mon, const float abort_value, const long itr,
		__global long * imagerow) {
	float delta_x = delta(x_min, x_max, x_mon);
	int j = get_global_id(0);	//the position in the row

	//set to top left corner
	my_complex_t c;
	c.real = x_min + j * delta_x;
	c.imaginary = y_value;

	//for each dot in the column
	j = get_global_id(0);
	imagerow[j] = iterate_dot(c, abort_value, itr);

}
/**
 * Calculates the color from the iteration values.
 *
 * @param imagevalues The calculated iteration values.
 * @param image The final imagerow.
 * @param width The width.
 */
__kernel void calculate_colorrow(const long width, long itr, __global long * imagerowvalues,
		__global unsigned char * imagerow) {

	float color_steps = (float) 255.0 / (float) itr;
	float red = color_steps;
	float green = color_steps;
	float blue = color_steps;

	int i = get_global_id(0);
	long value = imagerowvalues[i];
	value = itr - value;

	float myred = (float) value * red / 1.1;
	float mygreen = (float) value * green / 1.05;
	float myblue = (float) value * blue;

	if (myred > 255.0) {
		myred = 255;
	}
	if (mygreen > 255.0) {
		mygreen = 255;
	}
	if (myblue > 255.0) {
		myblue = 255;
	}

	imagerow[i * 3 - 3] = (unsigned char) myred;
	imagerow[i * 3 - 2] = (unsigned char) mygreen;
	imagerow[i * 3 - 1] = (unsigned char) myblue;
}

