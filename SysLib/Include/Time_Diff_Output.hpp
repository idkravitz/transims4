//*********************************************************
//	Time_Diff_Output.hpp - Time Difference Distribution
//*********************************************************

#ifndef TIME_DIFF_OUTPUT_HPP
#define TIME_DIFF_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Db_File.hpp"

#define NUM_DISTRIBUTION	151
#define MIN_DIFFERENCE		-60

//---------------------------------------------------------
//	Time_Diff_Output Class definition
//---------------------------------------------------------

class Time_Diff_Output : public Static_Service
{
public:
	Time_Diff_Output (void);
	virtual ~Time_Diff_Output (void);

	void Clear (void);

	void Report_Numbers (int total_distrib, int period_distrib, int period_sum);
	bool Time_Periods (Time_Range *periods);
	bool Open_Distribution (char *filename);
	bool Open_Mode_Distribution (char *filename, int modes = 5, bool diff_flag = true);
	void Generalized_Cost (bool flag); 

	void Add_Trip (int tod, int current_time, int base_time);
	void Add_Trip_Mode (int modes, int *current_modes, int *base_modes);

	void Distribution_Range (int period, double percent, double *low, double *high);
	void Write_Distribution (void);
	void Write_Mode_Distribution (int modes, char **label);

	void Time_Distribution (int type);
	void Report_Header (int type);
	void Period_Summary (void);
	void Total_Summary (void);

private:
	int num_periods, period, total_distrib, period_distrib, period_sum; 
	int num_distribution, min_difference, num_modes;
	double scaling_factor, units_factor;
	bool output_flag, mode_flag, cost_flag, diff_flag;

	typedef struct {
		double current;
		double base;
		double abs_diff;
		int num_diff;
		int max_diff;
		int min_diff;
	} Period_Sum;

	Period_Sum *sum_data;

	int **time_distribution;
	double **mode_distribution;

	Time_Range *time_periods;
	Db_File distrib_file, mode_file;
};

#endif
