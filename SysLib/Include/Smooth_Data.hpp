//*********************************************************
//	Smooth_Data.hpp - data smoothing class
//*********************************************************

#ifndef SMOOTH_DATA_HPP
#define SMOOTH_DATA_HPP

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Smooth_Data Class definition
//---------------------------------------------------------

class Smooth_Data : public Static_Service
{
public:
	Smooth_Data (int num_in = 0, int iterations = 3, bool loop_flag = false, int group_size = 3, 
		double forward = 20.0, double backward = 20.0, int num_sub = 1, bool dup_flag = false);

	virtual ~Smooth_Data (void)                   { Clear (); };

	bool Setup (int num_in, int iterations = 3, bool loop_flag = false,	int group_size = 3, 
		double forward = 20.0, double backward = 20.0, int num_sub = 1, bool dup_flag = false);

	bool Interpolate (int num_in, double increment, int iterations = 3, 
		int group_size = 3, double forward = 20.0, double backward = 20.0, bool dup_flag = true);

	bool Num_Input (int value);
	void Input (int num, double value)            { if (Check_In (num)) input [num-1] = value; };
	double Input (int num)                        { return ((Check_In (num)) ? input [num-1] : 0); }

	int  Num_Input (void)                         { return (num_in); }
	int  Num_Output (void)                        { return (num_out); }

	void Output (int num, double value)           { if (Check_Out (num)) output [num-1] = value; };
	double Output (int num)                       { return ((Check_Out (num)) ? output [num-1] : 0.0); }

	int Smooth (int num = 0);

protected:
	bool Check_In (int num)                       { return (num > 0 && num <= num_in); }
	bool Check_Out (int num)                      { return (num > 0 && num <= num_out); }

	void Clear (void);

private:
	int num_in, num_out, niter, size, num_sub;
	double increment, forward, backward, weight, *input, *output, *current;	
	bool loop_flag, dup_flag, interpolate;
};
#endif
