//*********************************************************
//	Time_Diff_Output.cpp - Time Difference Distribution
//*********************************************************

#include "Time_Diff_Output.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Time_Diff_Output constructor
//---------------------------------------------------------

Time_Diff_Output::Time_Diff_Output (void) : Static_Service ()
{
	num_periods = total_distrib = period_distrib = period_sum = 0; 
	num_distribution = NUM_DISTRIBUTION;
	min_difference = MIN_DIFFERENCE;
	output_flag = mode_flag = cost_flag = diff_flag = false;
	units_factor = 60.0;
	sum_data = NULL;
	time_periods = NULL;
	time_distribution = NULL;
	mode_distribution = NULL;
	num_modes = 5;
}

//---------------------------------------------------------
//	Time_Diff_Output destructor
//---------------------------------------------------------

Time_Diff_Output::~Time_Diff_Output (void)
{
	Clear ();
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Time_Diff_Output::Clear (void)
{
	int num;

	if (num_periods > 0) {
		if (sum_data != NULL) {
			delete [] sum_data;
			sum_data = NULL;
		}
		if (time_distribution != NULL) {
			if (period_distrib > 0 || period_sum > 0 || output_flag) {
				num = num_periods;
			} else {
				num = 1;
			}
			for (int i=0; i < num; i++) {
				if (time_distribution [i] != NULL) {
					delete [] time_distribution [i];
					time_distribution [i] = NULL;
				}
			}
			delete [] time_distribution;
			time_distribution = NULL;
		}
		if (mode_distribution != NULL) {
			num = num_modes;
			if (!diff_flag) num = num * 2 - 1;
			for (int i=0; i < num; i++) {
				if (mode_distribution [i] != NULL) {
					delete [] mode_distribution [i];
					mode_distribution [i] = NULL;
				}
			}
			delete [] mode_distribution;
			mode_distribution = NULL;
		}
		num_periods = 0;
	}
}

//---------------------------------------------------------
//	Report_Numbers -- set report numbers
//---------------------------------------------------------

void Time_Diff_Output::Report_Numbers (int total, int distrib, int summary)
{
	total_distrib = total;
	period_distrib = distrib;
	period_sum = summary;

	if (num_periods > 0) {
		Time_Periods (time_periods);
	}
}

//---------------------------------------------------------
//	Time_Periods -- set report numbers
//---------------------------------------------------------

bool Time_Diff_Output::Time_Periods (Time_Range *periods)
{
	if (num_periods > 0) {
		Clear ();
	}
	time_periods = periods;

	if (time_periods == NULL) {
		num_periods = 1;
	} else {
		num_periods = time_periods->Num_Ranges () + 1;
	}

	//---- allocate space for change data ----

	int num;

	if (period_distrib > 0 || period_sum > 0 || output_flag) {
		num = num_periods;
	} else {
		num = 1;
	}
	time_distribution = new int * [num];
	if (time_distribution == NULL) return (false);
	memset (time_distribution, '\0', num * sizeof (int *));
	
	for (int i=0; i < num; i++) {
		time_distribution [i] = new int [num_distribution];
		if (time_distribution [i] == NULL) return (false);
		memset (time_distribution [i], '\0', num_distribution * sizeof (int));
	}
	sum_data = new Period_Sum [num];
	if (sum_data == NULL) return (false);
	memset (sum_data, '\0', num * sizeof (Period_Sum));
	return (true);
}

//---------------------------------------------------------
//	Open_Distribution -- open the time distribution file
//---------------------------------------------------------

bool Time_Diff_Output::Open_Distribution (char *filename)
{
	if (filename != NULL) {
		output_flag = true;

		exe->Print (1);
		distrib_file.File_Type ("New Distribution File");
		distrib_file.File_Access (Db_Code::CREATE);

		if (!distrib_file.Open (exe->Project_Filename (filename))) {
			exe->File_Error ("Creating New Distribution File", distrib_file.Filename ());
			return (false);
		}
		if (num_periods > 0) {
			return (Time_Periods (time_periods));
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Open_Mode_Distribution -- open the mode distribution file
//---------------------------------------------------------

bool Time_Diff_Output::Open_Mode_Distribution (char *filename, int modes, bool flag)
{
	if (filename != NULL) {
		mode_flag = true;

		exe->Print (1);
		mode_file.File_Type ("New Mode Distribution File");
		mode_file.File_Access (Db_Code::CREATE);

		if (!mode_file.Open (exe->Project_Filename (filename))) {
			exe->File_Error ("Creating New Distribution File", mode_file.Filename ());
			return (false);
		}
		int num = num_modes = modes;
		diff_flag = flag;
		if (!diff_flag) num = num * 2 - 1;

		mode_distribution = new double * [num];
		if (mode_distribution == NULL) return (false);
		memset (mode_distribution, '\0', num * sizeof (double *));
		
		for (int i=0; i < num; i++) {
			mode_distribution [i] = new double [num_distribution];
			if (mode_distribution [i] == NULL) return (false);
			memset (mode_distribution [i], '\0', num_distribution * sizeof (double));
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Generalized_Cost -- set the cost flag
//---------------------------------------------------------

void Time_Diff_Output::Generalized_Cost (bool flag)
{
	cost_flag = flag;
	units_factor = (cost_flag) ? 100.0 : 60.0;
	num_distribution = NUM_DISTRIBUTION * 6;
	min_difference = MIN_DIFFERENCE * 6;

	if (num_periods > 0) {
		Time_Periods (time_periods);
	}
	if (mode_flag) {
		int num = num_modes;
		if (!diff_flag) num = num * 2 - 1;

		for (int i=0; i < num; i++) {
			delete [] mode_distribution [i];
			mode_distribution [i] = new double [num_distribution];
			if (mode_distribution [i] == NULL) return;
			memset (mode_distribution [i], '\0', num_distribution * sizeof (double));
		}
	}
}

//---------------------------------------------------------
//	Add_Trip -- add data to the summary periods
//---------------------------------------------------------

void Time_Diff_Output::Add_Trip (int tod, int current_time, int base_time)
{
	if (num_periods == 0 || current_time < 0 || base_time < 0) return;

	if (time_periods != NULL) {
		tod = time_periods->In_Index (tod);
		if (tod <= 0 || tod >= num_periods) return;
	} else {
		tod = 0;
	}
	int diff = current_time - base_time;

	sum_data->num_diff++;
	sum_data->current += current_time;
	sum_data->base += base_time;
	sum_data->abs_diff += abs (diff);
	if (diff > sum_data->max_diff || sum_data->num_diff == 1) sum_data->max_diff = diff;
	if (diff < sum_data->min_diff || sum_data->num_diff == 1) sum_data->min_diff = diff;

	//---- period summary ----

	if (period_sum > 0 && tod > 0) {
		Period_Sum *sum_ptr = sum_data + tod;

		sum_ptr->num_diff++;
		sum_ptr->current += current_time;
		sum_ptr->base += base_time;
		sum_ptr->abs_diff += abs (diff);
		if (diff > sum_ptr->max_diff || sum_data->num_diff == 1) sum_ptr->max_diff = diff;
		if (diff < sum_ptr->min_diff || sum_data->num_diff == 1) sum_ptr->min_diff = diff;
	}

	//---- difference distribution ----

	if (diff < 0) {
		diff = -(int)(-diff / units_factor) - 1;
	} else if (diff > 0) {
		diff = (int)(diff / units_factor) + 1;
	}
	diff -= min_difference;

	if (diff < 0) diff = 0;
	if (diff >= num_distribution) diff = num_distribution - 1;

	time_distribution [0] [diff]++;

	if ((period_distrib > 0 || period_sum > 0) && tod > 0) {
		time_distribution [tod] [diff]++;
	}
}

//---------------------------------------------------------
//	Add_Trip_Mode -- add data to the summary periods
//---------------------------------------------------------

void Time_Diff_Output::Add_Trip_Mode (int modes, int *current_modes, int *base_modes)
{
	if (num_modes == 0 || modes == 0 || current_modes == 0 || base_modes == 0) return;

	int diff = current_modes [0] - base_modes [0];

	//---- difference distribution ----

	if (diff < 0) {
		diff = -(int)(-diff / units_factor) - 1;
	} else if (diff > 0) {
		diff = (int)(diff / units_factor) + 1;
	}
	diff -= min_difference;

	if (diff < 0) diff = 0;
	if (diff >= num_distribution) diff = num_distribution - 1;

	mode_distribution [0] [diff] += 1.0;

	if (diff_flag) {
		for (int i=1; i < modes; i++) {
			int change = current_modes [i] - base_modes [i];
			mode_distribution [i] [diff] += change;
		}
	} else {
		for (int i=1; i < modes; i++) {
			mode_distribution [i] [diff] += current_modes [i];
			mode_distribution [modes + i - 1] [diff] += base_modes [i];
		}
	}
}

//---------------------------------------------------------
//	Distribution_Range
//---------------------------------------------------------

void Time_Diff_Output::Distribution_Range (int period, double percent, double *low, double *high)
{
	int i, j, total, start, end, num, cum0, cum1, cum2;

	*low = *high = 0.0;
	if (period < 0 || period >= num_periods) return;

	//---- get the distribution total ----

	total = end = 0;
	start = num_distribution;

	for (i=0; i < num_distribution; i++) {
		num = time_distribution [period] [i];
		if (num > 0) {
			total += num;
			if (start > i) start = i;
			end = i;
		}
	}
	if (total == 0) return;

	//--- calculate the percent criteria ----

	cum0 = (int) (total * (100.0 - percent) / 200.0 + 0.5);

	//---- get the lower end of the range ----

	cum1 = 0;

	for (i=start; i <= end; i++) {
		num = time_distribution [period] [i];
		cum2 = cum1 + num;

		if (cum2 >= cum0) {
			j = i + min_difference;
			if (num == 0) num = 1;

			if (j < 0) {
				*low = j + (double) (cum0 - cum1) / num;
			} else if (j > 0) {
				*low = j - 1 + (double) (cum0 - cum1) / num;
			}
			break;
		}
		cum1 = cum2;
	}

	//---- get the higher end of the range ----

	cum1 = 0;

	for (i=end; i >= start; i--) {
		num = time_distribution [period] [i];
		cum2 = cum1 + num;

		if (cum2 >= cum0) {
			j = i + min_difference;
			if (num == 0) num = 1;

			if (j < 0) {
				*high = j + 1 - (double) (cum0 - cum1) / num;
			} else if (j > 0) {
				*high = j - (double) (cum0 - cum1) / num;
			}
			break;
		}
		cum1 = cum2;
	}
}

//---------------------------------------------------------
//	Time Distribution
//---------------------------------------------------------

void Time_Diff_Output::Time_Distribution (int type)
{
	int i, total, num, start, end, max, max_row, first, last, max_lines, low85, high85;
	double percent, low, high;
	char *flag;
	char *stars = "************************************************************";

	//---- set the period range ----

	if (type == total_distrib) {
		first = last = 0;
	} else if (type == period_distrib) {
		first = 1;
		last = num_periods - 1;
	} else {
		return;
	}

	//---- print each time period's distribution ----

	for (period = first; period <= last; period++) {
		total = end = max = max_row = 0;
		start = num_distribution;
		
		for (i=0; i < num_distribution; i++) {
			num = time_distribution [period] [i];
			if (num > 0) {
				total += num;
				if (start > i) start = i;
				if (end < i) end = i;
				if (num > max) {
					max = num;
				}
			}
		}
		if (total == 0) continue;

		num = 0;

		for (i=start; i <= end; i++) {
			num += time_distribution [period] [i];
			if (2 * num >= total) {
				max_row = i;
				break;
			}
		}
		scaling_factor = 100.0 * max / (60.0 * total);

		//---- find the 85th percentile rows ----

		Distribution_Range (period, 85.0, &low, &high);

		if (low < 0.0) {
			low85 = -((int) (-low + 0.5));
		} else {
			low85 = (int) (low + 0.5);
		}
		if (high < 0.0) {
			high85 = -((int) (-high + 0.5));
		} else {
			high85 = (int) (high + 0.5);
		}

		//---- limit output to one page ----

		max_lines = exe->Page_Lines () - 8;

		if (start < max_row - max_lines / 2) {
			start = max_row - max_lines / 2;
		}
		if (start + max_lines <= end) {
			end = start + max_lines - 1;
		}

		//---- print the report ----

		exe->Header_Number (type);
		exe->New_Page ();

		for (i=start; i <= end; i++) {
			num = i + min_difference;
			percent = 100.0 * time_distribution [period] [i] / total;
			max = (int) (percent / scaling_factor + 0.5);

			flag = "  ";
			if (num == low85) flag = ">=";
			if (num == high85) flag = "<=";
			if (i == max_row) flag = "==";

			exe->Print (1, "  %4d %2s %8d %6.1lf%2s%*.*s", num, flag, 
				time_distribution [period] [i], percent, 
				((num == 0) ? "--" : "  "), max, max, stars);
		}
		exe->Header_Number (0);
	}
}

//---------------------------------------------------------
//	Report Header
//---------------------------------------------------------

void Time_Diff_Output::Report_Header (int type)
{
	if (type == period_sum) {
		exe->Print (1, "Time Period Summary (%s)", ((cost_flag) ? "Dollars" : "Minutes"));
		exe->Print (2, "%16cNumber  --Travel %s--  ------- %s Difference -------  -- 85%% Range --", BLANK, 
			((cost_flag) ? "Cost" : "Time"), ((cost_flag) ? "Cost" : "Time"));
		exe->Print (1, "Time Period      Trips   Current  Base   Average Percent Absolute Percent   Low     High");
	} else {
		char buffer [40];

		if (type == total_distrib) {
			str_cpy (buffer, sizeof (buffer), "All Time Periods");
		} else if (type == period_distrib) {
			if (period == 0) return;
			str_cpy (buffer, sizeof (buffer), time_periods->Range_Format (period));
		} else {
			return;
		}
		exe->Print (1, "%s Difference Distribution for %s", ((cost_flag) ? "Cost" : "Time"), buffer);
		exe->Print (1);
		exe->Print (1, "Difference%16c----------------- Percent of Total Trips -------------------", BLANK);
		exe->Print (1, "(%s)   Trips Percent ", ((cost_flag) ? "dollars" : "minutes"));

		for (int i=1; i <= 12; i++) {
			exe->Print (0, "%5d", (int) (i * 5 * scaling_factor + 0.5));
		}
	}
	exe->Print (1);
}

/*********************************************|***********************************************

	Time Period Summary (Minutes) (Dollars)

	                Number  --Travel Time--  ------- Time Difference -------  -- 85% Range --
					Number  --Travel Cost--  ------- Cost Difference -------  -- 85% Range --
	Time Period     Trips   Current  Base   Average Percent Absolute Percent   Low     High

	dd:dd..dd:dd ddddddddd  ffff.ff ffff.ff  ffff.ff ffff.ff ffff.ff ffff.ff  ffff.ff ffff.ff
	
	Total        ddddddddd  ffff.ff ffff.ff  ffff.ff ffff.ff ffff.ff ffff.ff  ffff.ff ffff.ff


	Time Difference Distribution for %s
                            
	Difference                ----------------- Percent of Total Trips -------------------
	(minutes)   Trips Percent     5   10   15   20   25   35   45   50   55   60   65   70
	(dollars)

	   ddd <= ddddddd  fff.fssccccccccccccccccccccccccccccccccccc60ccccccccccccccccccccccc
      
**********************************************|***********************************************/ 

//---------------------------------------------------------
//	Period_Summary
//---------------------------------------------------------

void Time_Diff_Output::Period_Summary (void)
{
	int i, period;
	double low, high, factor, diff, base;
	char buffer [FIELD_BUFFER];

	Period_Sum *sum_ptr;

	//---- print the report ----

	exe->Header_Number (period_sum);

	if (!exe->Break_Check (num_periods + 6)) {
		exe->Print (1);
		Report_Header (period_sum);
	}

	for (i=0; i < num_periods; i++) {

		if (i == num_periods - 1) {
			period = 0;
			exe->Print (1);
			str_cpy (buffer, sizeof (buffer), "Total");
		} else {
			period = i + 1;
			str_cpy (buffer, sizeof (buffer), time_periods->Range_Format (period));
		}
		sum_ptr = sum_data + period;

		Distribution_Range (period, 85.0, &low, &high);

		base = sum_ptr->base;
		diff = sum_ptr->current - base;
		if (base <= 0.0) base = 1.0;

		if (sum_ptr->num_diff < 1) {
			factor = 1.0;
		} else {
			factor = 1.0 / (units_factor * sum_ptr->num_diff);
		}
		exe->Print (1, "%-12.12s %9d %8.2lf%8.2lf %8.2lf%8.2lf%8.2lf%8.2lf %8.2lf%8.2lf",
			buffer, sum_ptr->num_diff, sum_ptr->current * factor, sum_ptr->base * factor,
			diff * factor, (100.0 * diff / base), sum_ptr->abs_diff * factor, 
			(100.0 * sum_ptr->abs_diff / base), low, high);
	}
	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Total_Summary
//---------------------------------------------------------

void Time_Diff_Output::Total_Summary (void)
{
	if (num_periods == 0) return;

	double base, diff, factor, low, high, fac;
	char *units, *measure, *total;

	//---- print the comparison summary ----

	base = sum_data->base;
	diff = sum_data->current - base;
	if (base == 0.0) base = 1.0;

	if (cost_flag) {
		fac = 100.0;
		measure = "Cost";
		units = "dollars";
		total = "dollars";
	} else {
		fac = 3600.0;
		measure = "Time";
		units = "minutes";
		total = "hours";
	}
	if (sum_data->num_diff < 1) {
		factor = 1.0;
	} else {
		factor = 1.0 / (units_factor * sum_data->num_diff);
	}
	Distribution_Range (0, 85.0, &low, &high);

	exe->Break_Check (10);

	exe->Print (2, "Number of Trips Compared = %d", sum_data->num_diff);
	exe->Print (1, "Average Input Travel %s = %.2lf %s", measure, sum_data->current * factor, units);
	exe->Print (1, "Average Compare Travel %s = %.2lf %s", measure, sum_data->base * factor, units);
	exe->Print (1, "Average Travel %s Difference = %.2lf %s (%.2lf%%)", 
		measure, diff * factor, units, (100.0 * diff / base));
	exe->Print (1, "Average Absolute Difference = %.2lf %s (%.2lf%%)", 
		sum_data->abs_diff * factor, units, (100.0 * sum_data->abs_diff / base));
	exe->Print (1, "Minimum %s Difference = %.2lf %s", measure, sum_data->min_diff / units_factor, units);
	exe->Print (1, "Maximum %s Difference = %.2lf %s", measure, sum_data->max_diff / units_factor, units);
	exe->Print (1, "85th Percentile Range = %.2lf to %.2lf %s", low, high, units);
	exe->Print (1, "Total Absolute Difference = %g %s", sum_data->abs_diff / fac, total);
	exe->Print (1, "Total User Benefit = %g %s", diff / fac, total);
}

//---------------------------------------------------------
//	Write_Distribution
//---------------------------------------------------------

void Time_Diff_Output::Write_Distribution (void)
{
	if (!output_flag) return;

	int i, j, k, start, end, num_out;
	char buffer [40];

	FILE *file;

	exe->Show_Message ("Writing %s -- Record", distrib_file.File_Type ());
	exe->Set_Progress ();

	file = distrib_file.File ();

	//---- get the data range ----

	num_out = end = 0;
	start = num_distribution;
	
	for (i=0; i < num_distribution; i++) {
		k = time_distribution [0] [i];
		if (k > 0) {
			num_out += k;
			if (start > i) start = i;
			if (end < i) end = i;
		}
	}
	if (num_out == 0) return;

	//---- print the header ----

	fprintf (file, ((cost_flag) ? "DOLLARS" : "MINUTES"));

	if (num_periods > 1) {
		fprintf (file, "\tTOTAL");
	}
	for (i=1; i < num_periods; i++) {
		str_fmt (buffer, sizeof (buffer), "P%s", time_periods->Range_Label (i));
		fprintf (file, "\t%s", buffer);
	}
	fprintf (file, "\n");

	//---- print each record ----
	
	num_out = 0;

	for (j=start; j <= end; j++) {
		exe->Show_Progress ();

		fprintf (file, "%d", j + min_difference);

		if (num_periods > 1) {
			i = 0;
		} else {
			i = 1;
		}
		for (; i < num_periods; i++) {		
			fprintf (file, "\t%d", time_distribution [i] [j]);
		}
		fprintf (file, "\n");
		num_out++;
	}
	exe->End_Progress ();

	distrib_file.Close ();

	exe->Print (2, "Number of %s Records = %d", distrib_file.File_Type (), num_out);
}

//---------------------------------------------------------
//	Write_Mode_Distribution
//---------------------------------------------------------

void Time_Diff_Output::Write_Mode_Distribution (int modes, char **label)
{
	if (!mode_flag) return;

	int i, j, k, start, end, num_out, num;
	double value;

	FILE *file;

	exe->Show_Message ("Writing %s -- Record", mode_file.File_Type ());
	exe->Set_Progress ();

	file = mode_file.File ();

	//---- get the data range ----

	num_out = end = 0;
	start = num_distribution;
	
	for (i=0; i < num_distribution; i++) {
		k = (int) mode_distribution [0] [i];
		if (k > 0) {
			num_out += k;
			if (start > i) start = i;
			if (end < i) end = i;
		}
	}
	if (num_out == 0) return;

	//---- print the header ----

	fprintf (file, ((cost_flag) ? "DOLLARS" : "MINUTES"));

	for (i=0; i < num_modes; i++) {
		if (i < modes) {
			fprintf (file, "\t%s", label [i]);
		} else {
			fprintf (file, "\tMODE_%d", i);
		}
	}
	if (!diff_flag) {
		for (i=1; i < num_modes; i++) {
			if (i < modes) {
				fprintf (file, "\t%s_C", label [i]);
			} else {
				fprintf (file, "\tMODE_%d_C", i);
			}
		}
	}
	fprintf (file, "\n");

	//---- print each record ----
	
	num_out = 0;
	num = num_modes;
	if (!diff_flag) num = num * 2 - 1;

	for (j=start; j <= end; j++) {
		exe->Show_Progress ();

		k = (int) mode_distribution [0] [j];
		fprintf (file, "%d\t%d", j + min_difference, k);

		for (i=1; i < num; i++) {	
			value = mode_distribution [i] [j];
			if (k > 0) value /= k;
			fprintf (file, "\t%.2lf", value);
		}
		fprintf (file, "\n");
		num_out++;
	}
	exe->End_Progress ();

	mode_file.Close ();

	exe->Print (2, "Number of %s Records = %d", mode_file.File_Type (), num_out);
}
