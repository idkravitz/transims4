//*********************************************************
//	Smooth_Data.cpp - data smoothing class
//*********************************************************

#include "Smooth_Data.hpp"

//---------------------------------------------------------
//	Smooth_Data constructor
//---------------------------------------------------------

Smooth_Data::Smooth_Data (int num_in, int iterations, bool loop_flag, int group_size, 
		  double forward, double backward, int num_sub, bool dup_flag) : Static_Service ()
{
	input = output = current = NULL;

	Setup (num_in, iterations, loop_flag, group_size, forward, backward, num_sub, dup_flag);
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Smooth_Data::Clear (void)
{
	if (input != NULL) {
		delete input;
		input = NULL;
	}
	if (output != NULL) {
		delete output;
		output = NULL;
	}
	if (current != NULL) {
		delete current;
		current = NULL;
	}
	num_in = num_out = 0;
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

bool Smooth_Data::Setup (int in, int iter, bool flag, int group, double front, double back, int sub, bool dup)
{
	niter = iter;
	loop_flag = flag;
	size = group;
	forward = front;
	backward = back;
	num_sub = sub;
	dup_flag = dup;
	interpolate = false;

	return (Num_Input (in));
}

//---------------------------------------------------------
//	Interpolate
//---------------------------------------------------------

bool Smooth_Data::Interpolate (int in, double inc, int iter, int group, double front, double back, bool dup)
{
	niter = iter;
	loop_flag = false;
	size = group;
	forward = front;
	backward = back;
	num_sub = 1;
	dup_flag = dup;
	interpolate = true;
	increment = inc;

	return (Num_Input (in));
}

//---------------------------------------------------------
//	Num_Input
//---------------------------------------------------------

bool Smooth_Data::Num_Input (int num)
{
	int max_iter, max_size;
	double factor;
	bool stat = true;

	Clear ();

	//---- number of input values ----

	num_in = num;

	if (num_in < 3) {
		if (num_in != 0) {
			exe->Warning ("Number of Input Values %d is Out of Range (>= 3)", num_in);
			stat = false;
		}
		num_in = 24;
	}
	input = new double [num_in];

	if (input == NULL) {
		num_in = 0;
		return (false);
	}

	//---- number of output values ----

	if (num_sub < 1) {
		exe->Warning ("Number of Subdivisions %d is Out of Range (>= 1)", num_sub);
		num_sub = 1;
		stat = false;
	}
	if (interpolate) {
		if (increment < 0.1) {
			exe->Warning ("Smoothing Increment %.2lf is Out of Range (>= 0.1)", increment);
			increment = 1.0;
		}
		num_out = (int) ((num_in - 1) * increment + 0.9) + 1;
	} else {
		num_out = num_in * num_sub;
	}

	max_iter = ((num_in + 2) / 3) * num_out / num_in;
	max_size = ((num_in + 18) / 20) * 2 + 1;

	output = new double [num_out];
	current = new double [num_out];

	if (output == NULL || current == NULL) {
		num_in = num_out = 0;
		return (false);
	}

	//---- number of iterations ----

	if (niter < 1 || niter > max_iter) {
		exe->Warning ("Number of Iterations %d is Out of Range (1-%d)", niter, max_iter);
		if (niter < 1) {
			niter = 1;
		} else {
			niter = max_iter;
		}
		stat = false;
	}

	//---- group size ----

	if (size < 3 || size > max_size) {
		exe->Warning ("Smooth Group Size %d is Out of Range (3..%d)", size, max_size);
		if (size < 3) {
			size = 3;
		} else if (size > max_size) {
			size = max_size;
		}
		stat = false;
	}
	if (size % 2 != 1) {
		exe->Warning ("Smooth Group Size %d must be an Odd Number", size);
		size += 1;
		stat = false;
	}

	//---- percent forward ----

	factor = 100.0 - (50.0 / size);

	if (forward < 0.0 || forward > factor) {
		exe->Warning ("Forward Percentage %.1lf is Out of Range (0..%.1lf)", forward, factor);
		if (forward < 0.0) {
			forward = 20.0;
		} else if (forward > factor) {
			forward = factor;
		}
		stat = false;
	}

	//---- read the percent distributed backward ----

	if (backward < 0.0 || backward > factor) {
		exe->Warning ("Backward Percentage %.1lf is Out of Range (0..%.1lf)", backward, factor);
		if (backward < 0.0) {
			backward = 20.0;
		} else if (backward > factor) {
			backward = factor;
		}
		stat = false;
	}	
	weight = forward + backward;
	factor = 100.0 - (100.0 / size);

	if (weight < 5.0 || weight > factor) {
		exe->Warning ("Combined Percentage %.1lf is Out of Range (5..%.0lf)", weight, factor);
		if (weight < 5.0) {
			forward += 2.5;
			backward += 2.5;
		} else if (weight > factor) {
			factor /= weight;
			forward *= factor;
			backward *= factor;
		}
		stat = false;
	}

	//---- adjust the factors ----

	weight = 0.0;
	max_size = size / 2;

	for (max_iter=1; max_iter <= max_size; max_iter++) {
		weight += max_iter;
	}
	weight = 1.0 / weight;

	return (stat);
}

//---------------------------------------------------------
//	Smooth
//---------------------------------------------------------

int Smooth_Data::Smooth (int num_rec)
{
	int i, j, n, mem_size, i1, i2, num, nout;
	double share, forward_total, backward_total, delta, cum;

	nout = num_out;

	if (num_rec <= 0 || num_rec > num_in) {
		num_rec = num_in;
	} else if (!interpolate) {
		if (num_sub > 1) {
			nout = num_rec * num_sub;
		} else {
			nout = num_rec;
		}
	}

	//---- initialize the output data ----

	mem_size = nout * sizeof (double);

	if (interpolate) {
		delta = 1.0 / increment;

		backward_total = 0;
		forward_total = input [0];
		memset (output, '\0', mem_size);

		for (i=n=0, cum=0; n < nout; n++, cum += delta) {
			if (cum >= i) {
				backward_total = forward_total;

				if (++i > num_rec) break;
				if (i == num_rec) {
					forward_total = 0;
				} else {
					forward_total = input [i];
				}
			}
			output [n] = (cum - i + 1) * (forward_total - backward_total) + backward_total;
		}
	} else if (num_sub > 1) {
		for (i=n=0; i < num_rec; i++) {
			if (dup_flag) {
				share = input [i];
			} else {
				share = input [i] / num_sub;
			}
			for (j=0; j < num_sub; j++) {
				output [n++] = share;
			}
		}
	} else {
		memcpy (output, input, mem_size);
	}
	num = size / 2;

	//---- process each iteration ----

	for (n=0; n < niter; n++) {

		memcpy (current, output, mem_size);
		memset (output, '\0', mem_size);

		for (i=0; i < nout; i++) {
			share = current [i];

			forward_total = share * forward / 100.0;
			backward_total = share * backward / 100.0;

			output [i] += share - forward_total - backward_total;

			i1 = i2 = i;
			forward_total *= weight;
			backward_total *= weight;

			for (j=num; j > 0; j--) {
				i1--;
				i2++;

				if (i1 < 0) {
					i1 = (loop_flag) ? nout - 1 : 0;
				}
				if (i2 >= nout) {
					i2 = (loop_flag) ? 0 : nout - 1;
				}
				output [i1] += backward_total * j;
				output [i2] += forward_total * j;
			}
		}
	}
	if (interpolate && dup_flag) {
		forward_total = backward_total = 0.0;

		for (i=0; i < num_rec; i++) {
			backward_total += input [i];
		}
		for (j=0; j < nout; j++) {
			forward_total += output [j];
		}
		if (forward_total > 0.0) {
			share = backward_total / forward_total;

			for (j=0; j < nout; j++) {
				output [j] *= share;
			}
		}
	}
	return (nout);
}
